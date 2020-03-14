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

#define   TASK_MAX_NUM			32   /**< OS下支持的最多任务数量 */


/**
* @brief 任务优先级,数值越低优先级越高最多支持32个优先级设置
*/
typedef enum
{
	 TASKPRI0,    /**< 优先级0 优先级最高*/
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
	 TASKPRI31,  /**< 优先级31，优先等级最低 */
}task_prio_t;


/**
*@brief 任务数据结构
*/
typedef struct _tTask{
		unsigned int *stack;    /**< 独立栈指针 */
	  unsigned int unDelay;   /**< 任务延时计数 */
	  unsigned int unPri;     /**< 任务优先级 */
}tTask,*p_tTask;

extern tTask *currentTask;   /**< 当前任务全局变量 */
extern tTask *nextTask;			 /**< 下一个任务全局变量 */
extern tTask *idleTask;      /**< 空闲任务全局变量 */

/**
 * @brief 任务初始化函数
 * @param task 任务独立栈，entry待保存的任务，param 任务的参数，prio 任务优先级，stack独立栈指针
 * @note 用户需要分配一片空间用于任务栈。即传入的stack变量为用户任务栈指针
 * @retval 无
 */
void OS_TASK_Init(tTask *task,void (*entry)(void*),void *param,task_prio_t prio,unsigned int *stack);

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

/**
 * @brief 获取最高优先级任务
 * @param 无
 * @note 无
 * @retval 返回最高优先级任务数据信息
 */
tTask* OS_TASK_HighestReadyTask(void);

#endif
