#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <cstdlib>

enum class Implement {
    Explicit_Free_List = 0,
    Implicit_Free_List,
    RB_Tree,
};

void InitMemSys(Implement impl);
void DeInitMemSys();
void *Malloc(size_t size);
void Free(void *ptr);

#endif