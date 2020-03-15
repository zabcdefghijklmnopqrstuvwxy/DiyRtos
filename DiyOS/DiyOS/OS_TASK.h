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


#ifdef USE_STM32F1
#include "stm32f1xx_hal.h"
#endif

#ifdef USE_STM32F4
#include "stm32f4xx_hal.h"
#endif

#include "OS_COM.h"


#define   TASK_MAX_NUM			32   /**< OS��֧�ֵ������������ */


/**
* @brief �������ȼ�,��ֵԽ�����ȼ�Խ�����֧��32�����ȼ�����
*/
typedef enum
{
	 TASKPRI0,    /**< ���ȼ�0�����ȼ����*/
	 TASKPRI1,
	 TASKPRI2,
	 TASKPRI3,
	 TASKPRI4,
	 TASKPRI5,
	 TASKPRI6,
	 TASKPRI7,
	 TASKPRI8,
	 TASKPRI9,
	 TASKPRI10,
	 TASKPRI11,
	 TASKPRI12,
	 TASKPRI13,
	 TASKPRI14,
	 TASKPRI15,
	 TASKPRI16,
	 TASKPRI17,
	 TASKPRI18,
	 TASKPRI19,
	 TASKPRI20,
	 TASKPRI21,
	 TASKPRI22,
	 TASKPRI23,
	 TASKPRI24,
	 TASKPRI25,
	 TASKPRI26,
	 TASKPRI27,
	 TASKPRI28,
	 TASKPRI29,
	 TASKPRI30,
	 TASKPRI31,  /**< ���ȼ�31�����ȵȼ���� */
}task_prio_t;

/**
*@brief ����״̬
*/
typedef enum TASKSTATUS{
		TASK_DELAYSTATUS,    /**< ��������ʱ״̬ */
	  TASK_READYSTATUS     /**< �����ھ���״̬ */
}task_status_t;

/**
*@brief �������ݽṹ
*/
typedef struct _tTask{
		unsigned int *stack;       /**< ����ջָ�� */
	  unsigned int unDelay;      /**< ������ʱ���� */
	  node_t tDelaynode;         /**< ��ʱ����ڵ� */
	  unsigned int unPri;        /**< �������ȼ� */
	  task_status_t tTaskState;  /**< ����״̬ */
}tTask,*p_tTask;

extern tTask *currentTask;   /**< ��ǰ����ȫ�ֱ��� */
extern tTask *nextTask;			 /**< ��һ������ȫ�ֱ��� */
extern tTask *idleTask;      /**< ��������ȫ�ֱ��� */


/**
 * @brief ����ϵͳ��ʼ��
 * @param ��
 * @note ��ʼ��˫����������PendSv���ȼ�����ʼ������ʱ��ƬĬ��Ϊ10ms
 * @retval ��
 */
void OS_TASK_OSInit(void);

/**
 * @brief �����ʼ������
 * @param task �������ջ��entry�����������param ����Ĳ�����prio �������ȼ���stack����ջָ��
 * @note �û���Ҫ����һƬ�ռ���������ջ���������stack����Ϊ�û�����ջָ��
 * @retval ��
 */
void OS_TASK_Init(tTask *task,void (*entry)(void*),void *param,task_prio_t prio,unsigned int *stack);

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

/**
 * @brief �ٽ�������
 * @param None
 * @note �ر��жϣ����һ���ⲿ�жϺ������л����������ٽ���ִ�У�ֱ���ж�ʹ��
 * @retval �����ж�״ֵ̬
 */
unsigned int OS_TASK_EnterCritical(void);

/**
 * @brief �ٽ����˳�
 * @param[in] status �ж�״ֵ̬
 * @note �˺���Ӧ��OS_TASK_EnterCritical���ʹ��
 * @retval None
 */
void OS_TASK_ExitCritical(unsigned int status);

/**
 * @brief ��������ʼ��
 * @param ��
 * @note ��������������ʼ��
 * @retval ��
 */
void OS_TASK_ScheduleInit(void);

/**
 * @brief ������ʹ��
 * @param ��
 * @note ������ʹ�ܣ��������������Լ�
 * @retval ��
 */
void OS_TASK_ScheduleEnable(void);

/**
 * @brief ��������ʹ��
 * @param ��
 * @note ��������ʹ�ܣ��������������Լ�
 * @retval ��
 */
void OS_TASK_ScheduleDisable(void);

/**
 * @brief ��ȡ������ȼ�����
 * @param ��
 * @note ��
 * @retval ����������ȼ�����������Ϣ
 */
tTask* OS_TASK_HighestReadyTask(void);

#endif
