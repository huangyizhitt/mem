#include "implict_free_list.h"
#include <cstdio>
#include <cstdlib>

bool TestAlign(uintptr_t addr, uintptr_t align)
{
    return ((addr & (align - 1)) == 0);
}

void IMPLICT_FREE_LIST_MEM_SYS::SetBlockSize(void *block, uint64_t size)
{
    uint8_t *head = (uint8_t *)block;
    uint64_t *block_head = (uint64_t *)head;
    uint64_t *block_foot = (uint64_t *)(head + sizeof(uint64_t) + size);

    uint64_t value = (size << ALIGN_SHIFT);
    *block_head = value;
    *block_foot = value;
}

uint64_t* IMPLICT_FREE_LIST_MEM_SYS::GetFreeBlock(uint64_t size)
{
    uint64_t inner_size = size + 16;
    for(uint8_t *block = heap; block < heap+heap_size; block += inner_size) {
        if(size <= (*block >> ALIGN_SHIFT) && (*block & 0x01 == 0)) {
            return (uint64_t *)block;
        }
    }

    return nullptr;
}

void IMPLICT_FREE_LIST_MEM_SYS::SetBlockAlloc(void *block, uint64_t size, bool alloc)
{
    uint8_t *head = (uint8_t *)block;
    uint64_t *block_head = (uint64_t *)head;
    uint64_t *block_foot = (uint64_t *)(head + sizeof(uint64_t) + size);

    uint64_t value = alloc ? 0x1 : 0x0;
    *block_head = *block_foot = (*block_head | value); 
}

IMPLICT_FREE_LIST_MEM_SYS::IMPLICT_FREE_LIST_MEM_SYS()
{
    heap_size = MEM_SIZE + 16;
    uint8_t *head = new uint8_t[heap_size + ALIGN];

    offset = ALIGN - (uintptr_t)head % ALIGN;

    heap = head + offset;

    SetBlockSize(heap, MEM_SIZE);
    SetBlockAlloc(heap, MEM_SIZE, false);
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