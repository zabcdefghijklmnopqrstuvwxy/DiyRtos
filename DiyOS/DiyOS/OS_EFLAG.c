/**
********************************************************************************
* @file    OS_EFLAG.c
* @author  Sai
* @version 01V01
* @date    2020-05-03
* @brief   OS_EFLAG.c
* @note    事件标志源代码
* @attention
********************************************************************************
*/
#include "OS_EFLAG.h"
#include "OS_TYPE.h"

/**
* @brief 事件标志组初始化
* @param[in] pevent 事件标志信息指针
* @note 初始化事件标志控制块链表
* @retval 返回0表示初始化成功，返回非0表示失败
*/
int OS_EFLAG_Init(p_eflag_t pevent, unsigned int unFlag)
{
	OS_EVENT_Init(&pevent->tEvent,EVENT_FLAG);
	pevent->unFlag = unFlag;
	
	return ERR_OK;
}

/**
* @brief 事件标志检查
* @param[in] pevent 事件标志信息指针，unFlagType等待的事件类型, unFlag 事件标志位指针
* @note None
* @retval 返回事件标志信息
*/
int OS_EFLAG_CheckProcess(p_eflag_t pflag, unsigned int unFlagType, unsigned int *unFlag)
{
	unsigned int unIsSet = unFlagType & FLAGGROUP_SET;
	unsigned int unIsAll = unFlagType & FLAGGROUP_ALL;
	unsigned int unSrcFlag = *unFlag;
	unsigned int unIsConsume = unFlagType & FLAGGROUP_CONSUME;
	
	unsigned int unCalcFlag = unIsSet ? (pflag->unFlag & unSrcFlag) : (~pflag->unFlag & unSrcFlag);
	
	if(((unIsAll) && (unCalcFlag == unSrcFlag)) || ((unIsAll == 0) && unCalcFlag))
	{
		if(unIsConsume)
		{
			if(unIsSet)
			{
				pflag->unFlag &= ~unSrcFlag;	
			}	
			else
			{
				pflag->unFlag |= unSrcFlag;	
			}	
		}
		
		*unFlag = unCalcFlag;
		return ERR_OK;
	}
	
	*unFlag = unCalcFlag;
	return ERR_RESOURCE_UNAVLIABLE;
}

/**
* @brief 事件标志等待
* @param[in] pevent 事件标志信息指针，unWaitType等待的事件类型, unRequestFlag 待等待的结果
* @note 查询事件标志位是否满足，如果满足则返回，否则加入等待事件标志队列中
* @retval 
*/
int OS_EFLAG_Wait(p_eflag_t pevent, unsigned int unWaitType, unsigned int unRequestFlag, unsigned int *unResultFlag, unsigned int unTimeout)
{
	int nResult = 0;
	unsigned int unStatus;
	unsigned int unFlag = unRequestFlag;
	int nRet = 0;
	unStatus = OS_TASK_EnterCritical();	
	
	nRet = OS_EFLAG_CheckProcess(pevent, unWaitType, &unFlag);
	if(nRet != ERR_OK)
	{
		currentTask->unWaitFlagType = unWaitType;
		currentTask->unEventFlag = unRequestFlag;
		OS_EVENT_Wait(&pevent->tEvent, currentTask, NULL, EVENT_FLAG, unTimeout);
		OS_TASK_ExitCritical(unStatus);
		OS_TASK_Sched();
		
		*unResultFlag = currentTask->unEventFlag;
		nResult = currentTask->unEventResult;
	}	
	else
	{
		*unResultFlag = unFlag;
		OS_TASK_ExitCritical(unStatus);
	}
		
	return nResult;
}

/**
* @brief 事件标志组不等待处理
* @param[in] pevent 事件标志信息指针，unSetMode设置模式，unFlag 设置的标志
* @note 
* @retval 
*/
int OS_EFLAG_NoWait(p_eflag_t pevent, unsigned int unWaitType, unsigned int unRequestFlag, unsigned int *unResultFlag)
{
	unsigned int unStatus;
	unsigned int unFlag = unRequestFlag;
	
	unStatus = OS_TASK_EnterCritical();	
	
	OS_EFLAG_CheckProcess(pevent, unWaitType, &unFlag);
	OS_TASK_ExitCritical(unStatus);

	*unResultFlag = unFlag;
	return ERR_OK;
}

/**
* @brief 事件标志组通知
* @param[in] pevent 事件标志信息指针，unSetMode设置模式，unFlag 设置的标志
* @note 遍历事件标志组等待链表，将符合事件标志条件的任务唤醒
* @retval 
*/
int OS_EFLAG_Notify(p_eflag_t pevent, unsigned int unSetMode, unsigned int unFlag)
{
	unsigned int unStatus;
	p_nodelist_t plist = NULL;
	p_node_t pnode = NULL;
	p_node_t pnextnode = NULL; 
	p_tTask ptask = NULL;
	unStatus = OS_TASK_EnterCritical();
	
	if(unSetMode)
	{
		pevent->unFlag |= unFlag;	
	}	
	else
	{
		pevent->unFlag &= ~unFlag;	
	}	
	
	plist = &pevent->tEvent.tEventList;
	
	for(pnode = plist->head.next; pnode != &pevent->tEvent.tEventList.head; pnode = pnextnode)
	{
		unsigned int unFlag;
		unsigned int unEventType;
		int nRet = 0;
		
		ptask = NODEPARENT(pnode, tTask, tLinkNode);
		unFlag = ptask->unEventFlag;
		unEventType = ptask->unWaitFlagType;
		
		nRet = OS_EFLAG_CheckProcess(pevent, unEventType, &unFlag);
		
		if(nRet == ERR_OK)
		{
			ptask->unEventFlag = unFlag;
			OS_EVENT_Wake(&pevent->tEvent, NULL, ptask->unEventResult);
		}
	}
	
	OS_TASK_ExitCritical(unStatus);
	OS_TASK_Sched();
	
	return ERR_OK;
}

/**
* @brief 事件标志组销毁处理
* @param[in] pevent 事件标志信息指针
* @note 将事件标志组队列销毁处理
* @retval 返回销毁时等待任务的个数数量
*/
unsigned int OS_EFLAG_Destroy(p_eflag_t pevent)
{
	unsigned int unStatus;
	unsigned int unCnt = 0;
	unStatus = OS_TASK_EnterCritical();
	
	unCnt = OS_EVENT_ClearAll(&pevent->tEvent, NULL, ERR_OK);
	
	OS_TASK_ExitCritical(unStatus);

	return unCnt;
}

/**
* @brief 事件标志组销毁处理
* @param[in] pevent 事件标志信息指针，pinfo 事件标志组信息
* @note 将事件标志组队列销毁处理
* @retval 
*/
void OS_EFLAG_GetInfo(p_eflag_t pevent, p_eflag_info_t pinfo)
{
	unsigned int unStatus;
	unStatus = OS_TASK_EnterCritical();
	
	pinfo->unFlag = pevent->unFlag;
	pinfo->unWaitTaskCnt = OS_EVENT_GetEventCount(&pevent->tEvent);

	OS_TASK_ExitCritical(unStatus);
}
