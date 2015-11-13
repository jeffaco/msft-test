/*
 *
 * Syslog test program.
 *
 * Here's a sample syslog line:

   Jun  1 15:54:14 scxcore-suse01 sudo:  jeffcof : TTY=pts/1 ; PWD=/home/jeffcof ; 
   USER=root ; COMMAND=/usr/bin/less /var/log/messages
 *
 */

#include <syslog.h>

int main()
{
    openlog("sudo", 0, LOG_KERN);

    syslog(LOG_ALERT, " jeffcof : TTY=pts/1 ; PWD=/home/jeffcof ; USER=root ; COMMAND=/usr/bin/less /var/log/messages (fake)");

    closelog();

    return 0;
}
