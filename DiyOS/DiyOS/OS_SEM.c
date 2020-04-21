/**
********************************************************************************
* @file    OS_SEM.c
* @author  Sai
* @version 01V01
* @date    2020-04-21
* @brief   OS_SEM.c
* @note    信号量源代码
* @attention
********************************************************************************
*/

#include "OS_SEM.h"

/**
 * @brief 信号量初始化
 * @param psem 信号量指针，unStartCnt 开始计数值，unMaxCnt 最大计数值
 * @note 
 * @retval 无
 */
void OS_SEM_Init(p_sem_t psem, unsigned int unStartCnt, unsigned int unMaxCnt)
{
		OS_EVENT_Init(&psem->tEvent, EVENT_SEM);
	
	  psem->unMaxCount = unMaxCnt;
	
	  if(0 == unMaxCnt)
		{
				psem->unCount = unStartCnt;
		}
		else
		{
			psem->unCount = unStartCnt > psem->unMaxCount ? psem->unMaxCount : unStartCnt;
		}
}