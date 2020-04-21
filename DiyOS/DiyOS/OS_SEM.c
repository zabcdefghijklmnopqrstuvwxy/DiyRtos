/**
********************************************************************************
* @file    OS_SEM.c
* @author  Sai
* @version 01V01
* @date    2020-04-21
* @brief   OS_SEM.c
* @note    �ź���Դ����
* @attention
********************************************************************************
*/

#include "OS_SEM.h"

/**
 * @brief �ź�����ʼ��
 * @param psem �ź���ָ�룬unStartCnt ��ʼ����ֵ��unMaxCnt ������ֵ
 * @note 
 * @retval ��
 */
void OS_SEM_Init(p_sem_t psem, unsigned int unStartCnt, unsigned int unMaxCnt)
{
		OS_EVENT_Init(&psem->tEvent, EVENT_SEM);
	
	  psem->unMaxCount = unMaxCnt;
	
	  if(0 == unMaxCnt)
		{
				psem->unCount = unStartCnt;
		}
		else
		{
			psem->unCount = unStartCnt > psem->unMaxCount ? psem->unMaxCount : unStartCnt;
		}
}