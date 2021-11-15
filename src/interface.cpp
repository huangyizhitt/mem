#include "interface.h"
#include "mem_factory.h"

static MEM_SYS *mem_sys;
static MemsysFactory *mem_sys_factory;

static void SelectFactory(Implement impl)
{
    switch(impl) {
        case Implement::Explicit_Free_List:
            mem_sys_factory = new ExplictFreeListFactory;
            break;

        default:
            break;
    }
}

void InitMemSys(Implement impl)
{
    SelectFactory(impl);
    mem_sys = mem_sys_factory->CreateMemsys();
}

void DeInitMemSys()
{
    delete mem_sys;
    delete mem_sys_factory;
}

void *Malloc(size_t size)
{
    mem_sys->Malloc(size);
}

void Free(void *ptr)
{
    mem_sys->Free(ptr);
}