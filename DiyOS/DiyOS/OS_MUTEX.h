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
*@brief ��������Ϣ
*/
typedef struct _MUTEX_INFO_T
{
	unsigned int unLockCnt;				/**< �������� */
	p_tTask poritask;					/**< ����ṹ */
	unsigned int unOriPri;				/**< ԭʼ���������ȼ� */
	unsigned int unInheritPri;			/**< �̳е����ȼ� */
}mutex_info_t,*p_mutex_info_t;


/**
 * @brief ��������ʼ��
 * @param pmutex �ź���ָ��
 * @note ��ʼ���¼����ƿ飬�������ʼ��Ϊ0�����ȼ�����
 * @retval ��
 */
void OS_MUTEX_Init(p_mutex_t pmutex);

/**
 * @brief ����������
 * @param pmutex ������ָ�룬unTimeOut ��ʱʱ��
* @note ��������������Դ�ѱ���������뵽�ȴ�������
 * @retval ��
 */
int OS_MUTEX_Lock(p_mutex_t pmutex,unsigned int unTimeOut);

/**
 * @brief ����������
 * @param pmutex ������ָ��
 * @note ����������������������������ɹ���ֱ�ӷ���
 * @retval ��
 */
int OS_MUTEX_TryLock(p_mutex_t pmutex);

/**
 * @brief ����������
 * @param pmutex ������ָ��
 * @note �������ͷŴ���
 * @retval ��
 */
int OS_MUTEX_UnLock(p_mutex_t pmutex);




#endif
