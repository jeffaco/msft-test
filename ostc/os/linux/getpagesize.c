#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    printf("%d\n", getpagesize());
    return 0;
}
