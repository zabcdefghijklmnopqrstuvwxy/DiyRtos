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


#endif
