// To compile, use something like:
//
// g++ -o sysctl_test sysctl_test.cpp -I/Developer/Headers/FlatCarbon -framework CoreServices

#include <sys/types.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <iostream>

// Trying gestalt

#include <Gestalt.h>

using namespace std;

int main()
{
    int mib[4];
    char kern_version[32];
    size_t len = sizeof(kern_version);

    mib[0] = CTL_KERN;
    mib[1] = KERN_OSRELEASE;

    if (0 != sysctl(mib, 2, kern_version, &len, NULL, 0))
    {
        cout << "Error executing sysctl() system call: " << errno << endl;
        return 1;
    }

    cout << "Kernel version: '" << kern_version << "' (len=" << strlen(kern_version) << ")" << endl;

    SInt32 major, minor, bugfix;
    if (0 != Gestalt(gestaltSystemVersionMajor, &major)
        || 0 != Gestalt(gestaltSystemVersionMinor, &minor)
        || 0 != Gestalt(gestaltSystemVersionBugFix, &bugfix))
    {
        cout << "Error executing Gestalt() system call" << endl;
        return 1;
    }

    cout << "O/S Version: " << major << "." << minor << "." << bugfix << endl;

    // Look up the machine class (i386, etc)

    char machine_class[64];
    len = sizeof(machine_class);

    mib[0] = CTL_HW;
    mib[1] = HW_MACHINE;

    if (0 != sysctl(mib, 2, machine_class, &len, NULL, 0))
    {
        cout << "Error executing sysctl() system call: " << errno << endl;
        return 1;
    }

    cout << "Machine class: " << machine_class << endl;

    // sysctlnametomib() to figure out the IDs for the sysctl entries (header files are "weak")

    len = 2;

    if (0 != sysctlnametomib("hw.cpu64bit_capable", mib, &len))
    {
        cout << "Error executing sysctlnametomib() system call: " << errno << endl;
        return 1;
    }

    int bitflag;
    len = sizeof(bitflag);
    if (0 != sysctl(mib, 2, &bitflag, &len, NULL, 0))
    {
        cout << "Error executing sysctl() to get architecture length: " << errno << endl;
        return 1;
    }
    cout << "Bit flag: " << bitflag << endl;

    return 0;
}
