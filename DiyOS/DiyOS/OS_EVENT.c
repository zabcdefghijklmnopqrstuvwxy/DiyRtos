/**
********************************************************************************
* @file    OS_EVENT.c
* @author  Sai
* @version 01V01
* @date    2020-03-28
* @brief   OS_EVENT.c
* @note    �¼������Դ����
* @attention
********************************************************************************
*/

#include "OS_EVENT.h"

/**
 * @brief �¼����ʼ��
 * @param[in] pevent �¼����б�type�¼�������
 * @note 
 * @retval ���ش�����
 */
int OS_EVENT_Init(p_event_block_t pevent,event_type_t type)
{
	pevent->tEventType = type;
	OS_COM_InitList(&pevent->tEventList);

	return 0;
}

/**
 * @brief �¼����ƿ�ȴ�
 * @param[in] pevent �¼����б�ptask ��������msg��Ϣ��state ״̬��timeout��ʱ
 * @note 
 * @retval ���ش�����
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
 * @brief �¼��黽��
 * @param[in] pevent �¼����б�msg�¼�����Ϣ��result�¼����ƿ�ȴ����
 * @note 
 * @retval ���ش�����
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
 * @brief �¼����ƿ��Ƴ�
 * @param[in] ptask ��������msg�¼������ͣ�result�¼���Ϣ
 * @note 
 * @retval ���ش�����
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
 * @brief ��������¼����ƿ�
 * @param[in] pevent �¼����ƿ�����ָ�룬msg�¼������ͣ�result�¼���Ϣ
 * @note None
 * @retval �������ʱ��ʣ�¶��ٵȴ����ƿ�����
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
 * @brief ��ȡ�¼����ƿ�����
 * @param[in] pevent �¼����ƿ�����ָ��
 * @note None
 * @retval �������ʱ��ʣ�¶��ٵȴ����ƿ�����
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
