/**
********************************************************************************
* @file    OS_SEM.h
* @author  Sai
* @version 01V01
* @date    2020-04-21
* @brief   OS_SEM.h
* @note    信号量头文件
* @attention
********************************************************************************
*/
#ifndef		_OS_SEM_H_
#define		_OS_SEM_H_

#include "os_type.h"
#include "os_event.h"



/**
*@brief 信号量数据结构
*/
typedef struct _SEM_T
{
		event_block_t tEvent;				/**< 事件控制块  */
		unsigned int unCount;			  /**< 信号量计数  */
		unsigned int unMaxCount;    /**< 信号计数最大值  */
}sem_t,*p_sem_t;


/**
 * @brief 信号量初始化
 * @param psem 信号量指针，unStartCnt 开始计数值，unMaxCnt 最大计数值
 * @note 
 * @retval 无
 */
void OS_SEM_Init(p_sem_t psem, unsigned int unStartCnt, unsigned int unMaxCnt);

/**
 * @brief 信号量等待操作
 * @param psem 信号量指针 unWaitTicks 等待Tick时间
 * @note 当检查到信号量计数值大于零时表示可以对临界区资源进行操作处理，当信号量计数值等于0时
 * 将当前任务放入等待队列中直至信号量计数值大于0为止
 * @retval 无
 */
int OS_SEM_Wait(p_sem_t psem, unsigned int unWaitTick);

/**
 * @brief 信号量无需等待操作
 * @param psem 信号量指针
 * @note 当检查到信号量计数值大于零时表示可以对临界区资源进行操作处理，当信号量计数值等于0时
 * 直接返回
 * @retval 无
 */
int OS_SEM_NoWait(p_sem_t psem);

/**
 * @brief 信号量通知操作
 * @param psem 信号量指针
 * @note 将信号量计数值增加处理
 * @retval 无
 */
void OS_SEM_Notify(p_sem_t psem);

#endif