#include "mem_factory.h"
#include "implict_free_list.h"
#include "explict_free_list.h"

MEM_SYS *ExplictFreeListFactory::CreateMemsys()
{
    return new EXPLICT_FREE_LIST_MEM_SYS;
}

MEM_SYS *ImplictFreeListFactory::CreateMemsys()
{
    return new IMPLICT_FREE_LIST_MEM_SYS;
}