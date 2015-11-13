// Build with something like: xlc++ -o perfstat_network perfstat_network.cpp -O0 -g -lperfstat

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <libperfstat.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    perfstat_id_t first;
    int structsAvailable = perfstat_netinterface(NULL, NULL, sizeof(perfstat_netinterface_t), 0);
    if (structsAvailable < 0) {
        printf("Error: No structures available, errno=%d\n", errno);
        exit(1);
    }
    else
        printf("Structures available: %d\n", structsAvailable);

    std::vector<char> buffer(structsAvailable * sizeof(perfstat_netinterface_t));
    perfstat_netinterface_t *statp = reinterpret_cast<perfstat_netinterface_t *>(&buffer[0]);
    strcpy(first.name, FIRST_NETINTERFACE);
    int structsReturned = perfstat_netinterface(&first, statp, sizeof(perfstat_netinterface_t), structsAvailable);
    if (structsReturned < 0) {
        printf("Error: No structures returned, errno=%d\n", errno);
        exit(1);
    }
    else
        printf("Structures returned: %d\n", structsReturned);

    for (int nr = 0; nr < structsReturned; nr++) {
        // Currently there is no way to return type of network, our current CIM-model supports ethernet
        if (statp[nr].type != IFT_ETHER) {
            printf("Name: %s, Not ethernet, type=%d\n", statp[nr].name, statp[nr].type);
            continue;
        }

        printf("Name: %s, type=ethernet (%d)\n", statp[nr].name, statp[nr].type);
        printf("\tPackets sent: %d\n", statp[nr].opackets);
        printf("\tPackets recv: %d\n", statp[nr].ipackets);
        printf("\tBytes sent:   %d\n", statp[nr].obytes);
        printf("\tBytes recv:   %d\n", statp[nr].ibytes);
        printf("\tErrors sent:  %d\n", statp[nr].oerrors);
        printf("\tErrors recv:  %d\n", statp[nr].ierrors);
        printf("\tCollisions:   %d\n", statp[nr].collisions);
    }

    return 0;
}
