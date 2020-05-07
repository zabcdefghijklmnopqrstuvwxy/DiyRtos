/**
********************************************************************************
* @file    OS_EFLAG.h
* @author  Sai
* @version 01V01
* @date    2020-05-06
* @brief   OS_EFLAG.h
* @note    �¼���־��ͷ�ļ�
* @attention
********************************************************************************
*/

#ifndef _OS_EFLAG_H_
#define	_OS_EFLAG_H_

#include "OS_EVENT.h"


#define		FLAGGROUP_SET		(0x01)    /**< �¼���־��һ*/
#define		FLAGGROUP_CLR		(0x02)	  /**< �¼���־���� */
#define		FLAGGROUP_ALL		(0x04)	  /**< �¼���־����λ */
#define		FLAGGROUP_ANY		(0x08)    /**< �¼���־����λ */

#define		FLAGGROUP_SET_ALL	(FLAGGROUP_SET | FLAGGROUP_ALL)   /**< ����λ��һ */
#define		FLAGGROUP_SET_ANY	(FLAGGROUP_SET | FLAGGROUP_ANY)   /**< ����λ��һ */
#define		FLAGGROUP_CLR_ALL	(FLAGGROUP_CLR | FLAGGROUP_ALL)   /**< ����λ����*/
#define		FLAGGROUP_CLR_ANY	(FLAGGROUP_CLR | FLAGGROUP_ANY)   /**< ����λ���� */

#define		FLAGGROUP_CONSUME	(0x80)   /**< ���ı�־ */


/**
* @brief �¼���־��
*/
typedef struct _EFLAG_T
{
	event_block_t tEvent;		/**< �¼����ƿ� */
	unsigned int unFlag;		/**< �¼���־ */
}eflag_t,*p_eflag_t;

/**
* @brief �¼���־��ѯ��Ϣ
*/
typedef struct _EFLAG_INFO_T
{
	unsigned int unFlag;			/**< �¼���־��ı�־��Ϣ */
	unsigned int unWaitTaskCnt;		/**< �¼���־��ĵȴ��������� */
}eflag_info_t,*p_eflag_info_t;

/**
* @brief �¼���־���ʼ��
* @param[in] pevent �¼���־��Ϣָ��
* @note ��ʼ���¼���־���ƿ�����
* @retval ����0��ʾ��ʼ���ɹ������ط�0��ʾʧ��
*/
int OS_EFLAG_Init(p_eflag_t pevent, unsigned int unFlag);

/**
* @brief �¼���־�ȴ�
* @param[in] pevent �¼���־��Ϣָ�룬unWaitType�ȴ����¼�����, unRequestFlag ���ȴ��Ľ��
* @note ��ѯ�¼���־λ�Ƿ����㣬��������򷵻أ��������ȴ��¼���־������
* @retval 
*/
int OS_EFLAG_Wait(p_eflag_t pevent, unsigned int unWaitType, unsigned int unRequestFlag, unsigned int *unResultFlag, unsigned int unTimeout);

/**
* @brief �¼���־�鲻�ȴ�����
* @param[in] pevent �¼���־��Ϣָ�룬unSetMode����ģʽ��unFlag ���õı�־
* @note 
* @retval 
*/
int OS_EFLAG_NoWait(p_eflag_t pevent, unsigned int unWaitType, unsigned int unRequestFlag, unsigned int *unResultFlag);

/**
* @brief �¼���־��֪ͨ
* @param[in] pevent �¼���־��Ϣָ�룬unSetMode����ģʽ��unFlag ���õı�־
* @note �����¼���־��ȴ������������¼���־������������
* @retval 
*/
int OS_EFLAG_Notify(p_eflag_t pevent, unsigned int unSetMode, unsigned int unFlag);

/**
* @brief �¼���־�����ٴ���
* @param[in] pevent �¼���־��Ϣָ��
* @note ���¼���־��������ٴ���
* @retval ��������ʱ�ȴ�����ĸ�������
*/
unsigned int OS_EFLAG_Destroy(p_eflag_t pevent);

/**
* @brief �¼���־�����ٴ���
* @param[in] pevent �¼���־��Ϣָ�룬pinfo �¼���־����Ϣ
* @note ���¼���־��������ٴ���
* @retval 
*/
void OS_EFLAG_GetInfo(p_eflag_t pevent, p_eflag_info_t pinfo);

#endif
