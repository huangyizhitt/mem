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
//    printf("free blocks: %d\n", free_blocks);
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
}

EXPLICT_FREE_LIST_MEM_SYS::EXPLICT_FREE_LIST_MEM_SYS()
{
    heap_size = MEM_SIZE + head_size;
    uint8_t *head = new uint8_t[heap_size + ALIGN_SIZE];

    offset = ALIGN_SIZE - (uintptr_t)head % ALIGN_SIZE;

    heap = head + offset;

    uintptr_t start = (uintptr_t)heap;
    SetSize((Flag_t *)start, MEM_SIZE);
    list.Insert(new FreeBlock(start, heap_size));
}

EXPLICT_FREE_LIST_MEM_SYS::~EXPLICT_FREE_LIST_MEM_SYS()
{
    uint8_t *head = heap - offset;
    delete [] head;
}

void* EXPLICT_FREE_LIST_MEM_SYS::Malloc(size_t size)
{
    if(size == 0) return nullptr;

    size_t asize = ALIGN(size, ALIGN_SIZE);

    FreeBlock *block = list.FindFitBlock(asize);

    uintptr_t vaddr = block->Alloc(asize);

    printf("Malloc success: vaddr: 0x%lx, size: %ld\n", vaddr, asize);

    return (void *)vaddr;
}

void EXPLICT_FREE_LIST_MEM_SYS::Free(void *ptr)
{
    if(!ptr) return;

    uintptr_t start = (uintptr_t)ptr - head_size;
    size_t block_size = GetSize((Flag_t *)start) + head_size;

    FreeBlock *block = new FreeBlock(start, block_size);

    list.Insert(block);

    printf("Free success: vaddr: %p, block_size: %ld\n", ptr, block_size);
}