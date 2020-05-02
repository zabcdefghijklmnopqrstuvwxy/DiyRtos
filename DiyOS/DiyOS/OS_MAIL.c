/**
********************************************************************************
* @file    OS_MAIL.c
* @author  Sai
* @version 01V01
* @date    2020-04-25
* @brief   OS_MAIL.c
* @note    邮箱源代码
* @attention
********************************************************************************
*/

#include "OS_MAIL.h"

/**
 * @brief 邮箱初始化
 * @param pmbox 邮箱信息指针，pbuf 邮箱缓存指针，unMaxLen 邮箱缓存最大长度
 * @note 
 * @retval 无
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
* @brief 邮箱读取信息等待处理
 * @param pmbox 邮箱信息指针，pmsg 读取邮箱的信息指针
 * @note 
 * @retval 无
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
* @brief 邮箱读取信息无等待处理
 * @param pmbox 邮箱信息指针，pmsg 读取邮箱的信息指针
 * @note 
 * @retval 无
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
 * @brief 邮箱写入信息
 * @param pmbox 邮箱信息指针，pmsg 写入邮箱的信息指针，unNotifyOption 优先写入标志
 * @note 
 * @retval 无
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
			
		if(unNotifyOption == MSG_PRIORITY)   //优先插入处理
		{
			pmbox->ucBuf[pmbox->unReadPos] = pmsg;
			
			pmbox->unReadPos = pmbox->unReadPos == 0 ? pmbox->unReadPos - 1 : pmbox->unReadPos - 1;			
			pmbox->unMsgCnt++;
			OS_TASK_ExitCritical(status);
		}
		else       //非优先插入处理
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
 * @brief 邮箱缓存清空处理
 * @param pmbox 邮箱信息指针
 * @note 判断邮箱计数是否为空，如果为空则将读写位置与缓存数据清零处理
 * @retval 无
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
 * @brief 邮箱销毁
 * @param pmbox 邮箱信息指针
 * @note 将pmbox中的事件链表清空，当等待事件链表中有等待任务时则进行相应的切换
 * @retval 无
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
 * @brief 邮箱信息获取
 * @param pinfo 邮箱信息指针
 * @note 无
 * @retval 无
 */
int OS_MAIL_GetInfo(p_mail_msg_t pmbox, p_mail_info_t pinfo)
{
	unsigned int status;
	status = OS_TASK_EnterCritical();
	
	pinfo->unMailMsgCnt = pmbox->unMsgCnt;
	pinfo->unWaitTaskCnt = OS_EVENT_GetEventCount(&pmbox->tEvent);
		
	OS_TASK_ExitCritical(status);	
}

