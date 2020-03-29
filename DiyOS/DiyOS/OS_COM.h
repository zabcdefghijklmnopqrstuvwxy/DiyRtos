/**
********************************************************************************
* @file    OS_COM.h
* @author  Sai
* @version 01V01
* @date    2020-03-08
* @brief   ϵͳ��������ͷ�ļ�
* @note   
* @attention
********************************************************************************
*/
#ifndef _OS_COM_H_
#define _OS_COM_H_


#ifdef USE_STM32F1
#include "stm32f1xx_hal.h"
#endif

#ifdef USE_STM32F4
#include "stm32f4xx_hal.h"
#endif

/*< �������ݽڵ���׵�ַ */
#define  NODEPARENT(node,parent,name)  (parent*)((unsigned int)node - (unsigned int)&((parent*)0)->name)

/*< ��ȡ��һ���ڵ� */
#define	 GET_FIRST_NODE(head) 					(head.next)

/**
*@brief λͼ���ݽṹ
*/
typedef struct _BITMAP{
		unsigned int unBitmap;    /**< λͼ���ݱ� */    
}tBitmap_t,*p_tBitmap_t;

/**
*@brief ˫������ڵ�
*/
typedef struct _NODE{
		struct _NODE *pre;     /**< ǰ���ڵ� */
		struct _NODE *next;    /**< �����ڵ� */
}node_t,*p_node_t;

/**
*@brief ˫������
*/
typedef struct _NODELIST{
		node_t head;      					/**< ����ͷ */         
	  unsigned int unNodeCnt;     /**< ������� */
}nodelist_t,*p_nodelist_t;


/**
 * @brief λͼ��ʼ��
 * @param[in] pbitmap λͼ����ָ��
 * @note ��λͼ��������
 * @retval ��
 */
void OS_COM_InitBitmap(p_tBitmap_t pbitmap);

/**
 * @brief ����λͼ
 * @param[in] pbitmap λͼ����ָ�� pos ���õ�λͼλ��
 * @note ��pos��λͼ���л�����
 * @retval ��
 */
void OS_COM_SetBitmap(p_tBitmap_t pbitmap,unsigned int pos);

/**
 * @brief ���λͼ
 * @param[in] pbitmap λͼ����ָ�� pos �������λͼλ��
 * @note ��
 * @retval ��
 */
void OS_COM_ClrBitmap(p_tBitmap_t pbitmap,unsigned int pos);

/**
 * @brief ��ȡ��һ��λ��
 * @param[in] pbitmap λͼ����ָ��
 * @note ��λͼ���ұ��в��ҵ�һ����һ��λ�ã�Ĭ��0Ϊ���31Ϊ���
 * @retval ��
 */
unsigned int OS_COM_GetFirstBit(p_tBitmap_t pbitmap);

/**
 * @brief ˫������ڵ��ʼ��
 * @param[in] phead ͷ�ڵ�
 * @note ��˫������������㣬ǰ���ڵ�ͺ����ڵ㶼ָ��ͷ�ڵ�
 * @retval ����0��ʾ��ʼ���ɹ������ط�0��ʾ��ʼ��ʧ��
 */
int OS_COM_InitList(p_nodelist_t phead);

/**
 * @brief ˫������ڵ�����
 * @param[in] phead ͷ�ڵ㣬pnode ����ӵĽڵ�
 * @note β������ڵ�
 * @retval ����0��ʾ����ɹ������ط�0��ʾ����ʧ��
 */
int OS_COM_AddNode(p_nodelist_t phead,p_node_t pnode);

/**
 * @brief ˫������ڵ��ɾ��
 * @param[in] phead ͷ�ڵ㣬pnode ��ɾ���Ľڵ�
 * @note ��
 * @retval ����0��ʾɾ���ɹ������ط�0��ʾɾ��ʧ��
 */
int OS_COM_DelNode(p_nodelist_t phead,p_node_t pnode);

/**
 * @brief �Ƴ�˫������ڵ�ĵ�һ���ڵ�
 * @param[in] phead ͷ�ڵ�
 * @note ��
 * @retval ����ɾ����ĵ�һ���ڵ�
 */
p_node_t OS_COM_RemoveFirstNode(p_nodelist_t phead);

/**
 * @brief ��ȡ˫�������и���
 * @param[in] phead ����ָ��
 * @note ��
 * @retval ��������ڵ���� 
 */
unsigned int OS_COM_GetNodeCount(p_nodelist_t phead);

#endif
