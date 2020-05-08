/**
********************************************************************************
* @file    OS_MUTEX.h
* @author  Sai
* @version 01V01
* @date    2020-05-07
* @brief   OS_MUTEX.h
* @note    互斥锁头文件
* @attention
********************************************************************************
*/

#ifndef _OS_MUTEX_H_
#define	_OS_MUTEX_H_

#include "os_type.h"
#include "os_event.h"

/**
*@brief 互斥锁数据结构
*/
typedef struct _MUTEX_T
{
	event_block_t tEvent;				/**< 事件控制块  */
	unsigned int unLockCnt;				/**< 上锁计数 */
	p_tTask ptask;						/**< 任务结构 */
	unsigned int unOriPri;				/**< 原始的任务优先级 */
}mutex_t,*p_mutex_t;

/**
 * @brief 互斥锁初始化
 * @param pmutex 信号量指针
 * @note 初始化事件控制块，将任务初始化为0，优先级清零
 * @retval 无
 */
void OS_MUTEX_Init(p_mutex_t pmutex);










#endif
