#ifndef _IMPLICT_FREE_LIST_H_
#define _IMPLICT_FREE_LIST_H_

#include <cstdint>

const int ALIGN_SHIFT = 3;
const uintptr_t ALIGN = (1 << ALIGN_SHIFT);
const int MEM_SIZE = 32 * 1024 * 1024;

class IMPLICT_FREE_LIST_MEM_SYS {
public:
    IMPLICT_FREE_LIST_MEM_SYS();
    ~IMPLICT_FREE_LIST_MEM_SYS();
    void CheckHeap();

private:
    void SetBlockSize(void *block, uint64_t size);
    void SetBlockAlloc(void *block, uint64_t size, bool alloc);
    uint64_t *GetFreeBlock(uint64_t size);

private:
    uint8_t *heap;
    uintptr_t offset;
    uint64_t heap_size;
};

#endif