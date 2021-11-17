#include "explict_free_list.h"

uintptr_t FreeBlock::Alloc(size_t size)
{
    uintptr_t addr = this->start + head_size;
    SetSize((Flag_t *)this->start, size);      //Set allocated block size

    this->start = addr + size;
    this->size -= (size + head_size);
    SetSize((Flag_t *)start, this->size - head_size);
    return addr;
}

FreeList::~FreeList()
{
    FreeBlock *block = front, *next_block;
    do {
        next_block = block->next;
        delete block;
        block = next_block;
    } while(block != nullptr);
}

void FreeList::PushBack(const FreeBlock& block)
{
    FreeBlock *b = new FreeBlock(block);

    FreeBlock *tmp = tail->prev;

    tail->prev = b;
    tmp->next = b;
    b->prev = tmp;
    b->next = tail;
}

FreeBlock* FreeList::FindInsertPos(const FreeBlock *block, InsertType& type)
{
    if(free_blocks == 0) {
        type = InsertType::Middle_Insert;
        return front;
    }

    for(FreeBlock *b = front; b != tail; b = b->next) {
        FreeBlock *next_b = b->next;
        if(b->start > block->start || next_b->start < block->start) {
            continue;
        } if(b->start + b->size < block->start && block->start + block->size < next_b->start) {
            type = InsertType::Middle_Insert;
        } else if(b->start + b->size <= block->start && block->start + block->size < next_b->start) {
            type = InsertType::Middle_MergeForward; 
        } else if(b->start + b->size < block->start && block->start + block->size <= next_b->start) {
            type = InsertType::Middle_MergeBackward;
        } else {
            type = InsertType::Middle_MergeDoubleEnd;
        }

        return b;
    }

    return nullptr;
}

void FreeList::InsertBack(FreeBlock *prev, FreeBlock *block)
{
    FreeBlock *tmp = prev->next;
    prev->next = block;
    tmp->prev = block;
    block->next = tmp;
    block->prev = prev;
    free_blocks++;
}

void FreeList::MergeForward(FreeBlock *prev_block, FreeBlock *next_block)
{
    prev_block->size += next_block->size;
    SetSize((Flag_t *)prev_block->start, prev_block->size - head_size);
    delete next_block;
}

void FreeList::MergeBackward(FreeBlock *prev_block, FreeBlock *next_block)
{
    next_block->start = prev_block->start;
    next_block->size += prev_block->size;
    SetSize((Flag_t *)next_block->start, next_block->size - head_size);
    delete prev_block;
}

void FreeList::MergeDoubleEnd(FreeBlock *prev_block, FreeBlock *mid_block, FreeBlock *next_block)
{
    prev_block->size += mid_block->size + next_block->size;
    prev_block->next = next_block->next;
    prev_block->next->prev = prev_block;
    SetSize((Flag_t *)prev_block->start, prev_block->size - head_size);

    delete mid_block;
    delete next_block;
    free_blocks--;
}

void FreeList::Insert(FreeBlock *block)
{
    InsertType insert_type;
    FreeBlock *b = FindInsertPos(block, insert_type);
    FreeBlock *next_b = b->next;

    switch(insert_type) {
        case InsertType::Middle_Insert:
            InsertBack(b, block);
            break;

        case InsertType::Middle_MergeForward:
            MergeForward(b, block);
            break;

        case InsertType::Middle_MergeBackward:
            MergeBackward(block, next_b);
            break;

        case InsertType::Middle_MergeDoubleEnd:
            MergeDoubleEnd(b, block, next_b);
            break;
    }
}

FreeBlock *FreeList::FindFitBlock(size_t size)
{
    for(FreeBlock *b = front->next; b != tail; b = b->next) {
        if(b->size >= size) 
            return b;
    }

    return nullptr;
}

EXPLICT_FREE_LIST_MEM_SYS::EXPLICT_FREE_LIST_MEM_SYS()
{
    Heap heap = MEM_SYS::GetHeaps()[0];
    uintptr_t start = (uintptr_t)heap.heap;
    SetSize((Flag_t *)start, MEM_SIZE);
    list.Insert(new FreeBlock(start, heap.heap_size));
}

EXPLICT_FREE_LIST_MEM_SYS::~EXPLICT_FREE_LIST_MEM_SYS()
{
    
}

size_t EXPLICT_FREE_LIST_MEM_SYS::Expand(size_t asize)
{
    MEM_SYS::Expand(asize);
    Heap heap = MEM_SYS::GetHeaps().back();
    uintptr_t start = (uintptr_t)heap.heap;
    SetSize((Flag_t *)start, heap.heap_size);
    list.Insert(new FreeBlock(start, heap.heap_size));

    return heap.heap_size;
}

void* EXPLICT_FREE_LIST_MEM_SYS::Malloc(size_t size)
{
    FreeBlock *block = list.FindFitBlock(size);

    // Can not find fit block, expand
    if(!block) {
        Expand(size);
        block = list.FindFitBlock(size);
    }

    uintptr_t vaddr = block->Alloc(size);

    return (void *)vaddr;
}

void EXPLICT_FREE_LIST_MEM_SYS::Free(void *ptr)
{
    if(!ptr) return;

    uintptr_t start = (uintptr_t)ptr - head_size;
    size_t block_size = GetSize((Flag_t *)start) + head_size;

    FreeBlock *block = new FreeBlock(start, block_size);

    list.Insert(block);
}