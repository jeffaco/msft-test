//#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <ostream>
//#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int sig, numSigs, j, sigData;
    union sigval sv;

    if (argc < 4 || strcmp(argv[1], "--help") == 0) {
        printf("%s pid sig-num data [num-sigs]\n", argv[0]);
        return 1;
    }

    /* Display our PID and UID, so that they can be compared with the
       corresponding fields of the siginfo_t argument supplied to the
       handler in the receiving process */

    // std::cout << argv[0] << ": PID is " << (long) getpid() << ", UID is " << (long) getuid() << std::endl;

    // printf("%s: PID is %ld, UID is %ld\n", argv[0],
            // (long) getpid(), (long) getuid());

    // std::cout << argv[0] << ": SIGRTMIN=" << SIGRTMIN << ", SIGRTMAX=" << SIGRTMAX << std::endl;
    // printf("%s: SIGRTMIN=%d, SIGRTMAX=%d\n", argv[0],
           // SIGRTMIN, SIGRTMAX);

    pid_t pid = atol(argv[1]);
    sig = atoi(argv[2]);
    sigData = atoi(argv[3]);
    numSigs = (argc > 4) ? atoi(argv[4]) : 1;

    for (j = 0; j < numSigs; j++) {
        sv.sival_int = sigData + j;
        if (sigqueue(pid, sig, sv) == -1) {
            perror("Failed sigqueue");
            return 2;
        }
    }

    exit(EXIT_SUCCESS);
}
