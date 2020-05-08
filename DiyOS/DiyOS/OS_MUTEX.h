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
*@brief 互斥锁消息
*/
typedef struct _MUTEX_INFO_T
{
	unsigned int unLockCnt;				/**< 上锁计数 */
	p_tTask poritask;					/**< 任务结构 */
	unsigned int unOriPri;				/**< 原始的任务优先级 */
	unsigned int unInheritPri;			/**< 继承的优先级 */
}mutex_info_t,*p_mutex_info_t;


/**
 * @brief 互斥锁初始化
 * @param pmutex 信号量指针
 * @note 初始化事件控制块，将任务初始化为0，优先级清零
 * @retval 无
 */
void OS_MUTEX_Init(p_mutex_t pmutex);

/**
 * @brief 互斥锁上锁
 * @param pmutex 互斥锁指针，unTimeOut 超时时间
* @note 互斥锁上锁，资源已被锁定则加入到等待链表中
 * @retval 无
 */
int OS_MUTEX_Lock(p_mutex_t pmutex,unsigned int unTimeOut);

/**
 * @brief 互斥锁上锁
 * @param pmutex 互斥锁指针
 * @note 互斥锁上锁，试着上锁，如果不成功则直接返回
 * @retval 无
 */
int OS_MUTEX_TryLock(p_mutex_t pmutex);

/**
 * @brief 互斥锁解锁
 * @param pmutex 互斥锁指针
 * @note 互斥锁释放处理
 * @retval 无
 */
int OS_MUTEX_UnLock(p_mutex_t pmutex);




#endif
