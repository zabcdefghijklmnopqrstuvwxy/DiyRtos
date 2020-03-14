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

/**
*@brief 位图数据结构
*/
typedef struct _BITMAP{
		unsigned int unBitmap;    /**< 位图数据表 */    
}tBitmap_t,*p_tBitmap_t;

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


#endif