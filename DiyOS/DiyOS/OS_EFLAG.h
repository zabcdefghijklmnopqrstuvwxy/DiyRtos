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


#endif
