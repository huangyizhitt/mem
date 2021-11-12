#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstdint>

#define __ALIGN_KERNEL(x, a)        __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)    (((x) + (mask)) & ~(mask))

#define ALIGN(x, a)     __ALIGN_KERNEL((x), (a))

bool TestAlign(uintptr_t addr, uintptr_t align)
{
    return ((addr & (align - 1)) == 0);
}

const int ALIGN_SHIFT = 3;
const uintptr_t ALIGN_SIZE = (1 << ALIGN_SHIFT);
const int MEM_SIZE = 32 * 1024 * 1024;

#endif
