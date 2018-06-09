#include <sys/times.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

int main()
{
    struct tms timeData;
    long long i;
    long ticks;

#ifdef _SC_CLK_TCK
    ticks = sysconf(_SC_CLK_TCK);
#else
    ticks = HZ;
#endif
    printf("Ticks derived:      %d\n", ticks);

    // Kill a little bit of time
    printf("Killing some time ...\n");
    time_t startTime = time(NULL);
    while (time(NULL) < startTime + 60)
    {
    }

    clock_t result = times(&timeData);

    printf("\n");
    printf("User time:          %d\n", timeData.tms_utime);
    printf("System time:        %d\n", timeData.tms_stime);
    printf("Child User time:    %d\n", timeData.tms_cutime);
    printf("Child system time:  %d\n", timeData.tms_cstime);

    long long total_ticks = (timeData.tms_utime + timeData.tms_stime + timeData.tms_cutime + timeData.tms_cstime);
    std::cout << std::endl << "Total ticks used:   " << total_ticks << std::endl;
    std::cout << "Total time (sec):   " << (total_ticks + ticks / 2) / ticks << std::endl;

    int logicalProcs = sysconf(_SC_NPROCESSORS_ONLN);
    std::cout << std::endl << "Logical processors: " << logicalProcs << std::endl;

    int percentBusy = ((total_ticks + (ticks / 2)) * 100) / (logicalProcs * ticks * 60);
    std::cout << "Percent CPU Busy:   " <<  percentBusy << std::endl;

    printf("Exiting ...\n");
    return 0;
}
