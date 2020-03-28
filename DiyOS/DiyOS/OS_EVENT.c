/**
********************************************************************************
* @file    OS_EVENT.c
* @author  Sai
* @version 01V01
* @date    2020-03-28
* @brief   OS_EVENT.c
* @note    事件块管理源代码
* @attention
********************************************************************************
*/

#include "OS_EVENT.h"

/**
 * @brief 事件块初始化
 * @param[in] pevent 事件块列表，type事件块类型
 * @note 
 * @retval 返回错误码
 */
int OS_EVENT_Init(p_event_block_t pevent,event_type_t type)
{
	  pevent->tEventType = type;
		OS_COM_InitList(&pevent->tEventList);
}