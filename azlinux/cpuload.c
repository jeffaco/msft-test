// This program generates CPU load until marker file is created.
// Marker file default: /root/cpuload.stop
//
// Compile this with something like: gcc cpuload.c -o cpuload

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
//#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Usage: cpuload <stop-file> <run time in seccond (or 0=indefinite)>\n");
        return -1;
    }

    char *stopName = argv[1];
    if (stopName == NULL || stopName[0] == '\0') {
        stopName = "/root/cpuload.stop";
    }
    float runtime = atof(argv[2]);

    const size_t size = 8 * 1024;

    int fdDevNull = open("/dev/null", O_WRONLY);
    if (fdDevNull == -1) {
        perror("Error opening handle to /dev/null");
        return -1;
    }

    //printf("Starting cpu loop ...\n");
    clock_t begin = clock();

    float timeTaken; 
    do
    {
        char buffer[size];
        ssize_t writeResult;

        // We don't want to hit file system for marker file that often,
        // so just create some busy work that can't really be optomized.
        for (unsigned int i = 0; i < 1024 * 512; ++i)
        {
            for (unsigned int j = 0; j < size; ++j) {
                buffer[j] = i % 255;
            }

            if ((writeResult = write(fdDevNull, buffer, size)) == -1) {
                perror("Error writing to file /dev/null");
                return -2;
            }
        }

        clock_t end = clock();
        timeTaken = (float)(end - begin) / CLOCKS_PER_SEC;

        // Check if marker file is created
        if (open(stopName, O_RDONLY) != -1)
            break;

        // Continue the loop?
        if (runtime > 0 && timeTaken > runtime)
            break;
    } while ( 1 );

    return 0;
}
