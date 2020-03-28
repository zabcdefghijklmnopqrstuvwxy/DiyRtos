/**
********************************************************************************
* @file    OS_TASK.c
* @author  Sai
* @version 01V01
* @date    2019-12-29
* @brief   OS_TASK.c
* @note    task scheduling function  
* @attention
********************************************************************************
*/


#include "OS_TASK.h"

#define			NVIC_INT_CTRL			0xE000ED04
#define			NVIC_PENDSVSET		0x10000000
#define			NVIC_SYSPRI2			0xE000ED22
#define			NVIC_PENDSV_PRI		0x000000FF

#define			MEM32(addr)				*(volatile unsigned long*)(addr)
#define			MEM8(addr)				*(volatile unsigned char*)(addr)
	
tTask *currentTask;   								/**< ��ǰ����ȫ�ֱ��� */
tTask *nextTask;										  /**< ��һ������ȫ�ֱ��� */
tBitmap_t tBitmap;     								/**< λͼʵ���� */
nodelist_t tNodeList;  								/**< ˫������ʵ���� */
nodelist_t taskTable[TASK_MAX_NUM];   /**< ��������ʵ���� */

/*������*/
static unsigned int gunSchedule;

/**
* @brief �����л�
* @param ��
* @note ��Ƕ��࣬����ǰ�����Ĳ���������ջ����֮ǰ�ĺ����Ĳ���������ջ
* ���LR=0xFFFFFFF9˵�������쳣��ʱ��ʹ�õ���MSP�����LR=0xFFFFFFFD�������쳣��ʱ��ʹ�õ���PSP
* @return ��
*/
__asm void PendSV_Handler()
{
			IMPORT		currentTask
			IMPORT		nextTask
			MRS				r0,PSP
			CBZ				r0,PendSVHandler_nosave
			
			STMDB			r0!,{r4-r11}
			
			LDR				r1,=currentTask
			LDR				r1,[r1]
			STR       r0,[r1]
			
PendSVHandler_nosave
			
			LDR				r0,=currentTask
			LDR				r1,=nextTask
			LDR				r2,[r1]
			STR       r2,[r0]
			
			LDR				r0,[r2]
			LDMIA			r0!,{r4-r11}
			
			MSR				PSP,r0
			ORR				LR,LR,#0x04
			BX				LR
}

/**
 * @brief �����ʼ������
 * @param task �������ջ��entry�����������param ����Ĳ�����stack����ջָ��
 * @note �û���Ҫ����һƬ�ռ���������ջ���������stack����Ϊ�û�����ջָ��
 * @retval ��
 */
void OS_TASK_Init(tTask *task,void (*entry)(void*),void *param,task_prio_t prio,unsigned int *stack)
{
		*(--stack) = (unsigned long)(1 << 24); //cpsr��־����
		*(--stack) = (unsigned long)entry;		 //����������ڵ�ַ
		*(--stack) = (unsigned long)0x14;		   //����lr�Ĵ���ֵ
		*(--stack) = (unsigned long)0x12;		   //����R12�Ĵ���ֵ
		*(--stack) = (unsigned long)0x3;		   //����R3�Ĵ���ֵ
		*(--stack) = (unsigned long)0x2;		   //����R2�Ĵ���ֵ
		*(--stack) = (unsigned long)0x1;		   //����R1�Ĵ���ֵ
		*(--stack) = (unsigned long)param;		 //�����������ָ��

		*(--stack) = (unsigned long)0x11;		   //����R11�Ĵ���ֵ
		*(--stack) = (unsigned long)0x10;		   //����R10�Ĵ���ֵ
		*(--stack) = (unsigned long)0x9;		   //����R9�Ĵ���ֵ
		*(--stack) = (unsigned long)0x8;		   //����R8�Ĵ���ֵ
		*(--stack) = (unsigned long)0x7;		   //����R7�Ĵ���ֵ
		*(--stack) = (unsigned long)0x6;		   //����R6�Ĵ���ֵ
		*(--stack) = (unsigned long)0x5;		   //����R5�Ĵ���ֵ
		*(--stack) = (unsigned long)0x4;		   //����R4�Ĵ���ֵ

		task->stack = stack;					         //��������Ķ���ջ
		task->nDelay = 0;                      //��ʼ����ʱ����
		task->unPri = prio;                    //���ȼ���ʼ��
		task->tTaskState = 0;   							 //��������Ϊ����״̬
		task->nSlice = TASK_MAX_SLICE;				 //ʱ��Ƭ��ʼ��
		task->nSuspendCount = 0;							 //������������ʼ��
		task->clearcb = NULL;                  //��������ʼ��Ϊ��
		task->clearparam = NULL;							 //������������ʼ��Ϊ��
	  task->nDeleteFlag = 0;	               //ɾ����־���� 
		
		OS_COM_SetBitmap(&tBitmap,prio);       //���ȼ�λͼ����
		
	  OS_COM_AddNode(&taskTable[prio],&task->tLinkNode);  //ͬ���ȼ��������������
}

