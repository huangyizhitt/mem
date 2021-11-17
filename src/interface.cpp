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

        case Implement::Implicit_Free_List:
            mem_sys_factory = new ImplictFreeListFactory;
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
    if(size == 0) return nullptr;
    size_t asize = ALIGN(size, ALIGN_SIZE);

    void *ptr = mem_sys->Malloc(asize);
    debugp("Malloc success! Address: %p, size: %lu\n", ptr, asize);
    return ptr;
}

void Free(void *ptr)
{
    mem_sys->Free(ptr);
    debugp("Free address: %p success!\n", ptr);
}