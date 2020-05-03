/**
********************************************************************************
* @file    OS_MEM.h
* @author  Sai
* @version 01V01
* @date    2020-05-03
* @brief   OS_MEM.c
* @note    内存管理头文件
* @attention
********************************************************************************
*/
#ifndef _OS_MEM_H_
#define	_OS_MEM_H_

#include "OS_COM.h"

#define POOL_MAX 4

#define     COM_PIBLIST_MAX_NUM        16       /**< 公共组件中的线性表最大长度 */


/**
* @brief 内存块信息结构
*/
typedef struct
{
    unsigned int unMemStart;         /**< 分配的内存起始地址 */
    unsigned int unBlockSize;        /**< 内存块大小 */
    unsigned int unBlockCount;       /**< 内存块总个数 */
    nodelist_t memlist;             /**< 内存块链表 */
}mem_block_t,*p_mem_block_t;

/**
* @brief 内存块信息结构
*/
typedef struct
{
    unsigned int unMemStart;         /**< 分配的内存起始地址 */
    unsigned int unBlockSize;        /**< 内存块大小 */
    unsigned int unBlockCount;       /**< 内存块总个数 */
    unsigned int unUsedCount;        /**< 已使用内存块个数 */
}mem_block_state_t;


/**
* @brief 内存块的初始化
* @param[in] unMemStart分配的内存块起始地址
*            unBlockSize内存块大小
*            unBlockCount内存块计数
* @note 初始化内存块链表
* @retval 返回0表示初始化成功，返回非0表示失败
*/
int MEM_InitBlock(unsigned int unMemStart,unsigned int unBlockSize,unsigned int unBlockCount);

/**
* @brief 内存分配
* @param[in] size申请内存大小
* @note 
* @retval 返回分配的地址，如果为NULL表示分配失败
*/
void* MEM_Alloc(unsigned int size);

/**
* @brief 内存释放
* @param[in] pAddr释放的地址
* @note None
* @retval None
*/
void MEM_Free(void *pAddr);

/**
* @brief 获取各内存池状态
* @param[in] pMemState mem_block_state_t结构体数组
* @note None
* @retval None
*/
void MEM_GetState(mem_block_state_t  pMemState[POOL_MAX]);

/**
* @brief 去初始化内存池
* @param[in] unMemStart 待去初始化的内存池起始地址
* @note None
* @retval 返回0表示初始化成功，返回非0表示失败
*/
int MEM_DeInitBlock(unsigned int unMemStart);

#endif