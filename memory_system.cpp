#include "memory_system.h"
#include "utils.h"

void MEM_SYS::AllocHeap(Heap& heap, size_t size)
{
    heap.heap_size = size + head_size;
    uint8_t *head = new uint8_t[heap.heap_size + ALIGN_SIZE];

    heap.offset = ALIGN_SIZE - (uintptr_t)head % ALIGN_SIZE;

    heap.heap = head + heap.offset;

    heaps.emplace_back(heap);
}

void MEM_SYS::Expand(size_t asize)
{
    Heap heap;
    size_t expand_size;
    if(asize < MEM_SIZE) {
        expand_size = MEM_SIZE;
    } else {
        expand_size = asize << 1;
    }
    AllocHeap(heap, expand_size);
    printf("Expand, expand size: %lu\n", expand_size);
}

MEM_SYS::MEM_SYS()
{
    Heap heap;
    AllocHeap(heap, MEM_SIZE);
}

MEM_SYS::~MEM_SYS()
{
    for(int i = 0; i < heaps.size(); i++) {
        uint8_t *head = heaps[i].heap - heaps[i].offset;
        delete [] head;
    } 

    printf("Call destructor of MEM_SYS\n");
}