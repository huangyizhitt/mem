#include "interface.h"
#include "memory_system.h"
#include "implict_free_list.h"
#include "explict_free_list.h"

static MEM_SYS *mem_sys;

void InitMemSys(Implement impl)
{
    switch(impl) {
        case Implement::Explicit_Free_List:
            mem_sys = new EXPLICT_FREE_LIST_MEM_SYS;
            break;

        case Implement::Implicit_Free_List:
            
            break;

        case Implement::RB_Tree:
            break;

        default:
            break;
    }
}

void DeInitMemSys()
{
    delete mem_sys;
}

void *Malloc(size_t size)
{
    mem_sys->Malloc(size);
}

void Free(void *ptr)
{
    mem_sys->Free(ptr);
}