#include "interface.h"
#include <vector>

#define MB(x) (x*1024*1024)

int main(int argc, char **argv)
{
    int alloc_size[] = {1,3,2,5,7,10,11,MB(33)}; 
//    int alloc_size[] = {MB(33)};
    std::vector<int> alloc_sizes(alloc_size, alloc_size + sizeof(alloc_size)/sizeof(int));
    std::vector<int *> ptrs;
    ptrs.resize(alloc_sizes.size());

//    InitMemSys(Implement::Implicit_Free_List);
    InitMemSys(Implement::Explicit_Free_List);

    for(int i = 0; i < ptrs.size(); i++) {
        ptrs[i] = (int *)Malloc(alloc_size[i]);
    }
    
    for(int i = 0; i < ptrs.size(); i++) {
        Free(ptrs[i]);
    }

    DeInitMemSys();
    return 0;
}