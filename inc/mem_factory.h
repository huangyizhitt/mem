#ifndef _MEM_FACTORY_H_
#define _MEM_FACTORY_H_

#include "memory_system.h"

class MemsysFactory {
public:
    virtual MEM_SYS *CreateMemsys() = 0;
};

class ExplictFreeListFactory : public MemsysFactory {
public:
    virtual MEM_SYS *CreateMemsys();
};

#endif