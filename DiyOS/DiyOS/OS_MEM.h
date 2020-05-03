/**
********************************************************************************
* @file    OS_MEM.h
* @author  Sai
* @version 01V01
* @date    2020-05-03
* @brief   OS_MEM.c
* @note    �ڴ����ͷ�ļ�
* @attention
********************************************************************************
*/
#ifndef _OS_MEM_H_
#define	_OS_MEM_H_

#include "OS_COM.h"

#define POOL_MAX 4

#define     COM_PIBLIST_MAX_NUM        16       /**< ��������е����Ա���󳤶� */


/**
* @brief �ڴ����Ϣ�ṹ
*/
typedef struct
{
    unsigned int unMemStart;         /**< ������ڴ���ʼ��ַ */
    unsigned int unBlockSize;        /**< �ڴ���С */
    unsigned int unBlockCount;       /**< �ڴ���ܸ��� */
    nodelist_t memlist;             /**< �ڴ������ */
}mem_block_t,*p_mem_block_t;

/**
* @brief �ڴ����Ϣ�ṹ
*/
typedef struct
{
    unsigned int unMemStart;         /**< ������ڴ���ʼ��ַ */
    unsigned int unBlockSize;        /**< �ڴ���С */
    unsigned int unBlockCount;       /**< �ڴ���ܸ��� */
    unsigned int unUsedCount;        /**< ��ʹ���ڴ����� */
}mem_block_state_t;


/**
* @brief �ڴ��ĳ�ʼ��
* @param[in] unMemStart������ڴ����ʼ��ַ
*            unBlockSize�ڴ���С
*            unBlockCount�ڴ�����
* @note ��ʼ���ڴ������
* @retval ����0��ʾ��ʼ���ɹ������ط�0��ʾʧ��
*/
int MEM_InitBlock(unsigned int unMemStart,unsigned int unBlockSize,unsigned int unBlockCount);

/**
* @brief �ڴ����
* @param[in] size�����ڴ��С
* @note 
* @retval ���ط���ĵ�ַ�����ΪNULL��ʾ����ʧ��
*/
void* MEM_Alloc(unsigned int size);

/**
* @brief �ڴ��ͷ�
* @param[in] pAddr�ͷŵĵ�ַ
* @note None
* @retval None
*/
void MEM_Free(void *pAddr);

/**
* @brief ��ȡ���ڴ��״̬
* @param[in] pMemState mem_block_state_t�ṹ������
* @note None
* @retval None
*/
void MEM_GetState(mem_block_state_t  pMemState[POOL_MAX]);

/**
* @brief ȥ��ʼ���ڴ��
* @param[in] unMemStart ��ȥ��ʼ�����ڴ����ʼ��ַ
* @note None
* @retval ����0��ʾ��ʼ���ɹ������ط�0��ʾʧ��
*/
int MEM_DeInitBlock(unsigned int unMemStart);

#endif