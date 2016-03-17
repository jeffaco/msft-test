// Build with something like: xlc++ -o getvminfo getvminfo.cpp -O0 -g

#include <stdio.h>
#include <stdlib.h>
#include <sys/vminfo.h>
#include <iostream>

int main(int argc, char* argv[])
{
    struct vminfo vm;

    int r = vmgetinfo(&vm, VMINFO_ABRIDGED, sizeof(struct vminfo));
    if ( r != 0 )
    {
        printf("Error: vmgetinfo() returned %d\n", r);
        return r;
    }

    if ( vm.minperm == 0 )
        printf("if says minperm = 0\n");
    else
        printf("if says minperm != 0\n");

    printf("minperm = %lld\n", vm.minperm);
    std::cout << "minperm (via stream) is: " << vm.minperm << std::endl;
    //printf("minperm boot = %d\n", vmpf_minperm);
    return 0; 
}
