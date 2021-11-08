#include "implict_free_list.h"
#include <cstdio>
#include <cstdlib>

bool TestAlign(uintptr_t addr, uintptr_t align)
{
    return ((addr & (align - 1)) == 0);
}

void IMPLICT_FREE_LIST_MEM_SYS::SetBlockSize(Block *block, uint64_t size)
{
    uint8_t *head = (uint8_t *)block;
    uint64_t *block_head = (uint64_t *)head;
    uint64_t *block_foot = (uint64_t *)(head + sizeof(uint64_t) + size);

    uint64_t flag_size = 2 * sizeof(uint64_t);
    uint64_t old_size = (*block >> ALIGN_SHIFT);
    uint64_t block_size = size + flag_size;
    uint64_t value = (size << ALIGN_SHIFT);
    *block_head = value;
    *block_foot = value;
    
    if(old_size > flag_size && block_size < old_size - flag_size) {
        Block *next_block = (Block *)(head + block_size);
        uint64_t next_size = old_size - block_size - 2 * sizeof(uint64_t);
        uint64_t *next_block_head = (uint64_t *)next_block;
        uint64_t *next_block_foot = (uint64_t *)(head + old_size + sizeof(uint64_t));
        *next_block_foot = *next_block_head = (next_size << ALIGN_SHIFT) & 0xF8;
    }
}

Block* IMPLICT_FREE_LIST_MEM_SYS::GetFreeBlock(uint64_t size)
{
    uint64_t inner_size = size + 16;
    for(uint8_t *block_addr = heap; block_addr < heap+heap_size; block_addr += inner_size) {
        Block *block = (Block *)block_addr;
        uint64_t flag = *block;
        uint64_t block_size = flag >> ALIGN_SHIFT;
        bool alloc_flag = flag & 0x01;
        if(size <= block_size && alloc_flag == 0) {
            return block;
        }
    }

    return nullptr;
}

Block *IMPLICT_FREE_LIST_MEM_SYS::GetNextBlock(Block *block)
{
    uint64_t flag = *block;
    uint64_t inner_size = (flag >> ALIGN_SHIFT) + 16;

    uint8_t *addr = (uint8_t *)block;
    uint8_t *next_block_addr = addr + inner_size;
    if(next_block_addr < heap + MEM_SIZE)
        return (Block *)next_block_addr;
    else
        return nullptr;  
}

bool IMPLICT_FREE_LIST_MEM_SYS::GetNextBlockAllocFlag(Block *block)
{
    Block *next_block = GetNextBlock(block);
    if(!next_block) return true;
    return (*next_block & 0x01);
}

Block *IMPLICT_FREE_LIST_MEM_SYS::GetPrevBlock(Block *block)
{
    uint8_t *block_addr = (uint8_t *)heap;
    if(block_addr == heap)   return nullptr;
    uint64_t *prev_block_foot = block - 1;
    uint64_t inner_size = *prev_block_foot >> ALIGN_SHIFT;
    return (Block *)(block_addr - inner_size);
}

bool IMPLICT_FREE_LIST_MEM_SYS::GetPrevBlockAllocFlag(Block *block)
{
    Block *prev_block = GetPrevBlock(block);
    if(!prev_block) return true;
    return (*prev_block & 0x01);
}

bool IMPLICT_FREE_LIST_MEM_SYS::CheckBlock(Block *block)
{
    if(!block) return false;
    uint64_t flag = *block;
    uint64_t inner_size = (flag >> ALIGN_SHIFT) + 16;
    bool alloc_flag = flag & 0x01;
    if(alloc_flag == true || (GetNextBlockAllocFlag(block) && GetPrevBlockAllocFlag(block))) {
        printf("Block Right!\n");
        return true;
    } else {
        printf("Block Fail!\n");
        return false;
    }
}

void IMPLICT_FREE_LIST_MEM_SYS::SetBlockAlloc(Block *block)
{
    uint8_t *head = (uint8_t *)block;
    uint64_t size = *block >> ALIGN_SHIFT;
    uint64_t *block_head = (uint64_t *)head;
    uint64_t *block_foot = (uint64_t *)(head + sizeof(uint64_t) + size);

    *block_head = *block_foot = (*block_head | 0x01); 
}

IMPLICT_FREE_LIST_MEM_SYS::IMPLICT_FREE_LIST_MEM_SYS()
{
    heap_size = MEM_SIZE + 16;
    uint8_t *head = new uint8_t[heap_size + ALIGN];

    offset = ALIGN - (uintptr_t)head % ALIGN;

    heap = head + offset;

    Block *first_block = (Block *)heap;

    SetBlockSize(first_block, MEM_SIZE);
    SetBlockFree(first_block);
}

IMPLICT_FREE_LIST_MEM_SYS::~IMPLICT_FREE_LIST_MEM_SYS()
{
    uint8_t *head = heap - offset;

    delete [] head;
}

void IMPLICT_FREE_LIST_MEM_SYS::CheckHeap()
{
    uintptr_t heap_addr = (uintptr_t)heap;
    
    if(!TestAlign(heap_addr, ALIGN)) {
        printf("Heap alloc error at %p, can't align: %lu\n", heap, ALIGN);
        exit(1);
    }
}

void *IMPLICT_FREE_LIST_MEM_SYS::Malloc(uint64_t size)
{
    if(size > MEM_SIZE) return nullptr;
    Block *block = GetFreeBlock(size);
    if(!block) return nullptr;
    SetBlockSize(block, size);
    SetBlockAlloc(block);

    void *vaddr = (void *)(block + 1);
    return vaddr;
}

Block *IMPLICT_FREE_LIST_MEM_SYS::MergeBlock(Block *prev, Block *cur)
{
    uint64_t prev_size = *prev >> ALIGN_SHIFT;
    uint64_t cur_size = *cur >> ALIGN_SHIFT;
    uint64_t block_size = prev_size+cur_size+16;
    uint8_t *cur_head_addr = (uint8_t *)cur;

    uint64_t *block_head = (uint64_t *)prev;
    uint64_t *block_foot = cur + cur_size + 8;
    *block_foot = *block_head = (block_size << ALIGN_SHIFT);

    return prev;
}

void IMPLICT_FREE_LIST_MEM_SYS::SetBlockFree(Block *block)
{
    uint64_t flag = *block;
    uint8_t *block_head = (uint8_t *)block;
    uint64_t size = (flag >> ALIGN_SHIFT);

    Block *next_block = GetNextBlock(block);
    Block *prev_block = GetPrevBlock(block);

    if(!CheckBlockAlloc(next_block) && !CheckBlockAlloc(prev_block)) {
        *block  = (flag & (~0x07));
        CheckBlock(block);
        return ;
    }

    if(next_block && !CheckBlockAlloc(next_block)) {
        MergeBlock(block, next_block);
    }

    if(prev_block && !CheckBlockAlloc(prev_block)) {
        prev_block = MergeBlock(prev_block, block);
    }

    CheckBlock(prev_block);
}

void IMPLICT_FREE_LIST_MEM_SYS::Free(void *vaddr)
{
    Block *block = (Block *)vaddr - 1;
    uint64_t flag = *block;
    uint64_t size = (flag >> ALIGN_SHIFT);
    SetBlockFree(block);
}