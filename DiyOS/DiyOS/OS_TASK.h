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
	  unsigned int unDelay;   /**< 任务延时计数 */
}tTask,*p_tTask;

extern tTask *currentTask;   /**< 当前任务全局变量 */
extern tTask *nextTask;			 /**< 下一个任务全局变量 */
extern tTask *idleTask;      /**< 空闲任务全局变量 */

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

/**
 * @brief 任务延时函数
 * @param[in] delay 延时计数时间
 * @note 通过Sys_Tick中断对delay进行自减，当自减至0
 * @retval 无
 */
void OS_TASK_Delay(unsigned int delay);

/**
 * @brief 临界区进入
 * @param None
 * @note 关闭中断，如此一来外部中断和任务切换都不会打断临界区执行，直至中断使能
 * @retval 返回中断状态值
 */
unsigned int OS_TASK_EnterCritical(void);

/**
 * @brief 临界区退出
 * @param[in] status 中断状态值
 * @note 此函数应与OS_TASK_EnterCritical配合使用
 * @retval None
 */
void OS_TASK_ExitCritical(unsigned int status);

/**
 * @brief 调度锁初始化
 * @param 无
 * @note 调度锁计数器初始化
 * @retval 无
 */
void OS_TASK_ScheduleInit(void);

/**
 * @brief 调度锁使能
 * @param 无
 * @note 调度锁使能，调度锁计数器自加
 * @retval 无
 */
void OS_TASK_ScheduleEnable(void);

/**
 * @brief 调度锁不使能
 * @param 无
 * @note 调度锁不使能，调度锁计数器自减
 * @retval 无
 */
void OS_TASK_ScheduleDisable(void);

#endif
