// Compile with something like:
//
//   xlc_r -o bind_test bind_test.c

#include <libperfstat.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/kinfo.h>
#include <sys/ndd_var.h> // For nddictl etc...
#include <sys/ndd.h> // For ndd_t structure.
#include <netinet/if_ether.h> // For ETHERMTU
#include <sys/cdli_entuser.h> // For kent_all_stats_t
#include <sys/cdli_entuser.gxent.h> // For gxent_all_stats_t
#include <sys/cdli_entuser.goent.h> // For goent_all_stats_t
#include <sys/cdli_entuser.scent.h> // For scent_all_stats_t
#include <sys/cdli_entuser.phxent.h> // For phxent_all_stats_t Must be included after goent.h
#include <sys/cdli_entuser.ment.h> // For ment_all_stats_t
#include <sys/cdli_entuser.hea.h> // For hea_all_stats_t
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stropts.h>

//
// Function getkerninfo() has not been documented, but used in IBM examples.
// It also doesn't have prototype in headers, so we declare it here.
//
int getkerninfo(int, char *, int *, int32long64_t);

#include <stdio.h>      /* printf */
#include <time.h>       /* time_t, time, ctime */

const char* ethName = "en0";

void dumpMessage(char *message)
{
    time_t rawtime;
    time (&rawtime);
    printf ("%-50s %s", message, ctime (&rawtime));
}

void invokeBind()
{
    dumpMessage("Doing bind() invocation");

    // First, we need to connect to the adapter in question.
    struct sockaddr_ndd_8022 sa;
    int s;

    dumpMessage("Connecting to socket");
    s = socket(AF_NDD, SOCK_DGRAM, 0);
    if (s < 0)
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "Failed socket(AF_NDD,SOCK_DGRAM,0). errno: %d", errno);
        dumpMessage(buf);
        exit(1);
    }

    sa.sndd_8022_family = AF_NDD;
    sa.sndd_8022_len = sizeof(struct sockaddr_ndd_8022);
    sa.sndd_8022_filtertype = NS_TAP;
    sa.sndd_8022_filterlen = sizeof(ns_8022_t);
    strcpy((char *)sa.sndd_8022_nddname, ethName);

    dumpMessage("Binding to socket");
    if (bind(s, (struct sockaddr *)&sa, sizeof(struct sockaddr_ndd_8022)) < 0) 
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "bind() failed. errno: %d", errno);
        dumpMessage(buf);
        exit(1);
    }

    close(s);
}

void invokeKern()
{
    /* size_t i; */
    /* char maddr[6]; */
    /* char *pMacAddr = &maddr[0]; */
    /* memset(pMacAddr, '\0', 6); */
    /* bool succ = false; */

    struct kinfo_ndd *nddp;
    int size, nrec;

    dumpMessage("Entering function invokeKern");

    // First, let's get the size of the results
    dumpMessage("Calling getkerninfo for size");

    size = getkerninfo(KINFO_NDD, 0, 0, 0);
    if (size <= 0)
    {
        dumpMessage("No size returned; no MAC address available?");
        exit(1);
    }

    nrec = size / sizeof(struct kinfo_ndd);
    nddp = (struct kinfo_ndd *) malloc(size);

    {
        char buf[256];
        snprintf(buf, sizeof(buf), "Calling getkerninfo requesting %d elements", nrec);
        dumpMessage(buf);
    }

    if (getkerninfo(KINFO_NDD, (char *)nddp, &size, 0) < 0)
    {
        dumpMessage("Call to getkerninfo returned invalid number of elements");
        exit(1);
    }

    // nddp should be free'd, but not important for this test program
    dumpMessage("Exiting function invokeKern");
}

int main (int argc, char *argv[])
{
    if (argc != 2 || (strcmp("bind", argv[1]) && strcmp("getkerninfo", argv[1]) ) )
    {
        fprintf(stderr, "Invalid parameter - invoke with \"bind\" or \"getkerninfo\"\n");
        return 1;
    }

    dumpMessage ("Main program start");

    if (strcmp("bind", argv[1]) == 0)
    {
        invokeBind();
    }
    else if (strcmp("getkerninfo", argv[1]) == 0)
    {
        invokeKern();
    }
    else
    {
        fprintf(stderr, "Logic error with argument parsing\n");
        return 1;
    }

    dumpMessage ("Main program completed");
    return 0;
}
