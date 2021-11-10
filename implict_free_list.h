#ifndef _IMPLICT_FREE_LIST_H_
#define _IMPLICT_FREE_LIST_H_

#include <cstdint>
#include <functional>

using Block = uint64_t;
using BlockMeta = uint64_t;
using Addr_t = uint8_t;

const int ALIGN_SHIFT = 3;
const uintptr_t ALIGN_SIZE = (1 << ALIGN_SHIFT);
const int MEM_SIZE = 32 * 1024 * 1024;

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

#endif