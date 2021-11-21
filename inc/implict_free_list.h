#ifndef _IMPLICT_FREE_LIST_H_
#define _IMPLICT_FREE_LIST_H_

#include <functional>
#include "utils.h"
#include "memory_system.h"

using Block = uintptr_t;
using BlockMeta = uintptr_t;
using Addr_t = uint8_t;

const Block NoneBlock = 0;
const int HEAP_HEAD_SHFIT = 1;
const int HEAP_FOOT_SHFIT = 2;

class IMPLICT_FREE_LIST_MEM_SYS : public MEM_SYS {
public:
    IMPLICT_FREE_LIST_MEM_SYS();
    virtual ~IMPLICT_FREE_LIST_MEM_SYS() {}

    virtual void *Malloc(size_t size);
    virtual void Free(void *ptr);
    virtual size_t Expand(size_t size);

private:
    inline void SetHeapFlag(Flag_t *heap_head, Flag_t *heap_foot) { 
        *heap_head |= (0x1 << HEAP_HEAD_SHFIT);
        *heap_foot |= (0x1 << HEAP_FOOT_SHFIT);
    }

    inline bool isHeapHead(Flag_t *head) const {return (*head >> HEAP_HEAD_SHFIT) & 0x1;}
    inline bool isHeapFoot(Flag_t *foot) const {return (*foot >> HEAP_FOOT_SHFIT) & 0x1;}

    inline void SetBlockAllocated(Block block) {
        uintptr_t head = (uintptr_t)block;
        size_t size = GetSize((Flag_t *)head);
        uintptr_t foot = head + size + head_size;

        SetAllocated((Flag_t *)head);
        SetAllocated((Flag_t *)foot);
    }
    void SetBlockFlag(Block block, size_t size, bool alloc);
    void InitHeap(const Heap& heap, const size_t& heap_size);
    void *AllocBlock(Block block, size_t size);
    Block MergeBlock(Block block, Block next_block);

    inline void SetBlockFree(Flag_t *head, Flag_t *foot) {
        SetFree(head);
        SetFree(foot);
    }

    inline void SetBlockFree(Block block) {
        Flag_t *head = (Flag_t *)block;
        Flag_t *foot = (Flag_t *)(block + GetSize(head) + head_size + foot_size);

        SetFree(head);
        SetFree(foot);
    }

private:
    Block FindFit(size_t size);
};

/*
class IMPLICT_FREE_LIST_MEM_SYS {
public:
    IMPLICT_FREE_LIST_MEM_SYS();
    ~IMPLICT_FREE_LIST_MEM_SYS();
    void *Malloc(uint64_t size);
    void Free(void *vaddr);
    void CheckHeap();

private:
    using pFunc = std::function<void(Block *block, void *args)>;
    void SetBlockSize(Block *block, uint64_t size);
    void SetBlockAlloc(Block *block);
    void TraverseList(Block *first_block, pFunc func, void *args);
    Block *GetAllocatableBlock(uint64_t size);
    bool CheckBlock(Block *block);
    Block *GetNextBlock(Block *block);
    bool GetNextBlockAllocFlag(Block *block);
    Block *GetPrevBlock(Block *block);
    bool GetPrevBlockAllocFlag(Block *block);
    void SetBlockFree(Block *block);
    Block *MergeBlock(Block *prev, Block *cur);
    inline bool CheckBlockAlloc(Block *block) {
        if(!block) return true;
        return (*block & 0x01);
    }

    inline void PrintAllocStatus(bool flag) {
        if(flag) {
            printf("A");
        } else {
            printf("F");
        }
    }

private:
    Addr_t *heap;
    uintptr_t offset;
    uint64_t heap_size;
};
*/

#endif