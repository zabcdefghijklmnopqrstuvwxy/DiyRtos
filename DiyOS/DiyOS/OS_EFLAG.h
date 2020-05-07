/**
********************************************************************************
* @file    OS_EFLAG.h
* @author  Sai
* @version 01V01
* @date    2020-05-06
* @brief   OS_EFLAG.h
* @note    事件标志组头文件
* @attention
********************************************************************************
*/

#ifndef _OS_EFLAG_H_
#define	_OS_EFLAG_H_

#include "OS_EVENT.h"


#define		FLAGGROUP_SET		(0x01)    /**< 事件标志置一*/
#define		FLAGGROUP_CLR		(0x02)	  /**< 事件标志清零 */
#define		FLAGGROUP_ALL		(0x04)	  /**< 事件标志所有位 */
#define		FLAGGROUP_ANY		(0x08)    /**< 事件标志任意位 */

#define		FLAGGROUP_SET_ALL	(FLAGGROUP_SET | FLAGGROUP_ALL)   /**< 所有位置一 */
#define		FLAGGROUP_SET_ANY	(FLAGGROUP_SET | FLAGGROUP_ANY)   /**< 任意位置一 */
#define		FLAGGROUP_CLR_ALL	(FLAGGROUP_CLR | FLAGGROUP_ALL)   /**< 所有位清零*/
#define		FLAGGROUP_CLR_ANY	(FLAGGROUP_CLR | FLAGGROUP_ANY)   /**< 任意位清零 */

#define		FLAGGROUP_CONSUME	(0x80)   /**< 消耗标志 */


/**
* @brief 事件标志组
*/
typedef struct _EFLAG_T
{
	event_block_t tEvent;		/**< 事件控制块 */
	unsigned int unFlag;		/**< 事件标志 */
}eflag_t,*p_eflag_t;

/**
* @brief 事件标志查询信息
*/
typedef struct _EFLAG_INFO_T
{
	unsigned int unFlag;			/**< 事件标志组的标志信息 */
	unsigned int unWaitTaskCnt;		/**< 事件标志组的等待任务数量 */
}eflag_info_t,*p_eflag_info_t;

/**
* @brief 事件标志组初始化
* @param[in] pevent 事件标志信息指针
* @note 初始化事件标志控制块链表
* @retval 返回0表示初始化成功，返回非0表示失败
*/
int OS_EFLAG_Init(p_eflag_t pevent, unsigned int unFlag);

/**
* @brief 事件标志等待
* @param[in] pevent 事件标志信息指针，unWaitType等待的事件类型, unRequestFlag 待等待的结果
* @note 查询事件标志位是否满足，如果满足则返回，否则加入等待事件标志队列中
* @retval 
*/
int OS_EFLAG_Wait(p_eflag_t pevent, unsigned int unWaitType, unsigned int unRequestFlag, unsigned int *unResultFlag, unsigned int unTimeout);

/**
* @brief 事件标志组不等待处理
* @param[in] pevent 事件标志信息指针，unSetMode设置模式，unFlag 设置的标志
* @note 
* @retval 
*/
int OS_EFLAG_NoWait(p_eflag_t pevent, unsigned int unWaitType, unsigned int unRequestFlag, unsigned int *unResultFlag);

/**
* @brief 事件标志组通知
* @param[in] pevent 事件标志信息指针，unSetMode设置模式，unFlag 设置的标志
* @note 遍历事件标志组等待链表，将符合事件标志条件的任务唤醒
* @retval 
*/
int OS_EFLAG_Notify(p_eflag_t pevent, unsigned int unSetMode, unsigned int unFlag);

/**
* @brief 事件标志组销毁处理
* @param[in] pevent 事件标志信息指针
* @note 将事件标志组队列销毁处理
* @retval 返回销毁时等待任务的个数数量
*/
unsigned int OS_EFLAG_Destroy(p_eflag_t pevent);

/**
* @brief 事件标志组销毁处理
* @param[in] pevent 事件标志信息指针，pinfo 事件标志组信息
* @note 将事件标志组队列销毁处理
* @retval 
*/
void OS_EFLAG_GetInfo(p_eflag_t pevent, p_eflag_info_t pinfo);

#endif
