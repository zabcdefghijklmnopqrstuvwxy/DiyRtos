/**
********************************************************************************
* @file    OS_MEM.c
* @author  Sai
* @version 01V01
* @date    2020-05-03
* @brief   OS_MEM.c
* @note    内存管理源代码
* @attention
********************************************************************************
*/

#include "OS_MEM.h"

static mem_block_t memhandle[POOL_MAX];           //内存管理句柄

/**
* @brief 内存块的初始化
* @param[in] unMemStart分配的内存块起始地址
*            unBlockSize内存块大小
*            unBlockCount内存块计数
* @note 初始化内存块链表
* @retval 返回0表示初始化成功，返回非0表示失败
*/
int OS_MEM_InitBlock(unsigned int unMemStart,unsigned int unBlockSize,unsigned int unBlockCount)
{
	mem_block_t *pmem = NULL;
    int32_t i = 0;
	uint32_t pool_start, pool_end, newpool_start, newpool_end;
    
    if(unBlockSize < sizeof(node_t))
    {
        return -1;
    }

	/* 判断起始地址是否4字节对其 */
	if (unMemStart%4 != 0)
	{
		return -1;
	}

	newpool_start = unMemStart;
	newpool_end = unMemStart + unBlockSize * unBlockCount;
	
	/* 判断内存池重叠 */
	for (i = 0; i< POOL_MAX; i++)
	{
		pool_start = memhandle[i].unMemStart;
		pool_end = pool_start + memhandle[i].unBlockSize * memhandle[i].unBlockCount;
		if (pool_start != 0)
		{
			if ((newpool_end > pool_start && newpool_end <= pool_end) ||
				(newpool_start >= pool_start && newpool_start < pool_end) ||
				(newpool_start < pool_start && newpool_end > pool_end))
			{
				/* 重叠 */
				return -1;
			}
		}
	}

	/* 判断是否达到最大内存池数 */
	for (i = 0; i< POOL_MAX; i++)
	{
		if (memhandle[i].unMemStart == 0)
		{
			break ;
		}
	}
	if (i >= POOL_MAX)
	{
		return -1;
	}

	pmem = &memhandle[i];
    pmem->unMemStart = unMemStart;
    pmem->unBlockSize = unBlockSize;
    pmem->unBlockCount = unBlockCount;
    
    OS_COM_InitList(&pmem->memlist);
    
    for(i = 0; i < unBlockCount; i++)
    {
        OS_COM_AddNode(&pmem->memlist,(node_t *)(pmem->unMemStart + i*unBlockSize));
    }
    
    return 0;  
}

/**
* @brief 内存分配
* @param[in] size申请内存大小
* @note 从空闲链表中获取一个可利用的空间给用户
* @retval 返回分配的地址，如果为NULL表示分配失败
*/
void* OS_MEM_Alloc(uint32_t size)
{
	uint32_t i;

	for (i = 0; i < POOL_MAX; i++)
	{
		if (memhandle[i].unMemStart != 0 && memhandle[i].unBlockSize == size &&
			memhandle[i].memlist.unNodeCnt > 0)
		{
			return (void*)OS_COM_DelNode(&memhandle[i].memlist, memhandle[i].memlist.head.pre);
		}
	}

    return NULL;   
}

/**
* @brief 内存释放
* @param[in] pAddr释放的地址
* @note 将释放空间加入到空闲链表中去
* @retval None
*/
void OS_MEM_Free(void *pAddr)
{
	uint32_t i, addr;
	uint32_t pool_start, block_count, block_size, pool_end;
	mem_block_t *phandle = NULL;
	p_node_t pnode = NULL;

	for (i = 0; i < POOL_MAX; i++)
	{
		pool_start = memhandle[i].unMemStart;
		block_count = memhandle[i].unBlockCount;
		block_size = memhandle[i].unBlockSize;
		pool_end = pool_start + block_count * block_size;

		if (pool_start != 0 && pool_start <= addr && addr < pool_end)
		{
			/* 找到所在内存池 */
			break;
		}
	}

	if (i >= POOL_MAX)
	{
		return ;
	}

	/* 判断地址是否未按块对其 */
	if ((addr-memhandle[i].unMemStart)%memhandle[i].unBlockSize != 0)
	{
		return ;
	}

	/* 判断该起始地址的内存块是否未被申请过 */
	pnode = (p_node_t)pAddr;
	if (pnode->pre != pAddr && pnode->next != pAddr &&
		pnode->pre->next == pAddr && pnode->next->pre == pAddr)
	{
		return ;
	}

	phandle = &memhandle[i];
    OS_COM_AddNode(&phandle->memlist, (p_node_t)addr);
}

/**
* @brief 获取各内存池状态
* @param[in] pMemState mem_block_state_t结构体数组
* @note None
* @retval None
*/
void OS_MEM_GetState(mem_block_state_t      pMemState[POOL_MAX])
{
	uint32_t i;

	for (i = 0; i < POOL_MAX; i++)
	{
		pMemState[i].unMemStart = memhandle[i].unMemStart;
		pMemState[i].unBlockSize = memhandle[i].unBlockSize;
		pMemState[i].unBlockCount = memhandle[i].unBlockCount;
		pMemState[i].unUsedCount = memhandle[i].unBlockCount - 
									memhandle[i].memlist.unNodeCnt;
	}
}

/**
* @brief 去初始化内存池
* @param[in] unMemStart 待去初始化的内存池起始地址
* @note None
* @retval 返回0表示初始化成功，返回非0表示失败
*/
int32_t OS_MEM_DeInitBlock(uint32_t unMemStart)
{
	uint32_t i;

	for (i = 0; i< POOL_MAX; i++)
	{
		if (memhandle[i].unMemStart == unMemStart)
		{
			break;
		}
	}

	if (i >= POOL_MAX)
	{
		return -1;
	}

	if (memhandle[i].memlist.unNodeCnt == memhandle[i].unBlockCount)
	{
		memhandle[i].unMemStart = 0;
		memhandle[i].unBlockSize = 0;
		memhandle[i].unBlockCount = 0;
		memhandle[i].memlist.unNodeCnt = 0;
		return 0;
	}
	else
	{
		return -1;
	}
}


