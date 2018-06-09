#include <unistd.h>
#include <linux/reboot.h>
#include <iostream>
#include <errno.h>
#include <sys/reboot.h>

int main()
{
    sync();
    int ret = reboot(LINUX_REBOOT_CMD_RESTART);
    if (ret == -1)
    {
        std::cerr<<"Reboot failed, errno="<<errno<<std::endl;
    }
    return 0;
}

