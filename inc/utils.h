#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstdint>
#include <cstdlib>
#include <cstdio>

#define __ALIGN_KERNEL(x, a)        __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)    (((x) + (mask)) & ~(mask))

#define ALIGN(x, a)     __ALIGN_KERNEL((x), (a))

#ifdef DEBUG
#define debugp(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debugp(fmt, ...)
#endif


static inline bool TestAlign(uintptr_t addr, uintptr_t align)
{
    return ((addr & (align - 1)) == 0);
}

const int ALIGN_SHIFT = 3;
const uintptr_t ALIGN_SIZE = (1 << ALIGN_SHIFT);
const int MEM_SIZE = 32 * 1024 * 1024;

using Flag_t = uintptr_t;
const size_t head_size = sizeof(Flag_t);
const size_t foot_size = sizeof(Flag_t);


static inline void SetSize(Flag_t *flag, size_t size)
{
    *flag = (size << ALIGN_SHIFT);
}

static inline size_t GetSize(Flag_t *flag)
{
    return (*flag >> ALIGN_SHIFT);
}

static inline void SetFlag(Flag_t *flag, size_t size, bool alloc)
{
    *flag = (size << ALIGN_SHIFT) | (alloc & 0x1);
}

static inline void SetAllocated(Flag_t *flag)
{
    *flag |= 0x1;
}

static inline void SetFree(Flag_t *flag)
{
    *flag &= (~0x1);
}

static inline bool isAlloc(Flag_t *flag) 
{
    return (*flag & 0x1);
}

#endif
