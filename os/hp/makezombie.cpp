// Makes a zombie process ...
//
// Compile with something like:  g++ -o makezombie makezombie.cpp

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid = fork();
    if (-1 == pid)      // Fork failed
    {
        fprintf(stderr, "Failed to fork process, errno: %d\n", errno);
        return -1;
    }

    if (pid == 0)       // Child process
    {
        // Immediately exit

        _exit(-5);
    }

    // We be the parent ...

    fprintf(stderr, "Child launched with PID: %d\n", pid);
    sleep(300);

    return 0;
}
