#ifndef _MEMORY_SYSTEM_H_
#define _MEMORY_SYSTEM_H_

#include <vector>
#include "utils.h"

struct Heap {
    uint8_t *heap;
    uintptr_t offset;
    size_t heap_size;
};

class MEM_SYS {
public:
    MEM_SYS();
    virtual ~MEM_SYS();
    inline std::vector<Heap>& GetHeaps() {return heaps;}

    virtual void *Malloc(size_t size) = 0;
    virtual void Free(void *ptr) = 0;
    virtual void Expand(size_t asize);

private:
    void AllocHeap(Heap& heap, size_t size);

private:
    std::vector<Heap> heaps;
};

#endif