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
*@brief 任务数据结构
*/
typedef struct _tTask{
		unsigned int *stack;    /**< 独立栈指针 */
}tTask,*p_tTask;

extern tTask *currentTask;   /**< 当前任务全局变量 */
extern tTask *nextTask;			 /**< 下一个任务全局变量 */

/**
 * @brief 任务初始化函数
 * @param task 任务独立栈，entry待保存的任务，param 任务的参数，stack独立栈指针
 * @note 用户需要分配一片空间用于任务栈。即传入的stack变量为用户任务栈指针
 * @retval 无
 */
void OS_TASK_Init(tTask *task,void (*entry)(void*),void *param,unsigned int *stack);

/**
 * @brief 系统时钟设置
 * @param[in] ms 定时器设置时钟时间 单位ms
 * @note None
 * @retval None
*/
void OS_TASK_SetSysTickPeriod(unsigned int ms);

/**
 * @brief 运行第一个多任务
 * @param 无
 * @note 无
 * @retval 无
 */
void OS_TASK_RunFirst(void);

/**
 * @brief 运行第一个多任务
 * @param 无
 * @note 无
 * @retval 无
 */
void OS_TASK_Sched(void);

#endif
