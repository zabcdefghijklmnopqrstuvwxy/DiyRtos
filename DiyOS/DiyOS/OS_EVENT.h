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
#include "OS_TASK.h"

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

/**
 * @brief 事件控制块等待
 * @param[in] pevent 事件块列表，ptask 任务句柄，msg消息，state 状态，timeout延时
 * @note 
 * @retval 返回错误码
 */
int OS_EVENT_Wait(p_event_block_t pevent,p_tTask ptask,void *msg, unsigned int state,unsigned int timeout);

/**
 * @brief 事件块唤醒
 * @param[in] pevent 事件块列表，msg事件块消息，result事件控制块等待结果
 * @note 
 * @retval 返回错误码
 */
int OS_EVENT_Wake(p_event_block_t pevent,void *msg,unsigned int result);

/**
 * @brief 事件控制块移除
 * @param[in] ptask 任务句柄，msg事件块类型，result事件消息
 * @note 
 * @retval 返回错误码
 */
int OS_EVENT_Delete(p_tTask ptask,void *msg,unsigned int result);

/**
 * @brief 清空所有事件控制块
 * @param[in] pevent 事件控制块数据指针，msg事件块类型，result事件消息
 * @note None
 * @retval 返回清空时还剩下多少等待控制块数量
 */
uint32_t OS_EVENT_ClearAll(p_event_block_t pevent,void *msg,unsigned int result);

/**
 * @brief 获取事件控制块数量
 * @param[in] pevent 事件控制块数据指针
 * @note None
 * @retval 返回清空时还剩下多少等待控制块数量
 */
uint32_t OS_EVENT_GetEventCount(p_event_block_t pevent);


#endif
