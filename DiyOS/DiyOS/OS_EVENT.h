/**
********************************************************************************
* @file    OS_EVENT.h
* @author  Sai
* @version 01V01
* @date    2020-03-28
* @brief   OS_EVENT.h
* @note    事件块管理头文件
* @attention
********************************************************************************
*/

#ifndef		_OS_EVENT_H_
#define		_OS_EVENT_H_

#include "OS_COM.h"

/**
*@brief 事件结果
*/
typedef enum _EVENT_RESULT{
		EVENT_OK
}event_result_t;

/**
*@brief 事件块类型
*/
typedef enum _EVENT_TYPE{
		EVENT_UNKNOWN									/**< 未知类型 */
}event_type_t;

/**
*@brief 事件块管理结构
*/
typedef struct _EVENT_BLOCK{
		event_type_t tEventType;			/**< 事件块类型 */
		nodelist_t	tEventList;				/**< 事件列表 */
}event_block_t,*p_event_block_t;

/**
 * @brief 事件块初始化
 * @param[in] pevent 事件块列表，type事件块类型
 * @note 
 * @retval 返回错误码
 */
int OS_EVENT_Init(p_event_block_t pevent,event_type_t type);

#endif