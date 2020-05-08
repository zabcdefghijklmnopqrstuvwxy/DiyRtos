/**
********************************************************************************
* @file    OS_MUTEX.c
* @author  Sai
* @version 01V01
* @date    2020-05-07
* @brief   OS_MUTEX.c
* @note    ������Դ�ļ������ļ���Ҫ����ź���������˵���������
* 1���ź�����֧��Ƕ�׵���
* 2���ź�����֧�������ߣ����������ܷ���Notify
* 3�����ȼ���ת�������ȼ�����ռ����Դ�������ȼ��ȴ���Դ�������ȼ�ռ��CPU����
* @attention 
********************************************************************************
*/
#include "OS_MUTEX.h"

/**
 * @brief ��������ʼ��
 * @param pmutex �ź���ָ��
 * @note ��ʼ���¼����ƿ飬�������ʼ��Ϊ0�����ȼ�����
 * @retval ��
 */
void OS_MUTEX_Init(p_mutex_t pmutex)
{
	OS_EVENT_Init(&pmutex->tEvent, EVENT_SEM);
	pmutex->unLockCnt = 0;
	pmutex->ptask = NULL;
	pmutex->unOriPri = 0;
}

/**
 * @brief ����������
 * @param pmutex ������ָ�룬unTimeOut ��ʱʱ��
* @note ��������������Դ�ѱ���������뵽�ȴ�������
 * @retval ��
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
 * @brief ����������
 * @param pmutex ������ָ��
 * @note ����������������������������ɹ���ֱ�ӷ���
 * @retval ��
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
 * @brief ����������
 * @param pmutex ������ָ��
 * @note �������ͷŴ���
 * @retval ��
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
 * @brief ����������
 * @param pmutex ������ָ��
 * @note ���������ٴ���
 * @retval ��
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
 * @brief ��������Ϣ��ѯ
 * @param pmutex ������ָ��
 * @note ��������Ϣ��ȡ
 * @retval ��
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
