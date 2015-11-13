// Test for getargs() function on AIX (C RTL)
//
// Test with something like:
//  xlc++ getargs_test.cpp -o getargs_test

#include <unistd.h>
#include <iostream>

#include <procinfo.h>
#include <sys/types.h>

extern "C" int getargs(struct procentry64*, int, char*, int);

using namespace std;

int main()
{
    pid_t myPid = getpid();
    struct procentry64 processBuffer;
    char args[1024];

    memset(&processBuffer, 0, sizeof(processBuffer));
    memset(&args, 0, sizeof(args));

    processBuffer.pi_pid = myPid;
    int result = getargs(&processBuffer, sizeof(processBuffer), &args[0], sizeof(args));

    cout << "My PID:\t" << myPid << endl;
    cout << "Arguments:" << endl;

    int count = 0;
    char *argP = &args[0];
    while ( argP < &args[0] + sizeof(args) && *argP )
    {
	cout << "  " << count++ << ": " << argP << endl;
	argP += strlen(argP) + 1;
    }

    return 0;
}
