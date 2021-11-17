#include "implict_free_list.h"
#include <cstdio>
#include <cstdlib>

void IMPLICT_FREE_LIST_MEM_SYS::SetBlockFlag(Block block, size_t size, bool alloc)
{
    uintptr_t head_addr = (uintptr_t)block;
    uintptr_t foot_addr = head_addr + size + head_size;
    Flag_t *head_flag = (Flag_t *)head_addr;
    Flag_t *foot_flag = (Flag_t *)foot_addr;

    bool is_heap_head = isHeapHead(head_flag);
    *head_flag = (size << ALIGN_SHIFT) | (is_heap_head << HEAP_HEAD_SHFIT) | (alloc & 0x1);

    bool is_heap_foot = isHeapFoot(foot_flag);
    *foot_flag = (size << ALIGN_SHIFT) | (is_heap_foot << HEAP_FOOT_SHFIT) | (alloc & 0x1);
}

void IMPLICT_FREE_LIST_MEM_SYS::InitHeap(const Heap& heap, const size_t& heap_size)
{
    uintptr_t heap_head_addr = (uintptr_t)heap.heap;
    uintptr_t heap_foot_addr = heap_head_addr + heap_size + head_size;
    SetFlag((Flag_t *)heap_head_addr, heap_size, false);
    SetFlag((Flag_t *)heap_foot_addr, heap_size, false);
    SetHeapFlag((Flag_t *)heap_head_addr, (Flag_t *)heap_foot_addr);
}

IMPLICT_FREE_LIST_MEM_SYS::IMPLICT_FREE_LIST_MEM_SYS()
{
    Heap heap = MEM_SYS::GetHeaps()[0];
    InitHeap(heap, MEM_SIZE);
}

Block IMPLICT_FREE_LIST_MEM_SYS::FindFit(size_t size)
{
    std::vector<Heap> heaps = MEM_SYS::GetHeaps();
    for(int i = 0; i < heaps.size(); i++) {
        uintptr_t start = (uintptr_t)heaps[i].heap;
        uintptr_t end = start + heaps[i].heap_size + head_size + foot_size;
        while(start < end) {
            size_t payload_size = GetSize((Flag_t *)start);
            size_t block_size = payload_size + head_size + foot_size;
            if(size <= payload_size && !isAlloc((Flag_t *)start))    return (Block)start;
            start += block_size;
        }
    }

    return NoneBlock;
}

size_t IMPLICT_FREE_LIST_MEM_SYS::Expand(size_t size)
{
    size = MEM_SYS::Expand(size);

    Heap heap = MEM_SYS::GetHeaps().back();
    InitHeap(heap, size);
    return size;
}

void *IMPLICT_FREE_LIST_MEM_SYS::AllocBlock(Block block, size_t size)
{
    // alloc address
    uintptr_t head_addr = (uintptr_t)block;
    void *ptr = (void *)(head_addr + head_size);

    //split block
    size_t old_size = GetSize((Flag_t *)head_addr);
    size_t block_size = size + head_size + foot_size;

    if(old_size >= block_size) {
        //need splite block;
        SetBlockFlag(block, size, true);

        Block next_block = block + block_size;
        size_t next_block_size = old_size - block_size;
        SetBlockFlag(next_block, next_block_size, false);
    } else {
        //use the whole block, only set allocated bit to 1
        SetBlockAllocated(block);
    }

    return ptr;
}

void* IMPLICT_FREE_LIST_MEM_SYS::Malloc(size_t size)
{
    Block fit_block = FindFit(size);

    if(fit_block == NoneBlock) {
        Expand(size);
        fit_block = FindFit(size);
    }

    return AllocBlock(fit_block, size);
}

Block IMPLICT_FREE_LIST_MEM_SYS::MergeBlock(Block block, Block next_block)
{
    uintptr_t head_addr = (uintptr_t)block;
    size_t block_size = GetSize((Flag_t *)head_addr) + head_size + foot_size;
    size_t next_block_size = GetSize((Flag_t *)next_block) + head_size + foot_size;
    uintptr_t foot_addr = head_addr + block_size + next_block_size - foot_size;

    size_t size = block_size + next_block_size - foot_size - head_size;

    SetBlockFlag(block, size, false);

    return block;
}

void IMPLICT_FREE_LIST_MEM_SYS::Free(void *ptr)
{
    Block block = (Block)((uintptr_t)ptr - head_size);

    uintptr_t head_addr = (uintptr_t)block;
    size_t block_size = GetSize((Flag_t *)head_addr) + head_size + foot_size;
    uintptr_t foot_addr = head_addr+block_size-foot_size;

    if(isHeapHead((Flag_t *)head_addr) && isHeapFoot((Flag_t *)foot_addr)) {
        // the block is heap, set allocated bit to 0
        SetBlockFree((Flag_t *)head_addr, (Flag_t *)foot_addr);
        return;
    } 
    
    Block next_block = block + block_size;
    if(!isHeapFoot((Flag_t *)foot_addr) && !isAlloc((Flag_t *)next_block)){
        // If the block is not the last block of the heap, and the next block is free
        // The block and next_block can be merged
        block = MergeBlock(block, next_block);
    }

    if(!isHeapHead((Flag_t *)head_addr)) {
        uintptr_t prev_foot_addr = head_addr - foot_size;
        size_t prev_block_size = GetSize((Flag_t *)prev_foot_addr) + head_size + foot_size;
        Block prev_block = block - prev_block_size;

        if(!isAlloc((Flag_t *)prev_block)) {
            block = MergeBlock(prev_block, block);
        }
    }
}
