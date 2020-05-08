/**
********************************************************************************
* @file    OS_MUTEX.c
* @author  Sai
* @version 01V01
* @date    2020-05-07
* @brief   OS_MUTEX.c
* @note    互斥锁源文件，该文件主要解决信号量解决不了的三大问题
* 1、信号量不支持嵌套调用
* 2、信号量不支持所有者，任意任务都能发送Notify
* 3、优先级反转，低优先级任务占用资源，高优先级等待资源，中优先级占用CPU运行
* @attention 
********************************************************************************
*/
#include "OS_MUTEX.h"

/**
 * @brief 互斥锁初始化
 * @param pmutex 信号量指针
 * @note 初始化事件控制块，将任务初始化为0，优先级清零
 * @retval 无
 */
void OS_MUTEX_Init(p_mutex_t pmutex)
{
	OS_EVENT_Init(&pmutex->tEvent, EVENT_SEM);
	pmutex->unLockCnt = 0;
	pmutex->ptask = NULL;
	pmutex->unOriPri = 0;
}

/**
 * @brief 互斥锁上锁
 * @param pmutex 互斥锁指针，unTimeOut 超时时间
* @note 互斥锁上锁，资源已被锁定则加入到等待链表中
 * @retval 无
 */
int OS_MUTEX_Lock(p_mutex_t pmutex,unsigned int unTimeOut)
{
	unsigned int unStatus;
	unStatus = OS_TASK_EnterCritical();
	
	if(pmutex->unLockCnt <= 0)
	{
		pmutex->unLockCnt++;
		pmutex->ptask = currentTask;
		pmutex->unOriPri = currentTask->unPri;
		OS_TASK_ExitCritical(unStatus);
		return ERR_OK;	
	}
	else
	{
		if(currentTask->unPri < pmutex->unOriPri)
		{
			p_tTask powner = pmutex->ptask;
			if(powner->tTaskState == TASK_READYSTATUS)	
			{
				OS_TASK_SuspendTask(powner);
				powner->unPri = currentTask->unPri;
				OS_TASK_WakeUpTask(powner);				
			}
			else
			{
				powner->unPri = currentTask->unPri;
			}			
		}
		OS_EVENT_Wait(&pmutex->tEvent, currentTask, NULL, EVENT_MUTEX, unTimeOut);
		OS_TASK_ExitCritical(unStatus);
		OS_TASK_Sched();
		return currentTask->unEventResult;
	}
}

/**
 * @brief 互斥锁上锁
 * @param pmutex 互斥锁指针
 * @note 互斥锁上锁，试着上锁，如果不成功则直接返回
 * @retval 无
 */
int OS_MUTEX_TryLock(p_mutex_t pmutex)
{
	unsigned int unStatus;
	unStatus = OS_TASK_EnterCritical();
	
	if(pmutex->unLockCnt <= 0)
	{
		pmutex->unLockCnt++;
		pmutex->ptask = currentTask;
		pmutex->unOriPri = currentTask->unPri;
		OS_TASK_ExitCritical(unStatus);
		return ERR_OK;
	}
	else
	{
		if(pmutex->ptask == currentTask)
		{
			pmutex->unLockCnt++;
			OS_TASK_ExitCritical(unStatus);
			return ERR_OK;
		}
		
		OS_TASK_ExitCritical(unStatus);
		return ERR_RESOURCE_UNAVLIABLE;
	}	
}

/**
 * @brief 互斥锁解锁
 * @param pmutex 互斥锁指针
 * @note 互斥锁释放处理
 * @retval 无
 */
int OS_MUTEX_UnLock(p_mutex_t pmutex)
{
	unsigned int unStatus;
	unStatus = OS_TASK_EnterCritical();
	
	if(pmutex->unLockCnt <= 0)
	{
		OS_TASK_ExitCritical(unStatus);
		return ERR_OK;
	}
	
	if(pmutex->ptask != currentTask)
	{
		OS_TASK_ExitCritical(unStatus);
		return ERR_OWNER;
	}
	
	
	if(--pmutex->unLockCnt > 0)
	{
		OS_TASK_ExitCritical(unStatus);
		return ERR_OK;
	}
	
	if(pmutex->unLockCnt == 0)
	{
		if(pmutex->ptask->unPri != pmutex->unOriPri)
		{
			OS_TASK_SuspendTask(pmutex->ptask);
			pmutex->ptask->unPri = currentTask->unPri;
			OS_TASK_WakeUpTask(pmutex->ptask);					
		}
		else
		{
			currentTask->unPri = pmutex->unOriPri;
		}
		
		
		if(OS_EVENT_GetEventCount(&pmutex->tEvent) > 0)
		{
			p_tTask ptask = OS_EVENT_Wake(&pmutex->tEvent, NULL, ERR_OK);
			pmutex->ptask = ptask;
			pmutex->unOriPri = ptask->unPri;
			pmutex->unLockCnt--;
			OS_TASK_ExitCritical(unStatus);

			if(ptask->unPri < currentTask->unPri)
			{
				OS_TASK_Sched();
			}
		}
	}
	
	OS_TASK_ExitCritical(unStatus);
	
	return ERR_OK;
}

/**
 * @brief 互斥锁销毁
 * @param pmutex 互斥锁指针
 * @note 互斥锁销毁处理
 * @retval 无
 */
int OS_MUTEX_Destroy(p_mutex_t pmutex)
{
	unsigned int unStatus;
	unsigned int unCnt = 0;
	unStatus = OS_TASK_EnterCritical();
	
	if(pmutex->unLockCnt > 0)
	{
		if(pmutex->unOriPri != pmutex->ptask->unPri)
		{
			if(pmutex->ptask->tTaskState == TASK_READYSTATUS)
			{
				OS_TASK_SuspendTask(pmutex->ptask);
				pmutex->ptask->unPri = currentTask->unPri;
				OS_TASK_WakeUpTask(pmutex->ptask);									
			}
			else
			{
				currentTask->unPri = pmutex->unOriPri;
			}		
		}
		
		unCnt = OS_EVENT_ClearAll(&pmutex->tEvent, NULL, ERR_OK);
		
		if(unCnt > 0)
		{
			OS_TASK_ExitCritical(unStatus);
			OS_TASK_Sched();
		}
	}
	
	OS_TASK_ExitCritical(unStatus);
	return ERR_OK;
}


/**
 * @brief 互斥锁信息查询
 * @param pmutex 互斥锁指针
 * @note 互斥锁信息获取
 * @retval 无
 */
void OS_MUTEX_GetInfo(p_mutex_t pmutex,p_mutex_info_t pinfo)
{
	unsigned int unStatus;
	unStatus = OS_TASK_EnterCritical();
	
	pinfo->poritask = pmutex->ptask;
	pinfo->unInheritPri = pmutex->ptask ? pmutex->ptask->unPri : NULL;
	pinfo->unLockCnt = pmutex->unLockCnt;
	pinfo->unOriPri = pmutex->unOriPri;
	
	OS_TASK_ExitCritical(unStatus);
}
