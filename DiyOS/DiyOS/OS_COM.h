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

/**
*@brief λͼ���ݽṹ
*/
typedef struct _BITMAP{
		unsigned int unBitmap;    /**< λͼ���ݱ� */    
}tBitmap_t,*p_tBitmap_t;

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


#endif