#include <stdio.h>
#include <unistd.h>

int main()
{
    long retval = sysconf(_SC_CLK_TCK);
    printf("_SC_CLK_TCK: %ld\n", retval);

    return 0;
}
