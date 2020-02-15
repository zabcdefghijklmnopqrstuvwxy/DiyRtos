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
	
tTask *currentTask;   /**< ��ǰ����ȫ�ֱ��� */
tTask *nextTask;			/**< ��һ������ȫ�ֱ��� */
p_tTask taskTable[32];

/**
* @brief �����л�
* @param ��
* @note ��Ƕ��࣬����ǰ�����Ĳ���������ջ����֮ǰ�ĺ����Ĳ���������ջ
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
void OS_TASK_Init(tTask *task,void (*entry)(void*),void *param,unsigned int *stack)
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
 * @brief ����PendSV�ж�
 * @param ��
 * @note ͨ������PendSV�жϽ��������л�
 * @retval ��
 */
void OS_TASK_Switch(void)
{
		MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
