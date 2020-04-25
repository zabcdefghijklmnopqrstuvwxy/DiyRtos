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

#include "os_type.h"
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
*@brief �ź�����Ϣ�ṹ
*/
typedef struct _SEM_INFO
{
		unsigned int unCount;            /**< �ź�������ֵ */
	  unsigned int unMaxCount;			   /**< �ź���������ֵ */
	  unsigned int unTaskCount;				 /**< �ź����������� */
}sem_info_t,*p_sem_info_t;

/**
 * @brief �ź�����ʼ��
 * @param psem �ź���ָ�룬unStartCnt ��ʼ����ֵ��unMaxCnt ������ֵ
 * @note 
 * @retval ��
 */
void OS_SEM_Init(p_sem_t psem, unsigned int unStartCnt, unsigned int unMaxCnt);

/**
 * @brief �ź����ȴ�����
 * @param psem �ź���ָ�� unWaitTicks �ȴ�Tickʱ��
 * @note ����鵽�ź�������ֵ������ʱ��ʾ���Զ��ٽ�����Դ���в����������ź�������ֵ����0ʱ
 * ����ǰ�������ȴ�������ֱ���ź�������ֵ����0Ϊֹ
 * @retval ��
 */
int OS_SEM_Wait(p_sem_t psem, unsigned int unWaitTick);

/**
 * @brief �ź�������ȴ�����
 * @param psem �ź���ָ��
 * @note ����鵽�ź�������ֵ������ʱ��ʾ���Զ��ٽ�����Դ���в����������ź�������ֵ����0ʱ
 * ֱ�ӷ���
 * @retval ��
 */
int OS_SEM_NoWait(p_sem_t psem);

/**
 * @brief �ź���֪ͨ����
 * @param psem �ź���ָ��
 * @note ���ź�������ֵ���Ӵ���
 * @retval ��
 */
void OS_SEM_Notify(p_sem_t psem);

/**
 * @brief �ź������ٲ���
 * @param psem �ź���ָ��
 * @note ���ź����Ķ���ȫ��ɾ���������ź����ļ���ֵ����
 * @retval �����ź�������ʱ��ʣ�����ź�����Ӧ������
 */
unsigned int OS_SEM_Destroy(p_sem_t psem);

/**
 * @brief �ź���״̬��Ϣ��ȡ
 * @param psem �ź���ָ�룬pinfo �ź���״̬��Ϣ
 * @note ���ź��������Ϣ���п�������
 * @retval ��
 */
void OS_SEM_GetInfo(p_sem_t psem, p_sem_info_t pinfo);

#endif