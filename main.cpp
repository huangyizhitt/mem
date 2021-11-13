//#include "implict_free_list.h"
#include "explict_free_list.h"

int main(int argc, char **argv)
{
    int alloc_size[7] = {1,3,2,5,7,10,11}; 
 //   int alloc_size[] = {1};
    int *a[7];
    EXPLICT_FREE_LIST_MEM_SYS mem_sys;
//    mem_sys.CheckHeap();
    for(int i = 0; i < 7; i++) {
        a[i] = (int *)mem_sys.Malloc(alloc_size[i]);
    }
    
    for(int i = 0; i < 7; i++) {
        mem_sys.Free(a[i]);
    }
    return 0;
}