/**
********************************************************************************
* @file    OS_TASK.h
* @author  Sai
* @version 01V01
* @date    2019-12-29
* @brief   OS_TASK.h
* @note    
* @attention
********************************************************************************
*/
#ifndef _OS_TASK_H_
#define _OS_TASK_H_

/**
*@brief �������ݽṹ
*/
typedef struct _tTask{
		unsigned int *stack;    /**< ����ջָ�� */
	  unsigned int unDelay;   /**< ������ʱ���� */
}tTask,*p_tTask;

extern tTask *currentTask;   /**< ��ǰ����ȫ�ֱ��� */
extern tTask *nextTask;			 /**< ��һ������ȫ�ֱ��� */
extern tTask *idleTask;      /**< ��������ȫ�ֱ��� */

/**
 * @brief �����ʼ������
 * @param task �������ջ��entry�����������param ����Ĳ�����stack����ջָ��
 * @note �û���Ҫ����һƬ�ռ���������ջ���������stack����Ϊ�û�����ջָ��
 * @retval ��
 */
void OS_TASK_Init(tTask *task,void (*entry)(void*),void *param,unsigned int *stack);

/**
 * @brief ϵͳʱ������
 * @param[in] ms ��ʱ������ʱ��ʱ�� ��λms
 * @note None
 * @retval None
*/
void OS_TASK_SetSysTickPeriod(unsigned int ms);

/**
 * @brief ���е�һ��������
 * @param ��
 * @note ��
 * @retval ��
 */
void OS_TASK_RunFirst(void);

/**
 * @brief ���е�һ��������
 * @param ��
 * @note ��
 * @retval ��
 */
void OS_TASK_Sched(void);

/**
 * @brief ������ʱ����
 * @param[in] delay ��ʱ����ʱ��
 * @note ͨ��Sys_Tick�ж϶�delay�����Լ������Լ���0
 * @retval ��
 */
void OS_TASK_Delay(unsigned int delay);

#endif
