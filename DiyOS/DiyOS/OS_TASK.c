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

#ifdef USE_STM32F1
#include "stm32f1xx_hal.h"
#endif

#ifdef USE_STM32F4
#include "stm32f4xx_hal.h"
#endif

#include "OS_TASK.h"
#include "OS_COM.h"

#define			NVIC_INT_CTRL			0xE000ED04
#define			NVIC_PENDSVSET		0x10000000
#define			NVIC_SYSPRI2			0xE000ED22
#define			NVIC_PENDSV_PRI		0x000000FF

#define			MEM32(addr)				*(volatile unsigned long*)(addr)
#define			MEM8(addr)				*(volatile unsigned char*)(addr)
	
tTask *currentTask;   /**< ��ǰ����ȫ�ֱ��� */
tTask *nextTask;			/**< ��һ������ȫ�ֱ��� */
tTask *idleTask;
tBitmap_t tBitmap;    /**< λͼʵ���� */

p_tTask taskTable[TASK_MAX_NUM];

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
		task->unDelay = 0;                     //��ʼ����ʱ����
		task->unPri = prio;

		taskTable[prio] = task;

		OS_COM_SetBitmap(&tBitmap,prio);
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
		return taskTable[OS_COM_GetFirstBit(&tBitmap)];
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
 * @brief ϵͳʱ���ж������л�
 * @param ��
 * @note ��ǰ������ʱ���������Լ�
 * @retval ��
 */
void OS_TASK_SystemTickHandler(void)
{
		int i = 0;
	  unsigned int status;
	  status = OS_TASK_EnterCritical();
	  for(i = 0; i < TASK_MAX_NUM; i++)
		{
			  if(taskTable[i])
				{
						if(taskTable[i]->unDelay)
						{
							  if(taskTable[i]->unDelay > 0)
								{
										taskTable[i]->unDelay--;
								}
						}
						else
						{
								OS_COM_SetBitmap(&tBitmap,taskTable[i]->unPri);
						}
				}
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
		currentTask->unDelay = delay;
	  OS_COM_ClrBitmap(&tBitmap,currentTask->unPri);
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
