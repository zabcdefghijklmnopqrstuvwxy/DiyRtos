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

/**
 * @brief 信号量等待操作
 * @param psem 信号量指针 unWaitTicks 等待Tick时间
 * @note 当检查到信号量计数值大于零时表示可以对临界区资源进行操作处理，当信号量计数值等于0时
 * 将当前任务放入等待队列中直至信号量计数值大于0为止
 * @retval 无
 */
int OS_SEM_Wait(p_sem_t psem, unsigned int unWaitTick)
{
	unsigned int unStatus;
	unStatus = OS_TASK_EnterCritical();

	if(psem->unCount > 0)
	{
		  psem->unCount--;
		  OS_TASK_ExitCritical(unStatus);
			return ERR_OK;
	}

	OS_EVENT_Wait(&psem->tEvent,currentTask,NULL,EVENT_SEM,unWaitTick);
	OS_TASK_ExitCritical(unStatus);
	OS_TASK_Sched();

	return currentTask->unEventResult;
}

/**
 * @brief 信号量无需等待操作
 * @param psem 信号量指针
 * @note 当检查到信号量计数值大于零时表示可以对临界区资源进行操作处理，当信号量计数值等于0时
 * 直接返回
 * @retval 无
 */
int OS_SEM_NoWait(p_sem_t psem)
{
	unsigned int unStatus;
	unStatus = OS_TASK_EnterCritical();

	if(psem->unCount > 0)
	{
		psem->unCount--;
		OS_TASK_ExitCritical(unStatus);

		return ERR_OK;
	}

	OS_TASK_ExitCritical(unStatus);
	return ERR_RESOURCE_UNAVLIABLE;
}

/**
 * @brief 信号量通知操作
 * @param psem 信号量指针
 * @note 将信号量计数值增加处理
 * @retval 无
 */
void OS_SEM_Notify(p_sem_t psem)
{
	unsigned int unStatus;
	unStatus = OS_TASK_EnterCritical();
	p_tTask ptask = NULL;

	if(OS_EVENT_GetEventCount(&psem->tEvent))
	{
		ptask = OS_EVENT_Wake(&psem->tEvent,NULL,ERR_OK);

		if(ptask->unPri < currentTask->unPri)
		{
			OS_TASK_Sched();
		}
	}

	if(!psem->unMaxCount)      //unMaxCount计数为0表示信号量无限制
	{
		if(++psem->unCount > psem->unMaxCount)
		{
				psem->unCount = psem->unMaxCount;
		}
	}
	else
	{
		++psem->unCount;
	}

	OS_TASK_ExitCritical(unStatus);
}

/**
 * @brief 信号量销毁操作
 * @param psem 信号量指针
 * @note 将信号量的队列全部删除掉并将信号量的计数值清零
 * @retval 返回信号量销毁时还剩多少信号量对应的任务
 */
unsigned int OS_SEM_Destroy(p_sem_t psem)
{
	unsigned int unStatus;
	unsigned int unCnt = 0;
	unStatus = OS_TASK_EnterCritical();

	unCnt = OS_EVENT_ClearAll(&psem->tEvent, NULL, ERR_OK);
	OS_TASK_ExitCritical(unStatus);

	if(unCnt > 0)
	{
		OS_TASK_Sched();
	}

	psem->unCount = 0;

	return unCnt;
}

/**
 * @brief 信号量状态信息获取
 * @param psem 信号量指针，pinfo 信号量状态信息
 * @note 将信号量相关信息进行拷贝操作
 * @retval 无
 */
void OS_SEM_GetInfo(p_sem_t psem, p_sem_info_t pinfo)
{
	unsigned int unStatus;
	unStatus = OS_TASK_EnterCritical();

	pinfo->unCount = psem->unCount;
	pinfo->unMaxCount = psem->unMaxCount;
	pinfo->unTaskCount = OS_EVENT_GetEventCount(&psem->tEvent);

	OS_TASK_ExitCritical(unStatus);
}