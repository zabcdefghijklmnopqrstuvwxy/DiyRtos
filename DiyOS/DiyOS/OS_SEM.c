/**
********************************************************************************
* @file    OS_SEM.c
* @author  Sai
* @version 01V01
* @date    2020-04-21
* @brief   OS_SEM.c
* @note    �ź���Դ����
* @attention
********************************************************************************
*/

#include "OS_SEM.h"

/**
 * @brief �ź�����ʼ��
 * @param psem �ź���ָ�룬unStartCnt ��ʼ����ֵ��unMaxCnt ������ֵ
 * @note 
 * @retval ��
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
 * @brief �ź����ȴ�����
 * @param psem �ź���ָ�� unWaitTicks �ȴ�Tickʱ��
 * @note ����鵽�ź�������ֵ������ʱ��ʾ���Զ��ٽ�����Դ���в����������ź�������ֵ����0ʱ
 * ����ǰ�������ȴ�������ֱ���ź�������ֵ����0Ϊֹ
 * @retval ��
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
 * @brief �ź�������ȴ�����
 * @param psem �ź���ָ��
 * @note ����鵽�ź�������ֵ������ʱ��ʾ���Զ��ٽ�����Դ���в����������ź�������ֵ����0ʱ
 * ֱ�ӷ���
 * @retval ��
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
 * @brief �ź���֪ͨ����
 * @param psem �ź���ָ��
 * @note ���ź�������ֵ���Ӵ���
 * @retval ��
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
		
		if(!psem->unMaxCount)      //unMaxCount����Ϊ0��ʾ�ź���������
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