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

#define   TASK_MAX_NUM			32   /**< OS下支持的最多任务数量 */
#define		TASK_MAX_SLICE		10	 /**< 任务时间片大小 */

/**
* @brief 任务优先级,数值越低优先级越高最多支持32个优先级设置
*/
typedef enum
{
	 TASKPRI0,    /**< 优先级0，优先级最高*/
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
*@brief 任务状态
*/
#define		  TASK_DELAYSTATUS        1     			/**< 任务延时状态 */
#define			TASK_SUSPENDSTATUS			2						/**< 任务挂起状态 */
#define			TASK_DESTROYSTATUS			4						/**< 任务销毁状态 */
#define			TASK_WAITEVENT					0x10000     /**< 任务事件等待 */


/*< 任务清理函数 */
typedef   int (*clearfn)(void *param);

struct _EVENT_BLOCK;

/**
*@brief 任务数据结构
*/
typedef struct _tTask{
		unsigned int *stack;       /**< 独立栈指针 */
	  node_t tLinkNode;					 /**< 同优先级链表 */ 
	  int nDelay;                /**< 任务延时计数 */
	  node_t tDelaynode;         /**< 延时任务节点 */
	  unsigned int unPri;        /**< 任务优先级 */
	  unsigned int tTaskState;   /**< 任务状态 */
	  int nSlice;			 					 /**< 任务时间片 */
	  int nSuspendCount;				 /**< 任务挂起次数 */
	  clearfn clearcb;					 /**< 清理函数 */
	  void *clearparam;          /**< 清理函数参数 */
	  int nDeleteFlag;           /**< 删除标志 */
	  struct _EVENT_BLOCK	*pevent;		 /**< 事件块管理*/
		void *eventmsg;						 /**< 事件块信息 */
	  unsigned int unEventResult;/**< 事件结果 */
}tTask,*p_tTask;

/**
*@brief 任务信息数据结构
*/
typedef struct _TASK_INFO{
		int nDelayTick;									/**< 任务延时tick计数 */
		unsigned int unPri;							/**< 任务优先级 */
		unsigned int unTaskState;				/**< 任务当前状态码 */
	  int nSlice;											/**< 任务时间片 */
}task_info_t,*p_task_info_t;

extern tTask *currentTask;   /**< 当前任务全局变量 */
extern tTask *nextTask;			 /**< 下一个任务全局变量 */
extern tTask *idleTask;      /**< 空闲任务全局变量 */

/**
 * @brief 任务系统初始化
 * @param 无
 * @note 初始化双向链表，设置PendSv优先级，初始化任务时间片默认为10ms
 * @retval 无
 */
void OS_TASK_OSInit(void);

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

/**
 * @brief 任务挂起
 * @param 无
 * @note 将任务从任务链表中删除
 * @retval 无
 */
void OS_TASK_SuspendTask(p_tTask ptask);

/**
 * @brief 任务唤醒
 * @param 无
 * @note 将任务加入到任务链表中
 * @retval 无
 */
void OS_TASK_WakeUpTask(p_tTask ptask);

/**
 * @brief 任务的删除请求
 * @param[in] 待删除的任务句柄
 * @note 将任务句柄中的删除标志置一即可
 * @retval 返回0表示删除成功，返回非0表示删除失败
 */
int OS_TASK_DeleteReq(p_tTask ptask);

/**
 * @brief 任务删除标志查询
 * @param[in] 待查询的任务句柄
 * @note None
 * @retval 如果需要删除则返回1，否则返回0
 */
int OS_TASK_IsDelete(p_tTask ptask);

/**
 * @brief 自身任务删除
 * @param None
 * @note None
 * @retval None
 */
void OS_TASK_SelfDelete(void);

/**
 * @brief 任务的强制删除
 * @param[in] 待删除的任务句柄
 * @note 将延迟队列
 * @retval 返回0表示删除成功，返回非0表示删除失败
 */
int OS_TASK_ForceDelete(p_tTask ptask);

/**
 * @brief 任务清理函数的注册
 * @param[in] ptask 待删除的任务句柄，fn待清除的回调函数，param回调函数参数
 * @note None
 * @retval None
 */
void OS_TASK_RegisterCLearFn(p_tTask ptask,clearfn fn,void* param);

/**
 * @brief 任务就绪处理
 * @param 无
 * @note 位图任务优先级设置，任务放入任务表中
 * @retval 无
 */
void OS_TASK_TaskRdy(p_tTask ptask);

/**
 * @brief 任务非就绪处理
 * @param 无
 * @note 位图任务优先级清除，从任务表中清零该任务项
 * @retval 无
 */
void OS_TASK_TaskUnRdy(p_tTask ptask);

/**
 * @brief 任务等待
 * @param[in] ptask 任务数据，unDelay 延时时间
 * @note 将延时任务加入到延时任务队列中，设置任务为延时状态
 * @retval 无
 */
void OS_TASK_DelayWait(p_tTask ptask,int nDelay);

/**
 * @brief 任务唤醒
 * @param 无
 * @note 将延时完成的任务从延时任务队列中删除，设置任务为就绪状态
 * @retval 无
 */
void OS_TASK_DelayWakeup(p_tTask ptask);

#endif
