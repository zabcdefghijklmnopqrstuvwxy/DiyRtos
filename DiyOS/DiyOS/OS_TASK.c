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
	
tTask *currentTask;    /**< 当前任务全局变量 */
tTask *nextTask;			 /**< 下一个任务全局变量 */
tBitmap_t tBitmap;     /**< 位图实例化 */
nodelist_t tNodeList;  /**< 双向链表实例化 */

p_tTask taskTable[TASK_MAX_NUM];

/*调度锁*/
static unsigned int gunSchedule;

/**
* @brief 任务切换
* @param 无
* @note 内嵌汇编，将当前函数的参数变量入栈，将之前的函数的参数变量出栈
* 如果LR=0xFFFFFFF9说明产生异常的时候使用的是MSP，如果LR=0xFFFFFFFD明产生异常的时候使用的是PSP
* @return 无
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
 * @brief 任务初始化函数
 * @param task 任务独立栈，entry待保存的任务，param 任务的参数，stack独立栈指针
 * @note 用户需要分配一片空间用于任务栈。即传入的stack变量为用户任务栈指针
 * @retval 无
 */
void OS_TASK_Init(tTask *task,void (*entry)(void*),void *param,task_prio_t prio,unsigned int *stack)
{
		*(--stack) = (unsigned long)(1 << 24); //cpsr标志设置
		*(--stack) = (unsigned long)entry;		 //保存任务入口地址
		*(--stack) = (unsigned long)0x14;		   //保存lr寄存器值
		*(--stack) = (unsigned long)0x12;		   //保存R12寄存器值
		*(--stack) = (unsigned long)0x3;		   //保存R3寄存器值
		*(--stack) = (unsigned long)0x2;		   //保存R2寄存器值
		*(--stack) = (unsigned long)0x1;		   //保存R1寄存器值
		*(--stack) = (unsigned long)param;		 //保存任务参数指针

		*(--stack) = (unsigned long)0x11;		   //保存R11寄存器值
		*(--stack) = (unsigned long)0x10;		   //保存R10寄存器值
		*(--stack) = (unsigned long)0x9;		   //保存R9寄存器值
		*(--stack) = (unsigned long)0x8;		   //保存R8寄存器值
		*(--stack) = (unsigned long)0x7;		   //保存R7寄存器值
		*(--stack) = (unsigned long)0x6;		   //保存R6寄存器值
		*(--stack) = (unsigned long)0x5;		   //保存R5寄存器值
		*(--stack) = (unsigned long)0x4;		   //保存R4寄存器值

		task->stack = stack;					         //保存任务的独立栈
		task->unDelay = 0;                     //初始化延时计数
		task->unPri = prio;                    //优先级初始化
		task->tTaskState = TASK_READYSTATUS;   //设置任务为就绪状态
		taskTable[prio] = task;

		OS_COM_SetBitmap(&tBitmap,prio);       //优先级位图设置
		
		OS_COM_AddNode(&tNodeList,&taskTable[prio]->tDelaynode);  //延时任务加入链表中
}

/**
 * @brief PendSV触发
 * @param 无
 * @note 触发PendSV中断，设置中断最低优先级，等其他中断完成后再处理任务切换的中断
 * @retval 无
 */
void OS_TASK_TriggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
} 

/**
 * @brief 触发PendSV中断
 * @param 无
 * @note 通过触发PendSV中断进行任务切换
 * @retval 无
 */
