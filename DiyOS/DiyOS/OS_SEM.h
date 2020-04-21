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


#endif