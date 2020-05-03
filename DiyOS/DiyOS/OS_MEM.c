/**
********************************************************************************
* @file    OS_MEM.c
* @author  Sai
* @version 01V01
* @date    2020-05-03
* @brief   OS_MEM.c
* @note    �ڴ����Դ����
* @attention
********************************************************************************
*/

#include "OS_MEM.h"

static mem_block_t memhandle[POOL_MAX];           //�ڴ������

/**
* @brief �ڴ��ĳ�ʼ��
* @param[in] unMemStart������ڴ����ʼ��ַ
*            unBlockSize�ڴ���С
*            unBlockCount�ڴ�����
* @note ��ʼ���ڴ������
* @retval ����0��ʾ��ʼ���ɹ������ط�0��ʾʧ��
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

	/* �ж���ʼ��ַ�Ƿ�4�ֽڶ��� */
	if (unMemStart%4 != 0)
	{
		return -1;
	}

	newpool_start = unMemStart;
	newpool_end = unMemStart + unBlockSize * unBlockCount;
	
	/* �ж��ڴ���ص� */
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
				/* �ص� */
				return -1;
			}
		}
	}

	/* �ж��Ƿ�ﵽ����ڴ���� */
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
* @brief �ڴ����
* @param[in] size�����ڴ��С
* @note �ӿ��������л�ȡһ�������õĿռ���û�
* @retval ���ط���ĵ�ַ�����ΪNULL��ʾ����ʧ��
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
* @brief �ڴ��ͷ�
* @param[in] pAddr�ͷŵĵ�ַ
* @note ���ͷſռ���뵽����������ȥ
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
			/* �ҵ������ڴ�� */
			break;
		}
	}

	if (i >= POOL_MAX)
	{
		return ;
	}

	/* �жϵ�ַ�Ƿ�δ������� */
	if ((addr-memhandle[i].unMemStart)%memhandle[i].unBlockSize != 0)
	{
		return ;
	}

	/* �жϸ���ʼ��ַ���ڴ���Ƿ�δ������� */
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
* @brief ��ȡ���ڴ��״̬
* @param[in] pMemState mem_block_state_t�ṹ������
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
* @brief ȥ��ʼ���ڴ��
* @param[in] unMemStart ��ȥ��ʼ�����ڴ����ʼ��ַ
* @note None
* @retval ����0��ʾ��ʼ���ɹ������ط�0��ʾʧ��
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


