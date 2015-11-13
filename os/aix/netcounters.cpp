#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <libperfstat.h>
#include <net/if_types.h>
#include <iostream>

char *
decode(uchar type) {

    switch(type) {

    case IFT_LOOP:
        return("loopback");

    case IFT_ISO88025:
        return("token-ring");

    case IFT_ETHER:
        return("ethernet");
    }

    return("other");
}

int main(int argc, char* argv[]) {
   int i, ret, tot;
   perfstat_netinterface_t *statp;
   perfstat_id_t first;

   /* check how many perfstat_netinterface_t structures are available */
   tot = perfstat_netinterface(NULL, NULL, sizeof(perfstat_netinterface_t), 0);
   
   /* allocate enough memory for all the structures */
   statp = (perfstat_netinterface_t *) calloc(tot, sizeof(perfstat_netinterface_t));
   
   /* set name to first interface */
   strcpy(first.name, FIRST_NETINTERFACE);
   
   /* ask to get all the structures available in one call */
   /* return code is number of structures returned */
   ret = perfstat_netinterface(&first, statp, sizeof(perfstat_netinterface_t), tot);

   /* print statistics for each of the interfaces */
   for (i = 0; i < ret; i++) {
       std::cout << std::endl << "Statistics for interface : " << statp[i].name << std::endl;
       std::cout << "------------------------" << std::endl;
       std::cout << "type : " << decode(statp[i].type) << std::endl;
       std::cout << std::endl << "input statistics:" << std::endl;
       std::cout << "number of packets : " << statp[i].ipackets << std::endl;
       std::cout << "number of errors  : " << statp[i].ierrors << std::endl;
       std::cout << "number of bytes   : " << statp[i].ibytes << std::endl;
       std::cout << std::endl << "output statistics:" << std::endl;
       std::cout << "number of packets : " << statp[i].opackets << std::endl;
       std::cout << "number of bytes   : " << statp[i].obytes << std::endl;
       std::cout << "number of errors  : " << statp[i].oerrors << std::endl;
       }
}
