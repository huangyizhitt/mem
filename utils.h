#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstdint>
#include <cstdlib>
#include <cstdio>

#define __ALIGN_KERNEL(x, a)        __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)    (((x) + (mask)) & ~(mask))

#define ALIGN(x, a)     __ALIGN_KERNEL((x), (a))

static inline bool TestAlign(uintptr_t addr, uintptr_t align)
{
    return ((addr & (align - 1)) == 0);
}

const int ALIGN_SHIFT = 3;
const uintptr_t ALIGN_SIZE = (1 << ALIGN_SHIFT);
const int MEM_SIZE = 32 * 1024 * 1024;

const size_t head_size = 8;
using Flag_t = uint64_t;

static inline void SetSize(Flag_t *flag, size_t size)
{
    *flag = (size << ALIGN_SHIFT);
}

static inline size_t GetSize(Flag_t *flag)
{
    return (*flag >> ALIGN_SHIFT);
}

#endif
