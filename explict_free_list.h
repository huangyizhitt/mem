#ifndef _EXPLICT_LIST_H_
#define _EXPLICT_LIST_H_

#include "utils.h"
#include "memory_system.h"

enum class InsertType {
    Middle_Insert,
    Middle_MergeForward,
    Middle_MergeBackward,
    Middle_MergeDoubleEnd, 
};

class FreeBlock {
public:
    FreeBlock(const uintptr_t& start = 0, const size_t& size = 0) 
        : next(nullptr), prev(nullptr), start(start), size(size) {}

    FreeBlock(const FreeBlock& other) : next(nullptr), prev(nullptr), 
        start(other.start), size(other.size) {}

    uintptr_t Alloc(size_t size);
 
    FreeBlock *next;
    FreeBlock *prev;
    uintptr_t start;
    size_t size;                            //block_size = payload_size + head_size;
};

class FreeList {
public:
    FreeList() {
        front = new FreeBlock(0, 0);
        tail = new FreeBlock(UINTPTR_MAX, 0);
        front->next = tail;
        front->prev = nullptr;
        tail->next = nullptr;
        tail->prev = front;
        free_blocks = 0;
    }
    ~FreeList();

    void PushBack(const FreeBlock& block);
    void Insert(FreeBlock *block);
    FreeBlock *FindFitBlock(size_t size);

private:
    FreeBlock *FindInsertPos(const FreeBlock *block, InsertType& type);
    void InsertBack(FreeBlock *prev, FreeBlock *block);
    void MergeForward(FreeBlock *prev_block, FreeBlock *next_block);
    void MergeBackward(FreeBlock *prev_block, FreeBlock *next_block);
    void MergeDoubleEnd(FreeBlock *prev_block, FreeBlock *mid_block, FreeBlock *next_block);

private:
    FreeBlock *front;
    FreeBlock *tail;
    uint64_t free_blocks;
};

class EXPLICT_FREE_LIST_MEM_SYS : public MEM_SYS {
public:
    EXPLICT_FREE_LIST_MEM_SYS();
    virtual ~EXPLICT_FREE_LIST_MEM_SYS();
    virtual void *Malloc(size_t size);
    virtual void Free(void *ptr);
    virtual void Expand(size_t asize);

private:
    FreeList list;
};

#endif