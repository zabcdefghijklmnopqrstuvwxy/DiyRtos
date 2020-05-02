/**
********************************************************************************
* @file    OS_MAIL.h
* @author  Sai
* @version 01V01
* @date    2020-04-25
* @brief   OS_MAIL.h
* @note    邮箱头文件
* @attention
********************************************************************************
*/

#ifndef _OS_MAIL_H_
#define	_OS_MAIL_H_

#include "OS_EVENT.h"
#include "OS_TYPE.h"

#define			MSG_PRIORITY		1			/**< 消息优先插入 */
#define			MSG_NORMAL			0			/**< 消息正常插入 */


/**
*@brief 邮箱存储信息
*/
typedef struct _MAIL_MSG
{
	event_block_t tEvent;	      /**< 事件控制块信息 */
	unsigned int unReadPos;		  /**< 邮箱缓存读取位置 */
	unsigned int unWritePos;      /**< 邮箱缓存写入位置 */
	unsigned int unMsgCnt;        /**< 邮箱信息计数 */
	unsigned int unMaxLen;	      /**< 邮箱缓存最大长度 */  
	unsigned char **ucBuf;        /**< 邮箱缓存空间 */
}mail_msg_t, *p_mail_msg_t;

/**
 * @brief 邮箱初始化
 * @param pmbox 邮箱信息指针，pbuf 邮箱缓存指针，unMaxLen 邮箱缓存最大长度
 * @note 
 * @retval 无
 */
void OS_MAIL_Init(p_mail_msg_t pmbox, unsigned char **pbuf, unsigned int unMaxLen);

/**
* @brief 邮箱读取信息
 * @param pmbox 邮箱信息指针，pmsg 读取邮箱的信息指针
 * @note 
 * @retval 无
 */
int OS_MAIL_Wait(p_mail_msg_t pmbox, void **pmsg, unsigned int unWaitTicks);

/**
* @brief 邮箱读取信息无等待处理
 * @param pmbox 邮箱信息指针，pmsg 读取邮箱的信息指针
 * @note 
 * @retval 无
 */
int OS_MAIL_NoWait(p_mail_msg_t pmbox, void **pmsg);

/**
 * @brief 邮箱写入信息
 * @param pmbox 邮箱信息指针，pmsg 写入邮箱的信息指针
 * @note 
 * @retval 无
 */
int OS_MAIL_Notify(p_mail_msg_t pmbox, void *pmsg,unsigned int unNotifyOption);


#endif
