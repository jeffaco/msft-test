#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <libperfstat.h>
#include <iostream>

int main(int argc, char* argv[]) {
   perfstat_id_t id;
   perfstat_disk_t data;

   strcpy(id.name, FIRST_DISKPATH);

   do {
       int r = perfstat_disk(&id, &data, sizeof(data), 1);
       if (1 == r && 0 != strncmp(data.name, "cd", 2)) // TODO: better way to exclude CD/DVD!
           std::cout << "Response: " << data.name << std::endl;
   } while ( 0 != strcmp(id.name, FIRST_DISKPATH) );
}
