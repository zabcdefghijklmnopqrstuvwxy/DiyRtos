/**
********************************************************************************
* @file    OS_COM.h
* @author  Sai
* @version 01V01
* @date    2020-03-08
* @brief   系统公共功能头文件
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

/*< 计算数据节点的首地址 */
#define  NODEPARENT(node,parent,name)  (parent*)((unsigned int)node - (unsigned int)&((parent*)0)->name)

/*< 获取第一个节点 */
#define	 GET_FIRST_NODE(head) 					(head.next)

/**
*@brief 位图数据结构
*/
typedef struct _BITMAP{
		unsigned int unBitmap;    /**< 位图数据表 */    
}tBitmap_t,*p_tBitmap_t;

/**
*@brief 双向链表节点
*/
typedef struct _NODE{
		struct _NODE *pre;     /**< 前驱节点 */
		struct _NODE *next;    /**< 后驱节点 */
}node_t,*p_node_t;

/**
*@brief 双向链表
*/
typedef struct _NODELIST{
		node_t head;      					/**< 链表头 */         
	  unsigned int unNodeCnt;     /**< 链表计数 */
}nodelist_t,*p_nodelist_t;


/**
 * @brief 位图初始化
 * @param[in] pbitmap 位图数据指针
 * @note 将位图数据清零
 * @retval 无
 */
void OS_COM_InitBitmap(p_tBitmap_t pbitmap);

/**
 * @brief 设置位图
 * @param[in] pbitmap 位图数据指针 pos 设置的位图位置
 * @note 将pos与位图进行或运算
 * @retval 无
 */
void OS_COM_SetBitmap(p_tBitmap_t pbitmap,unsigned int pos);

/**
 * @brief 清除位图
 * @param[in] pbitmap 位图数据指针 pos 待清除的位图位置
 * @note 无
 * @retval 无
 */
void OS_COM_ClrBitmap(p_tBitmap_t pbitmap,unsigned int pos);

/**
 * @brief 获取第一个位置
 * @param[in] pbitmap 位图数据指针
 * @note 从位图查找表中查找第一个置一的位置，默认0为最高31为最低
 * @retval 无
 */
unsigned int OS_COM_GetFirstBit(p_tBitmap_t pbitmap);

/**
 * @brief 双向链表节点初始化
 * @param[in] phead 头节点
 * @note 将双向链表计数清零，前驱节点和后驱节点都指向头节点
 * @retval 返回0表示初始化成功，返回非0表示初始化失败
 */
int OS_COM_InitList(p_nodelist_t phead);

/**
 * @brief 双向链表节点的添加
 * @param[in] phead 头节点，pnode 待添加的节点
 * @note 尾部插入节点
 * @retval 返回0表示插入成功，返回非0表示插入失败
 */
int OS_COM_AddNode(p_nodelist_t phead,p_node_t pnode);

/**
 * @brief 双向链表节点的删除
 * @param[in] phead 头节点，pnode 待删除的节点
 * @note 无
 * @retval 返回0表示删除成功，返回非0表示删除失败
 */
int OS_COM_DelNode(p_nodelist_t phead,p_node_t pnode);

/**
 * @brief 移除双向链表节点的第一个节点
 * @param[in] phead 头节点
 * @note 无
 * @retval 返回删除后的第一个节点
 */
p_node_t OS_COM_RemoveFirstNode(p_nodelist_t phead);

/**
 * @brief 获取双向链表中个数
 * @param[in] phead 链表指针
 * @note 无
 * @retval 返回链表节点个数 
 */
unsigned int OS_COM_GetNodeCount(p_nodelist_t phead);

#endif
