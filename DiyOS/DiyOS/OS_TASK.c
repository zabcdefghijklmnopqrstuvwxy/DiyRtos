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
#define			NVIC_SYSPRI2			0xE0000ED22
#define			NVIC_PENDSV_PRI		0x0000000FF

#define			MEM32(addr)				*(volatile unsigned long*)(addr)
#define			MEM8(addr)				*(volatile unsigned char*)(addr)
	
tTask *currentTask;   /**< 当前任务全局变量 */
tTask *nextTask;			/**< 下一个任务全局变量 */
p_tTask taskTable[32];

/**
* @brief 任务切换
* @param 无
* @note 内嵌汇编，将当前函数的参数变量入栈，将之前的函数的参数变量出栈
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
 * @brief 触发PendSV中断
 * @param 无
 * @note 通过触发PendSV中断进行任务切换
 * @retval 无
 */
void OS_TASK_Switch(void)
{
		MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
