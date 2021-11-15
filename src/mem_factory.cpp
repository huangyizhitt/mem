#include "mem_factory.h"
#include "implict_free_list.h"
#include "explict_free_list.h"

MEM_SYS *ExplictFreeListFactory::CreateMemsys()
{
    return new EXPLICT_FREE_LIST_MEM_SYS;
}