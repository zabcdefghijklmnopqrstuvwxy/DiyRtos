/**
********************************************************************************
* @file    OS_EVENT.c
* @author  Sai
* @version 01V01
* @date    2020-03-28
* @brief   OS_EVENT.c
* @note    �¼������Դ����
* @attention
********************************************************************************
*/

#include "OS_EVENT.h"

/**
 * @brief �¼����ʼ��
 * @param[in] pevent �¼����б�type�¼�������
 * @note 
 * @retval ���ش�����
 */
int OS_EVENT_Init(p_event_block_t pevent,event_type_t type)
{
	  pevent->tEventType = type;
		OS_COM_InitList(&pevent->tEventList);
}