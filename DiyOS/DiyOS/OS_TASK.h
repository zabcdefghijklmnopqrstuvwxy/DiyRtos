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
}tTask,*p_tTask;

extern tTask *currentTask;   /**< ��ǰ����ȫ�ֱ��� */
extern tTask *nextTask;			 /**< ��һ������ȫ�ֱ��� */

/**
 * @brief �����ʼ������
 * @param task �������ջ��entry�����������param ����Ĳ�����stack����ջָ��
 * @note �û���Ҫ����һƬ�ռ���������ջ���������stack����Ϊ�û�����ջָ��
 * @retval ��
 */
void OS_TASK_Init(tTask *task,void (*entry)(void*),void *param,unsigned int *stack);
	
/**
 * @brief ���е�һ��������
 * @param ��
 * @note ��
 * @retval ��
 */
void OS_TASK_RunFirst(void);

/**
 * @brief ����PendSV�ж�
 * @param ��
 * @note ͨ������PendSV�жϽ��������л�
 * @retval ��
 */
void OS_TASK_Switch(void);




#endif
