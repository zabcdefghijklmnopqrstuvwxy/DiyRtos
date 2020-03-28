/**
********************************************************************************
* @file    OS_EVENT.h
* @author  Sai
* @version 01V01
* @date    2020-03-28
* @brief   OS_EVENT.h
* @note    �¼������ͷ�ļ�
* @attention
********************************************************************************
*/

#ifndef		_OS_EVENT_H_
#define		_OS_EVENT_H_

#include "OS_COM.h"

/**
*@brief �¼����
*/
typedef enum _EVENT_RESULT{
		EVENT_OK
}event_result_t;

/**
*@brief �¼�������
*/
typedef enum _EVENT_TYPE{
		EVENT_UNKNOWN									/**< δ֪���� */
}event_type_t;

/**
*@brief �¼������ṹ
*/
typedef struct _EVENT_BLOCK{
		event_type_t tEventType;			/**< �¼������� */
		nodelist_t	tEventList;				/**< �¼��б� */
}event_block_t,*p_event_block_t;

/**
 * @brief �¼����ʼ��
 * @param[in] pevent �¼����б�type�¼�������
 * @note 
 * @retval ���ش�����
 */
int OS_EVENT_Init(p_event_block_t pevent,event_type_t type);

#endif