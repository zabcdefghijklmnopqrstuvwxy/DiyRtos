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

	return 0;
}

/**
 * @brief 事件控制块等待
 * @param[in] pevent 事件块列表，ptask 任务句柄，msg消息，state 状态，timeout延时
 * @note 
 * @retval 返回错误码
 */
int OS_EVENT_Wait(p_event_block_t pevent,p_tTask ptask,void *msg, unsigned int state,unsigned int timeout)
{
	unsigned int status;

	if(NULL == pevent || NULL == ptask)
	{
		return -1;
	}

	status = OS_TASK_EnterCritical();
	ptask->tTaskState = ptask->tTaskState | state;
	ptask->pevent = pevent;
	ptask->eventmsg = msg;
	ptask->unEventResult = EVENT_OK;

	OS_TASK_TaskUnRdy(ptask);

	OS_COM_AddNode(&pevent->tEventList,&ptask->tLinkNode);

	OS_TASK_ExitCritical(status);		

	return 0;
}

/**
 * @brief 事件块唤醒
 * @param[in] pevent 事件块列表，msg事件块消息，result事件控制块等待结果
 * @note 
 * @retval 返回错误码
 */
p_tTask OS_EVENT_Wake(p_event_block_t pevent,void *msg,unsigned int result)
{
	unsigned int status;	
	status = OS_TASK_EnterCritical();
	p_node_t pnode = NULL;
	p_tTask ptask = NULL;

	if((pnode= OS_COM_RemoveFirstNode(&pevent->tEventList)) != NULL)
	{
		ptask = NODEPARENT(pnode, tTask, tLinkNode);
		ptask->pevent = NULL;
		ptask->eventmsg = msg;
		ptask->unEventResult = result;
		ptask->tTaskState = ptask->tTaskState & (~TASK_WAITEVENT); 

		if(ptask->nDelay != 0)
		{
			OS_TASK_DelayWakeup(ptask);
		}

		OS_TASK_TaskRdy(ptask);
	}

	OS_TASK_ExitCritical(status);	

	return ptask;
}

/**
 * @brief 事件控制块移除
 * @param[in] ptask 任务句柄，msg事件块类型，result事件消息
 * @note 
 * @retval 返回错误码
 */
int OS_EVENT_Delete(p_tTask ptask,void *msg,unsigned int result)
{
	unsigned int status;	
	if(NULL == ptask)
	{
		return -1;
	}

	status = OS_TASK_EnterCritical();

	OS_COM_DelNode(&ptask->pevent->tEventList,&ptask->tLinkNode);
	ptask->pevent = NULL;
	ptask->eventmsg = msg;
	ptask->unEventResult = result;
	ptask->tTaskState = ptask->tTaskState & (~TASK_WAITEVENT); 

	OS_TASK_ExitCritical(status);	

	return 0;
}

/**
 * @brief 清空所有事件控制块
 * @param[in] pevent 事件控制块数据指针，msg事件块类型，result事件消息
 * @note None
 * @retval 返回清空时还剩下多少等待控制块数量
 */
uint32_t OS_EVENT_ClearAll(p_event_block_t pevent,void *msg,unsigned int result)
{
	unsigned int status;	
	unsigned int unCount;
	p_node_t pnode = NULL;
	p_tTask ptask = NULL;
	status = OS_TASK_EnterCritical();
		
	unCount = OS_COM_GetNodeCount(&pevent->tEventList);

	while((pnode= OS_COM_RemoveFirstNode(&pevent->tEventList)) != NULL)
	{
		ptask = NODEPARENT(pnode, tTask, tLinkNode);
		ptask->pevent = NULL;
		ptask->eventmsg = msg;
		ptask->unEventResult = result;
		ptask->tTaskState = ptask->tTaskState & (~TASK_WAITEVENT); 

		if(ptask->nDelay != 0)
		{
			OS_TASK_DelayWakeup(ptask);
		}

		OS_TASK_TaskRdy(ptask);
	}

	OS_TASK_ExitCritical(status);

	return unCount;
}

/**
 * @brief 获取事件控制块数量
 * @param[in] pevent 事件控制块数据指针
 * @note None
 * @retval 返回清空时还剩下多少等待控制块数量
 */
uint32_t OS_EVENT_GetEventCount(p_event_block_t pevent)
{
	unsigned int status;
	unsigned int unCount;

	status = OS_TASK_EnterCritical();

	unCount = OS_COM_GetNodeCount(&pevent->tEventList);
	
	OS_TASK_ExitCritical(status);

	return unCount;
}