/**
 * @brief PendSV����
 * @param ��
 * @note ����PendSV�жϣ������ж�������ȼ����������ж���ɺ��ٴ��������л����ж�
 * @retval ��
 */
void OS_TASK_TriggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
} 

/**
 * @brief ����PendSV�ж�
 * @param ��
 * @note ͨ������PendSV�жϽ��������л�
 * @retval ��
 */
void OS_TASK_Switch(void)
{
		MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

/**
 * @brief ��ȡ������ȼ�����
 * @param ��
 * @note ��
 * @retval ����������ȼ�����������Ϣ
 */
tTask* OS_TASK_HighestReadyTask(void)
{
		node_t *pnode = NULL;
	  pnode = GET_FIRST_NODE(taskTable[OS_COM_GetFirstBit(&tBitmap)].head);
		return NODEPARENT(pnode,tTask,tLinkNode);
}

/**
 * @brief ������Ⱥ���
 * @param ��
 * @note ����һ�������赱ǰ���������л���
 * @retval ��
 */
void OS_TASK_Sched(void)
{	
	  unsigned int status;
	  tTask *tmptask = NULL;
	
	  if(gunSchedule)
		{
				return ;
		}
	
		status = OS_TASK_EnterCritical();
		
		tmptask = OS_TASK_HighestReadyTask();	
		if(tmptask != currentTask)   //��ȡ������������͵�ǰ������ͬ�򲻽����л�
		{
				nextTask = tmptask;
			  OS_TASK_Switch();

		}
		
	 	OS_TASK_ExitCritical(status);		
}

/**
 * @brief ������
 * @param ��
 * @note ����ʱ��ɵ��������ʱ���������ɾ������������Ϊ����״̬
 * @retval ��
 */
void OS_TASK_DelayWakeup(p_tTask ptask)
{
	  if(NULL == ptask)
		{
				return ;
		}
	
	  ptask->tTaskState = ptask->tTaskState & (~TASK_DELAYSTATUS);
		OS_COM_DelNode(&tNodeList,&ptask->tDelaynode);
}

/**
 * @brief ����ȴ�
 * @param[in] ptask �������ݣ�unDelay ��ʱʱ��
 * @note ����ʱ������뵽��ʱ��������У���������Ϊ��ʱ״̬
 * @retval ��
 */
void OS_TASK_DelayWait(p_tTask ptask,int nDelay)
{
	  if(NULL == ptask)
		{
				return ;
		}
	  ptask->nDelay = nDelay;
		ptask->tTaskState = ptask->tTaskState | TASK_DELAYSTATUS;
		OS_COM_AddNode(&tNodeList,&ptask->tDelaynode);
}

/**
 * @brief �����������
 * @param ��
 * @note λͼ�������ȼ����ã���������������
 * @retval ��
 */
void OS_TASK_TaskRdy(p_tTask ptask)
{
		OS_COM_AddNode(&taskTable[ptask->unPri],&ptask->tLinkNode);  //ͬ���ȼ��������������
		OS_COM_SetBitmap(&tBitmap,ptask->unPri);
}

/**
 * @brief ����Ǿ�������
 * @param ��
 * @note λͼ�������ȼ��������������������������
 * @retval ��
 */
void OS_TASK_TaskUnRdy(p_tTask ptask)
{
		OS_COM_DelNode(&taskTable[ptask->unPri],&ptask->tLinkNode);  //ͬ���ȼ�����������
	  
	  if(0 == OS_COM_GetNodeCount(&taskTable[ptask->unPri]))
		{
				OS_COM_ClrBitmap(&tBitmap,ptask->unPri);
		}
}

/**
 * @brief �������
 * @param ��
 * @note �����������������ɾ��,�����ǰ��������Ҫ�����������Ҫ����
 * �����л�����
 * @retval ��
 */
void OS_TASK_SuspendTask(p_tTask ptask)
{
	  unsigned int unStatus = 0;
	  unStatus = OS_TASK_EnterCritical();
    
		if(!(ptask->tTaskState & TASK_SUSPENDSTATUS))
		{
			if(0 == ptask->nSuspendCount)
			{
					ptask->tTaskState = ptask->tTaskState | TASK_SUSPENDSTATUS;
					OS_TASK_TaskUnRdy(ptask);
					ptask->nSuspendCount++;
				  
					if(currentTask == ptask)
					{
							OS_TASK_ExitCritical(unStatus);		
							OS_TASK_Sched();
					}
			}
		}
	  
		OS_TASK_ExitCritical(unStatus);		
}

/**
 * @brief ������
 * @param ��
 * @note ��������뵽����������
 * @retval ��
 */
void OS_TASK_WakeUpTask(p_tTask ptask)
{
	  unsigned int unStatus = 0;
	  unStatus = OS_TASK_EnterCritical();
	 
	  if(ptask->tTaskState & TASK_SUSPENDSTATUS)
		{
			  if(ptask->nSuspendCount > 0)
				{
					OS_TASK_TaskRdy(ptask);
					ptask->nSuspendCount--;
					ptask->tTaskState = ptask->tTaskState & (~TASK_SUSPENDSTATUS);
				}
		}		
	  
	  OS_TASK_ExitCritical(unStatus);		
}

/**
 * @brief ϵͳʱ���ж������л�
 * @param ��
 * @note ��ǰ������ʱ���������Լ�
 * @retval ��
 */
void OS_TASK_SystemTickHandler(void)
{
		int i = 0;
	  unsigned int status;
	  p_node_t pnode = tNodeList.head.next; 
	  p_tTask ptask = NULL;
	  status = OS_TASK_EnterCritical();
	  
		while(pnode != &tNodeList.head)
		{
				ptask = NODEPARENT(pnode,tTask,tDelaynode);
			  pnode = pnode->next;
			
			  if(ptask->tTaskState & TASK_SUSPENDSTATUS)
				{
						continue;
				}
			
				if(ptask->nDelay > 0)
				{
						ptask->nDelay--;
						continue;					
				}
				OS_TASK_DelayWakeup(ptask);
				OS_TASK_TaskRdy(ptask);					
		}
	
		if(0 == --currentTask->nSlice)
		{
				currentTask->nSlice = TASK_MAX_SLICE;
				OS_COM_DelNode(&taskTable[currentTask->unPri],&currentTask->tLinkNode);
				OS_COM_AddNode(&taskTable[currentTask->unPri],&currentTask->tLinkNode);
		}
		
	  OS_TASK_ExitCritical(status);		
		OS_TASK_Sched();
}

/**
* @brief ϵͳʱ���жϺ���
* @param ��
* @note ��
* @return ��
*/
void SysTick_Handler(void)
{
		OS_TASK_SystemTickHandler();
}

/**
 * @brief ϵͳʱ������
 * @param[in] ms ��ʱ������ʱ��ʱ�� ��λms
 * @note None
 * @retval None
*/
void OS_TASK_SetSysTickPeriod(unsigned int ms)
{
	SysTick->LOAD = ms * SystemCoreClock /1000 -1;  //��ʼ�����ؼ������Ĵ���ֵ
	NVIC_SetPriority(SysTick_IRQn,(1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;                               //�ݼ��Ĵ���ֵ
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;	
}

/**
 * @brief ���е�һ��������
 * @param ��
 * @note ��
 * @retval ��
 */
void OS_TASK_RunFirst(void)
{
		__set_PSP(0);
	  OS_TASK_TriggerPendSVC();	
}

/**
 * @brief ������ʱ����
 * @param[in] delay ��ʱ����ʱ��
 * @note ͨ��Sys_Tick�ж϶�delay�����Լ������Լ���0
 * @retval ��
 */
void OS_TASK_Delay(unsigned int delay)
{
	  unsigned int status = 0;
	  status = OS_TASK_EnterCritical();
	  OS_TASK_DelayWait(currentTask,delay);
	  OS_TASK_TaskUnRdy(currentTask);
		OS_TASK_ExitCritical(status);		
		OS_TASK_Sched();
}

/**
 * @brief �ٽ�������
 * @param None
 * @note �ر��жϣ����һ���ⲿ�жϺ������л����������ٽ���ִ�У�ֱ���ж�ʹ��
 * �ú������Է�ֹ�ٽ�������Ƕ�׵����⡣
 * @retval �����ж�״ֵ̬
 */
unsigned int OS_TASK_EnterCritical(void)
{
	   unsigned int unPrimask = __get_PRIMASK();
		 __disable_irq();
	
		 return unPrimask;
}

/**
 * @brief �ٽ����˳�
 * @param[in] status �ж�״ֵ̬
 * @note �˺���Ӧ��OS_TASK_EnterCritical���ʹ��
 * @retval None
 */
void OS_TASK_ExitCritical(unsigned int status)
{
		__set_PRIMASK(status);
}

/**
 * @brief ��������ʼ��
 * @param ��
 * @note ��������������ʼ��
 * @retval ��
 */
void OS_TASK_ScheduleInit(void)
{
		gunSchedule = 0;
}

/**
 * @brief ������ʹ��
 * @param ��
 * @note ������ʹ�ܣ��������������Լ�
 * @retval ��
 */
void OS_TASK_ScheduleEnable(void)
{
	  unsigned int unStatus = 0;
		unStatus = OS_TASK_EnterCritical();
	
	  gunSchedule++;
	
	  OS_TASK_ExitCritical(unStatus);
}	

/**
 * @brief ��������ʹ��
 * @param ��
 * @note ��������ʹ�ܣ��������������Լ�
 * @retval ��
 */
void OS_TASK_ScheduleDisable(void)
{
		unsigned int unStatus = 0;
		unStatus = OS_TASK_EnterCritical();
	
	  if(gunSchedule > 0)
		{
				gunSchedule--;
		}
	
	  OS_TASK_ExitCritical(unStatus);
}

/**
 * @brief �����ɾ������
 * @param[in] ��ɾ����������
 * @note ���������е�ɾ����־��һ����
 * @retval ����0��ʾɾ���ɹ������ط�0��ʾɾ��ʧ��
 */
int OS_TASK_DeleteReq(p_tTask ptask)
{
		unsigned int unStatus = 0;
	
	  if(NULL == ptask)
		{
				return -1;
		}
	
		unStatus = OS_TASK_EnterCritical();	 
	  ptask->nDeleteFlag = 1;
	 	OS_TASK_ExitCritical(unStatus);
		
		return 0;
}

/**
 * @brief ����ɾ����־��ѯ
 * @param[in] ����ѯ��������
 * @note None
 * @retval �����Ҫɾ���򷵻�1�����򷵻�0
 */
int OS_TASK_IsDelete(p_tTask ptask)
{
		if(NULL == ptask)
		{
				return -1;
		}
		
		return ptask->nDeleteFlag;
}


/**
 * @brief �����ɾ��
 * @param[in] ��ɾ����������
 * @note ������������������Ƴ�
 * @retval ����0��ʾ�Ƴ��ɹ������ط�0��ʾ�Ƴ�ʧ��
 */
int OS_TASK_ScheduleRemove(p_tTask ptask)
{
		unsigned int unStatus = 0;

		if(NULL == ptask)
		{
				return -1;
		}
		
		unStatus = OS_TASK_EnterCritical();	 
		
		OS_COM_DelNode(&taskTable[ptask->unPri],&ptask->tLinkNode);  //ͬ���ȼ�����������
	  
	  if(0 == OS_COM_GetNodeCount(&taskTable[ptask->unPri]))
		{
				OS_COM_ClrBitmap(&tBitmap,ptask->unPri);
		}
		
	  OS_TASK_ExitCritical(unStatus);
		
		return 0;
}

/**
 * @brief �ӳ�����ɾ��
 * @param[in] ��ɾ�����ӳ�������
 * @note ���ӳٶ��д��ӳٶ������Ƴ�
 * @retval ����0��ʾɾ���ɹ������ط�0��ʾɾ��ʧ��
 */
int OS_TASK_DelayRemove(p_tTask ptask)
{
	  unsigned int unStatus = 0;

		if(NULL == ptask)
		{
				return -1;
		}
	
	  unStatus = OS_TASK_EnterCritical();	 
		
	  ptask->tTaskState = ptask->tTaskState & (~TASK_DELAYSTATUS);
		OS_COM_DelNode(&tNodeList,&ptask->tDelaynode);
		
    OS_TASK_ExitCritical(unStatus);
		
		return 0;
}

/**
 * @brief ������������ע��
 * @param[in] ptask ��ɾ������������fn������Ļص�������param�ص���������
 * @note None
 * @retval None
 */
void OS_TASK_RegisterCLearFn(p_tTask ptask,clearfn fn,void* param)
{
		unsigned int unStatus = 0;
	  unStatus = OS_TASK_EnterCritical();	 

		ptask->clearcb = fn;
	  ptask->clearparam = param;
	
	  OS_TASK_ExitCritical(unStatus);
}


/**
 * @brief �����ǿ��ɾ��
 * @param[in] ��ɾ����������
 * @note ���ӳٶ���
 * @retval ����0��ʾɾ���ɹ������ط�0��ʾɾ��ʧ��
 */
int OS_TASK_ForceDelete(p_tTask ptask)
{
		if(NULL == ptask)
		{
				return -1;
		}
		
		OS_TASK_ScheduleRemove(ptask);
		
		OS_TASK_DelayRemove(ptask);
		
		ptask->tTaskState = ptask->tTaskState | TASK_DESTROYSTATUS;

		if(ptask->clearcb)
		{
				ptask->clearcb(ptask->clearparam);
		}
		
		if(currentTask == ptask)   //��ǰ�����ɾ������һ��ʱ����������Ⱥ���
		{
				OS_TASK_Sched();
		}
		
		return 0;
}

/**
 * @brief ��������ɾ��
 * @param None
 * @note None
 * @retval None
 */
void OS_TASK_SelfDelete(void)
{
		OS_TASK_ScheduleRemove(currentTask);
	  
		if(currentTask->clearcb)   //���������������������
		{
				currentTask->clearcb(currentTask->clearparam);
		}

		currentTask->tTaskState = currentTask->tTaskState | TASK_DESTROYSTATUS;

	  OS_TASK_Sched();	  //��ǰ����ɾ���󣬵���������Ⱥ���	
}

/**
 * @brief ����״̬�Ĳ�ѯ
 * @param[in] ptask ��������pinfo ����״̬��Ϣָ��
 * @note None
 * @retval ����-1��ʾ�����Ƿ�������0��ʾ����
 */
int OS_TASK_GetTaskInfo(p_tTask ptask,p_task_info_t pinfo)
{
		unsigned int unStatus = 0;
		if(NULL == ptask || NULL == pinfo)
		{
				return -1;
		}

	  unStatus = OS_TASK_EnterCritical();	 
		
		pinfo->nDelayTick = ptask->nDelay;
		pinfo->nSlice = ptask->nSlice;
		pinfo->unPri = ptask->unPri;
		pinfo->unTaskState = ptask->tTaskState;
		OS_TASK_ExitCritical(unStatus);		
}

/**
 * @brief ����ϵͳ��ʼ��
 * @param ��
 * @note ��ʼ��˫����������PendSv���ȼ�����ʼ������ʱ��ƬĬ��Ϊ10ms
 * @retval ��
 */
void OS_TASK_OSInit(void)
{
	  int i = 0;
	   __set_PSP(0);
	  MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	
		OS_TASK_SetSysTickPeriod(10);   /**< ��������ʱ��Ƭ��Ĭ��Ϊ10ms */
		OS_TASK_ScheduleInit();
		OS_COM_InitList(&tNodeList);  //���������ʼ��
	
	  for(i = 0; i < TASK_MAX_NUM; i++)
		{
				OS_COM_InitList(&taskTable[i]);  //���������ʼ��
		}
}