void OS_TASK_Switch(void)
{
		MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

/**
 * @brief 获取最高优先级任务
 * @param 无
 * @note 无
 * @retval 返回最高优先级任务数据信息
 */
tTask* OS_TASK_HighestReadyTask(void)
{
		return taskTable[OS_COM_GetFirstBit(&tBitmap)];
}

/**
 * @brief 任务调度函数
 * @param 无
 * @note 将下一个任务赋予当前任务用于切换，
 * @retval 无
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
		if(tmptask != currentTask)   //获取到的最新任务和当前任务相同则不进行切换
		{
				nextTask = tmptask;
			  OS_TASK_Switch();

		}
		
	 	OS_TASK_ExitCritical(status);		
}

/**
 * @brief 任务唤醒
 * @param 无
 * @note 将延时完成的任务从延时任务队列中删除，设置任务为就绪状态
 * @retval 无
 */
void OS_TASK_DelayWakeup(p_tTask ptask)
{
	  if(NULL == ptask)
		{
				return ;
		}
	
	  ptask->tTaskState = TASK_READYSTATUS;
		OS_COM_DelNode(&tNodeList,&ptask->tDelaynode);
}

/**
 * @brief 任务等待
 * @param[in] ptask 任务数据，unDelay 延时时间
 * @note 将延时任务加入到延时任务队列中，设置任务为延时状态
 * @retval 无
 */
void OS_TASK_DelayWait(p_tTask ptask,unsigned int unDelay)
{
	  if(NULL == ptask)
		{
				return ;
		}
	  ptask->unDelay = unDelay;
		ptask->tTaskState = TASK_DELAYSTATUS;
		OS_COM_AddNode(&tNodeList,&ptask->tDelaynode);
}

/**
 * @brief 延时任务就绪处理
 * @param 无
 * @note 位图任务优先级设置，任务放入任务表中
 * @retval 无
 */
void OS_TASK_DelayTaskRdy(p_tTask ptask)
{
		taskTable[ptask->unPri] = ptask;
		OS_COM_SetBitmap(&tBitmap,ptask->unPri);
}

/**
 * @brief 延时任务清除
 * @param 无
 * @note 位图任务优先级清除，从任务表中清零该任务项
 * @retval 无
 */
void OS_TASK_DelayTaskWait(p_tTask ptask)
{
		taskTable[ptask->unPri] = (p_tTask)0;
	  OS_COM_ClrBitmap(&tBitmap,ptask->unPri);
}

/**
 * @brief 系统时钟中断任务切换
 * @param 无
 * @note 当前任务延时计数进行自减
 * @retval 无
 */
void OS_TASK_SystemTickHandler(void)
{
		int i = 0;
	  unsigned int status;
	  p_node_t pnode = NULL; 
	  p_tTask ptask = NULL;
	  status = OS_TASK_EnterCritical();
	  
		for(pnode = tNodeList.head.next; pnode != &tNodeList.head; pnode = pnode->next)
		{
				ptask = NODEPARENT(pnode,tTask,tDelaynode);
			  
			  if(--ptask->unDelay)
				{
						OS_TASK_DelayWakeup(ptask);
						OS_TASK_DelayTaskRdy(ptask);				
				}
		}
	
	  OS_TASK_ExitCritical(status);		
		OS_TASK_Sched();
}


/**
* @brief 系统时钟中断函数
* @param 无
* @note 无
* @return 无
*/
void SysTick_Handler(void)
{
		OS_TASK_SystemTickHandler();
}

/**
 * @brief 系统时钟设置
 * @param[in] ms 定时器设置时钟时间 单位ms
 * @note None
 * @retval None
*/
void OS_TASK_SetSysTickPeriod(unsigned int ms)
{
	SysTick->LOAD = ms * SystemCoreClock /1000 -1;  //初始化重载计数器寄存器值
	NVIC_SetPriority(SysTick_IRQn,(1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;                               //递减寄存器值
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;	
}

/**
 * @brief 运行第一个多任务
 * @param 无
 * @note 无
 * @retval 无
 */
void OS_TASK_RunFirst(void)
{
		__set_PSP(0);
	  OS_TASK_TriggerPendSVC();	
}

/**
 * @brief 任务延时函数
 * @param[in] delay 延时计数时间
 * @note 通过Sys_Tick中断对delay进行自减，当自减至0
 * @retval 无
 */
void OS_TASK_Delay(unsigned int delay)
{
	  unsigned int status = 0;
	  status = OS_TASK_EnterCritical();
	  OS_TASK_DelayWait(currentTask,delay);
	  OS_TASK_DelayTaskWait(currentTask);
		OS_TASK_ExitCritical(status);		
		OS_TASK_Sched();
}


/**
 * @brief 临界区进入
 * @param None
 * @note 关闭中断，如此一来外部中断和任务切换都不会打断临界区执行，直至中断使能
 * 该函数可以防止临界区进入嵌套的问题。
 * @retval 返回中断状态值
 */
unsigned int OS_TASK_EnterCritical(void)
{
	   unsigned int unPrimask = __get_PRIMASK();
		 __disable_irq();
	
		 return unPrimask;
}

/**
 * @brief 临界区退出
 * @param[in] status 中断状态值
 * @note 此函数应与OS_TASK_EnterCritical配合使用
 * @retval None
 */
void OS_TASK_ExitCritical(unsigned int status)
{
		__set_PRIMASK(status);
}

/**
 * @brief 调度锁初始化
 * @param 无
 * @note 调度锁计数器初始化
 * @retval 无
 */
void OS_TASK_ScheduleInit(void)
{
		gunSchedule = 0;
}

/**
 * @brief 调度锁使能
 * @param 无
 * @note 调度锁使能，调度锁计数器自加
 * @retval 无
 */
void OS_TASK_ScheduleEnable(void)
{
	  unsigned int unStatus = 0;
		unStatus = OS_TASK_EnterCritical();
	
	  gunSchedule++;
	
	  OS_TASK_ExitCritical(unStatus);
}	

/**
 * @brief 调度锁不使能
 * @param 无
 * @note 调度锁不使能，调度锁计数器自减
 * @retval 无
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
 * @brief 任务系统初始化
 * @param 无
 * @note 初始化双向链表，设置PendSv优先级，初始化任务时间片默认为10ms
 * @retval 无
 */
void OS_TASK_OSInit(void)
{
	   __set_PSP(0);
	  MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	
		OS_TASK_SetSysTickPeriod(10);   /**< 设置任务时间片，默认为10ms */
	  OS_COM_InitList(&tNodeList);    /**< 初始化双向链表 */
}
