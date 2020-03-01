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

#ifdef USE_STM32F1
#include "stm32f1xx_hal.h"
#endif

#ifdef USE_STM32F4
#include "stm32f4xx_hal.h"
#endif

#define			NVIC_INT_CTRL			0xE000ED04
#define			NVIC_PENDSVSET		0x10000000
#define			NVIC_SYSPRI2			0xE000ED22
#define			NVIC_PENDSV_PRI		0x000000FF

#define			MEM32(addr)				*(volatile unsigned long*)(addr)
#define			MEM8(addr)				*(volatile unsigned char*)(addr)
	
tTask *currentTask;   /**< 当前任务全局变量 */
tTask *nextTask;			/**< 下一个任务全局变量 */
tTask *idleTask;
p_tTask taskTable[32];

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
void OS_TASK_Init(tTask *task,void (*entry)(void*),void *param,unsigned int *stack)
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
 * @brief 任务调度函数
 * @param 无
 * @note 将下一个任务赋予当前任务用于切换，
 * @retval 无
 */
void OS_TASK_Sched(void)
{	
		int i = 0;
		if(currentTask == idleTask)
		{
				for(i = 0; i < 2; i++)
			  {
						if(0 == taskTable[i]->unDelay)
						{
								nextTask = taskTable[i];
							  break;
						}
				}
				
				if(i >= 2)
				{
						return ;
				}
		}
		else
		{
			  for(i = 0; i < 2; i++)
				{
						if(0 == taskTable[i]->unDelay)
						{
								nextTask = taskTable[i];
							  break;
						}
				}
				
				if(i >= 2)
				{
						nextTask = idleTask;
				}
		}	
		
		OS_TASK_Switch();
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
	  for(i = 0; i < 2; i++)
		{
			  if(taskTable[i]->unDelay)
				{
					  taskTable[i]->unDelay--;
				}
		}
		
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
		currentTask->unDelay = delay;
}

