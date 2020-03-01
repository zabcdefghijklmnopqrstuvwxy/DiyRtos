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
	
tTask *currentTask;   /**< ��ǰ����ȫ�ֱ��� */
tTask *nextTask;			/**< ��һ������ȫ�ֱ��� */
tTask *idleTask;
p_tTask taskTable[32];

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
	task->unDelay = 0;                     //��ʼ����ʱ����
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
 * @brief ������Ⱥ���
 * @param ��
 * @note ����һ�������赱ǰ���������л���
 * @retval ��
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
 * @brief ϵͳʱ���ж������л�
 * @param ��
 * @note ��ǰ������ʱ���������Լ�
 * @retval ��
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
		currentTask->unDelay = delay;
}

