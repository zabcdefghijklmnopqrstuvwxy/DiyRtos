/**
********************************************************************************
* @file    OS_MUTEX.h
* @author  Sai
* @version 01V01
* @date    2020-05-07
* @brief   OS_MUTEX.h
* @note    ������ͷ�ļ�
* @attention
********************************************************************************
*/

#ifndef _OS_MUTEX_H_
#define	_OS_MUTEX_H_

#include "os_type.h"
#include "os_event.h"

/**
*@brief ���������ݽṹ
*/
typedef struct _MUTEX_T
{
	event_block_t tEvent;				/**< �¼����ƿ�  */
	unsigned int unLockCnt;				/**< �������� */
	p_tTask ptask;						/**< ����ṹ */
	unsigned int unOriPri;				/**< ԭʼ���������ȼ� */
}mutex_t,*p_mutex_t;

/**
 * @brief ��������ʼ��
 * @param pmutex �ź���ָ��
 * @note ��ʼ���¼����ƿ飬�������ʼ��Ϊ0�����ȼ�����
 * @retval ��
 */
void OS_MUTEX_Init(p_mutex_t pmutex);










#endif
