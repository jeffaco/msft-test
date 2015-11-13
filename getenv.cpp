#include <stdlib.h>
#include <stdio.h>

int main()
{
    char *p = getenv("SCX_TESTRUN_NAMES");
    if (p)
	printf("Found $SCX_TESTRUN_NAMES: %s\n", p);
    else
	printf("Did not find $SCX_TESTRUN_NAMES\n");

    return 0;
}
