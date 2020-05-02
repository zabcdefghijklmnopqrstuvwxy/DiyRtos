/**
********************************************************************************
* @file    OS_MAIL.h
* @author  Sai
* @version 01V01
* @date    2020-04-25
* @brief   OS_MAIL.h
* @note    ����ͷ�ļ�
* @attention
********************************************************************************
*/

#ifndef _OS_MAIL_H_
#define	_OS_MAIL_H_

#include "OS_EVENT.h"
#include "OS_TYPE.h"

#define			MSG_PRIORITY		1			/**< ��Ϣ���Ȳ��� */
#define			MSG_NORMAL			0			/**< ��Ϣ�������� */


/**
*@brief ����洢��Ϣ
*/
typedef struct _MAIL_MSG
{
	event_block_t tEvent;	      /**< �¼����ƿ���Ϣ */
	unsigned int unReadPos;		  /**< ���仺���ȡλ�� */
	unsigned int unWritePos;      /**< ���仺��д��λ�� */
	unsigned int unMsgCnt;        /**< ������Ϣ���� */
	unsigned int unMaxLen;	      /**< ���仺����󳤶� */  
	unsigned char **ucBuf;        /**< ���仺��ռ� */
}mail_msg_t, *p_mail_msg_t;

/**
 * @brief �����ʼ��
 * @param pmbox ������Ϣָ�룬pbuf ���仺��ָ�룬unMaxLen ���仺����󳤶�
 * @note 
 * @retval ��
 */
void OS_MAIL_Init(p_mail_msg_t pmbox, unsigned char **pbuf, unsigned int unMaxLen);

/**
* @brief �����ȡ��Ϣ
 * @param pmbox ������Ϣָ�룬pmsg ��ȡ�������Ϣָ��
 * @note 
 * @retval ��
 */
int OS_MAIL_Wait(p_mail_msg_t pmbox, void **pmsg, unsigned int unWaitTicks);

/**
* @brief �����ȡ��Ϣ�޵ȴ�����
 * @param pmbox ������Ϣָ�룬pmsg ��ȡ�������Ϣָ��
 * @note 
 * @retval ��
 */
int OS_MAIL_NoWait(p_mail_msg_t pmbox, void **pmsg);

/**
 * @brief ����д����Ϣ
 * @param pmbox ������Ϣָ�룬pmsg д���������Ϣָ��
 * @note 
 * @retval ��
 */
int OS_MAIL_Notify(p_mail_msg_t pmbox, void *pmsg,unsigned int unNotifyOption);


#endif
