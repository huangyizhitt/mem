#include "explict_free_list.h"

FreeList::~FreeList()
{
    FreeBlock *block, *next_block;
    do {
        block = front;
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

FreeBlock* FreeList::FindFit(const FreeBlock *block, InsertType& type)
{
    if(free_blocks == 0) {
        type = InsertType::Middle_Insert;
        return front;
    }

    for(FreeBlock *b = front->next; b != tail; b = b->next) {
        FreeBlock *next_b = b->next;
        if(b->start + b->size < block->start && block->start + block->size < next_b->start) {
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
}

void FreeList::MergeForward(FreeBlock *prev_block, FreeBlock *next_block)
{

}

void FreeList::MergeBackward(FreeBlock *prev_block, FreeBlock *next_block)
{

}

void FreeList::MergeDoubleEnd(FreeBlock *prev_block, FreeBlock *mid_block, FreeBlock *next_block)
{
    
}

void FreeList::Insert(FreeBlock *block)
{
    InsertType insert_type;
    FreeBlock *b = FindFit(block, insert_type);
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