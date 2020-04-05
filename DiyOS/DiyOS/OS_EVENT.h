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
#include "OS_TASK.h"

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

/**
 * @brief �¼����ƿ�ȴ�
 * @param[in] pevent �¼����б�ptask ��������msg��Ϣ��state ״̬��timeout��ʱ
 * @note 
 * @retval ���ش�����
 */
int OS_EVENT_Wait(p_event_block_t pevent,p_tTask ptask,void *msg, unsigned int state,unsigned int timeout);

/**
 * @brief �¼��黽��
 * @param[in] pevent �¼����б�msg�¼�����Ϣ��result�¼����ƿ�ȴ����
 * @note 
 * @retval ���ش�����
 */
int OS_EVENT_Wake(p_event_block_t pevent,void *msg,unsigned int result);

/**
 * @brief �¼����ƿ��Ƴ�
 * @param[in] ptask ��������msg�¼������ͣ�result�¼���Ϣ
 * @note 
 * @retval ���ش�����
 */
int OS_EVENT_Delete(p_tTask ptask,void *msg,unsigned int result);

/**
 * @brief ��������¼����ƿ�
 * @param[in] pevent �¼����ƿ�����ָ�룬msg�¼������ͣ�result�¼���Ϣ
 * @note None
 * @retval �������ʱ��ʣ�¶��ٵȴ����ƿ�����
 */
uint32_t OS_EVENT_ClearAll(p_event_block_t pevent,void *msg,unsigned int result);

/**
 * @brief ��ȡ�¼����ƿ�����
 * @param[in] pevent �¼����ƿ�����ָ��
 * @note None
 * @retval �������ʱ��ʣ�¶��ٵȴ����ƿ�����
 */
uint32_t OS_EVENT_GetEventCount(p_event_block_t pevent);


#endif
