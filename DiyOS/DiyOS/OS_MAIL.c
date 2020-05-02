/**
********************************************************************************
* @file    OS_MAIL.c
* @author  Sai
* @version 01V01
* @date    2020-04-25
* @brief   OS_MAIL.c
* @note    ����Դ����
* @attention
********************************************************************************
*/

#include "OS_MAIL.h"

/**
 * @brief �����ʼ��
 * @param pmbox ������Ϣָ�룬pbuf ���仺��ָ�룬unMaxLen ���仺����󳤶�
 * @note 
 * @retval ��
 */
void OS_MAIL_Init(p_mail_msg_t pmbox, unsigned char **pbuf, unsigned int unMaxLen)
{
	OS_EVENT_Init(&pmbox->tEvent, EVENT_MAIL);
	pmbox->ucBuf = pbuf;
	pmbox->unMaxLen = unMaxLen;
	pmbox->unReadPos = 0;
	pmbox->unWritePos = 0;
}

/**
* @brief �����ȡ��Ϣ�ȴ�����
 * @param pmbox ������Ϣָ�룬pmsg ��ȡ�������Ϣָ��
 * @note 
 * @retval ��
 */
int OS_MAIL_Wait(p_mail_msg_t pmbox, void **pmsg, unsigned int unWaitTicks)
{
	unsigned int status;	
	status = OS_TASK_EnterCritical();

	if(pmbox->unMsgCnt > 0)
	{
		*pmsg = pmbox->ucBuf[pmbox->unReadPos];
		pmbox->unMsgCnt--;
		pmbox->unReadPos = (pmbox->unReadPos + 1)%pmbox->unMaxLen;
		OS_TASK_ExitCritical(status);		
				
		return ERR_OK;
	}
	else
	{
		OS_EVENT_Wait(&pmbox->tEvent,currentTask, NULL, EVENT_MAIL, unWaitTicks);
		OS_TASK_ExitCritical(status);		
		pmsg = currentTask->eventmsg;
		OS_TASK_Sched();
		
		return currentTask->tTaskState;
	}	
}

/**
* @brief �����ȡ��Ϣ�޵ȴ�����
 * @param pmbox ������Ϣָ�룬pmsg ��ȡ�������Ϣָ��
 * @note 
 * @retval ��
 */
int OS_MAIL_NoWait(p_mail_msg_t pmbox, void **pmsg)
{
	unsigned int status;	
	status = OS_TASK_EnterCritical();

	if(pmbox->unMsgCnt > 0)
	{
		*pmsg = pmbox->ucBuf[pmbox->unReadPos];
		pmbox->unMsgCnt--;
		pmbox->unReadPos = (pmbox->unReadPos + 1)%pmbox->unMaxLen;
		OS_TASK_ExitCritical(status);		
				
		return ERR_OK;
	}
	else
	{
		OS_TASK_ExitCritical(status);		
		return ERR_RESOURCE_UNAVLIABLE;
	}	
}

/**
 * @brief ����д����Ϣ
 * @param pmbox ������Ϣָ�룬pmsg д���������Ϣָ�룬unNotifyOption ����д���־
 * @note 
 * @retval ��
 */
int OS_MAIL_Notify(p_mail_msg_t pmbox, void *pmsg,unsigned int unNotifyOption)
{
	unsigned int status;	
	status = OS_TASK_EnterCritical();	
	
	if(OS_EVENT_GetEventCount(&pmbox->tEvent) > 0)
	{
		p_tTask ptask = OS_EVENT_Wake(&pmbox->tEvent, pmsg, ERR_OK);
		
		if(ptask->unPri < currentTask->unPri)
		{
			OS_TASK_ExitCritical(status);		
			OS_TASK_Sched();
		}
	}
	else
	{
		if(pmbox->unMsgCnt >= pmbox->unMsgCnt)
		{
			OS_TASK_ExitCritical(status);
			return 	ERR_RESOURCE_OVER;		
		}
			
		if(unNotifyOption == MSG_PRIORITY)   //���Ȳ��봦��
		{
			pmbox->ucBuf[pmbox->unReadPos] = pmsg;
			
			pmbox->unReadPos = pmbox->unReadPos == 0 ? pmbox->unReadPos - 1 : pmbox->unReadPos - 1;			
			pmbox->unMsgCnt++;
			OS_TASK_ExitCritical(status);
		}
		else       //�����Ȳ��봦��
		{
			pmbox->ucBuf[pmbox->unWritePos] = pmsg;
			pmbox->unWritePos = (pmbox->unWritePos + 1)%pmbox->unMaxLen;
			
			pmbox->unMsgCnt = pmbox->unMsgCnt + 1 > pmbox->unMaxLen ? pmbox->unMaxLen : pmbox->unMsgCnt + 1;
			pmbox->unMsgCnt++;
			OS_TASK_ExitCritical(status);
		}
		
		return ERR_OK;
	}
		
	OS_TASK_ExitCritical(status);	
}

/**
 * @brief ���仺����մ���
 * @param pmbox ������Ϣָ��
 * @note �ж���������Ƿ�Ϊ�գ����Ϊ���򽫶�дλ���뻺���������㴦��
 * @retval ��
 */
int OS_MAIL_Flush(p_mail_msg_t pmbox)
{
	unsigned int status;	
	status = OS_TASK_EnterCritical();	
	
	if(pmbox->unMsgCnt == 0)
	{
		pmbox->unReadPos = 0;
		pmbox->unWritePos = 0;
	}
	
	OS_TASK_ExitCritical(status);	
	
	return ERR_OK;
}

/**
 * @brief ��������
 * @param pmbox ������Ϣָ��
 * @note ��pmbox�е��¼�������գ����ȴ��¼��������еȴ�����ʱ�������Ӧ���л�
 * @retval ��
 */
int OS_MAIL_Destory(p_mail_msg_t pmbox)
{
	unsigned int status;
	unsigned int unCnt = 0;
	status = OS_TASK_EnterCritical();
	
	unCnt = OS_EVENT_ClearAll(&pmbox->tEvent, NULL, ERR_OK);
	
	OS_TASK_ExitCritical(status);	
	
	if(unCnt > 0)
	{
		OS_TASK_Sched();
	}
	
	return ERR_OK;
}

/**
 * @brief ������Ϣ��ȡ
 * @param pinfo ������Ϣָ��
 * @note ��
 * @retval ��
 */
int OS_MAIL_GetInfo(p_mail_msg_t pmbox, p_mail_info_t pinfo)
{
	unsigned int status;
	status = OS_TASK_EnterCritical();
	
	pinfo->unMailMsgCnt = pmbox->unMsgCnt;
	pinfo->unWaitTaskCnt = OS_EVENT_GetEventCount(&pmbox->tEvent);
		
	OS_TASK_ExitCritical(status);	
}

