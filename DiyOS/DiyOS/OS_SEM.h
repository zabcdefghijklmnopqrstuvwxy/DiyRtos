/**
********************************************************************************
* @file    OS_SEM.h
* @author  Sai
* @version 01V01
* @date    2020-04-21
* @brief   OS_SEM.h
* @note    �ź���ͷ�ļ�
* @attention
********************************************************************************
*/
#ifndef		_OS_SEM_H_
#define		_OS_SEM_H_

#include "os_event.h"

/**
*@brief �ź������ݽṹ
*/
typedef struct _SEM_T
{
		event_block_t tEvent;				/**< �¼����ƿ�  */
		unsigned int unCount;			  /**< �ź�������  */
		unsigned int unMaxCount;    /**< �źż������ֵ  */
}sem_t,*p_sem_t;


/**
 * @brief �ź�����ʼ��
 * @param psem �ź���ָ�룬unStartCnt ��ʼ����ֵ��unMaxCnt ������ֵ
 * @note 
 * @retval ��
 */
void OS_SEM_Init(p_sem_t psem, unsigned int unStartCnt, unsigned int unMaxCnt);


#endif