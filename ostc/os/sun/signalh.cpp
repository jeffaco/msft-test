#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <ostream>

#include <pthread.h>


// #define DAEMONIZE 1

#if defined(DAEMONIZE)

// open() and friends (in Process_Daemonize)
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

//
// Support making us a daemon if selected
//

int Process_Daemonize()
{
    int pid;

    /* Fork for the first time */
    pid = fork();
    if (pid == -1)
        return -1;

    /* If parent, then exit; let child live on. */
    if (pid > 0)
        exit(0);

    /* Become the session leader (return if this fails) */
    if (setsid() == -1)
        return -1;

    /* Fork a second time */
    pid = fork();
    if (pid == -1)
        return -1;

    /* If parent (first child), then exit; let second child live on */
    if (pid > 0)
        exit(0);

    /* Close all file descriptors inherited by this process */
#if 0
    {
        int i;
        for (i = 0; i < 64 ; i++)
            close(i);
    }
#else
    close(0);
    close(1);
    close(2);
#endif

    /* Tie stdin to /dev/null */
    open("/dev/null", O_RDONLY);

    /* Tie stdout to /dev/null */
    open("/dev/null", O_RDWR);

    /* Tie stderr to /dev/null */
    open("/dev/null", O_RDWR);

    return 0;
}

#endif // defined(DAMONIZE)

static volatile int handlerSleepTime;
static volatile int sigCnt = 0;         /* Number of signals received */
static volatile int allDone = 0;

extern "C" static void             /* Handler for signals established using SA_SIGINFO */
siginfoHandler(int sig, siginfo_t *si, void *ucontext)
{
    /* UNSAFE: This handler uses non-async-signal-safe functions
       (printf()); see Section 21.1.2) */

    /* SIGINT or SIGTERM can be used to terminate program */

    if (sig == SIGINT || sig == SIGTERM || (sig == SIGRTMIN && si->si_value.sival_int == 1)) {
        std::cout << "Returning from handler due to signal: " << sig << std::endl;
        // printf("Returning from handler due to signal: %d\n", sig);
        allDone = 1;
        return;
    }

    sigCnt++;
    std::cout << "Caught signal: " << sig << std::endl;
    // printf("caught signal %d\n", sig);

    // printf("    si_signo=%d, si_code=%d (%s), ", si->si_signo, si->si_code,
            // (si->si_code == SI_USER) ? "SI_USER" :
            // (si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
    std::cout << "    si_signo=" 
              << si->si_signo 
              << ", si->si_code=" 
              << si->si_code 
              << "(" 
              << ((si->si_code == SI_USER) ? "SI_USER" : (si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other")
              << ")" << std::endl;

    // printf("si_value=%d\n", si->si_value.sival_int);
    std::cout << "si_value=<< si->si_value.sival_int" << std::endl;

    // printf("    si_pid=%ld, si_uid=%ld\n", (long) si->si_pid, (long) si->si_uid);
    std::cout << "    si_pid=" << (long) si->si_pid << ", si_uid=" << (long) si->si_uid << std::endl;

    sleep(handlerSleepTime);
}

int
main(int argc, char *argv[])
{
    int sig;
    sigset_t prevMask, blockMask;

    std::cout << "Starting server..." << std::endl;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        // printf("%s [block-time [handler-sleep-time]]\n", argv[0]);
        std::cout << argv[0] << " [block-time [handler-sleep-time]]" << std::endl;
        return 1;
    }

#if defined(DAEMONIZE)
    if (Process_Daemonize())
    {
        // printf("Failed to damonize process ...");
        std::cout << "Failed to damonize process ..." << std::endl;
        return 1;
    }
#else
    // printf("%s: PID is %ld\n", argv[0], (long) getpid());
    std::cout << argv[0] << ": PID is " << (long) getpid() << std::endl;
#endif

    handlerSleepTime = (argc > 2) ? atoi(argv[2]) : 1;

    /* Establish handler for most signals. During execution of the handler,
       mask all other signals to prevent handlers recursively interrupting
       each other (which would make the output hard to read). */

    struct sigaction sa;
    memset( &sa, 0, sizeof(sa) );
    sa.sa_sigaction = siginfoHandler;
    sa.sa_flags = SA_SIGINFO;
    //sigfillset(&sa.sa_mask);
    sigemptyset( &sa.sa_mask );

#if 0
    for (sig = 1; sig < NSIG; sig++)
        if (sig != SIGTSTP && sig != SIGQUIT)
            sigaction(sig, &sa, NULL);
#endif
    sigaction(SIGRTMIN, &sa, NULL);

    sigset_t blockedSignals;
    sigemptyset( &blockedSignals );
    sigaddset( &blockedSignals, SIGRTMIN+1 );
    (void) pthread_sigmask(SIG_BLOCK, &blockedSignals, NULL);

    /* Optionally block signals and sleep, allowing signals to be
       sent to us before they are unblocked and handled */

    if (argc > 1) {
        sigfillset(&blockMask);
        sigdelset(&blockMask, SIGINT);
        sigdelset(&blockMask, SIGTERM);

        if (sigprocmask(SIG_SETMASK, &blockMask, &prevMask) == -1) {
            perror("sigprocmask");
            return 2;
        }

        // printf("%s: signals blocked - sleeping %s seconds\n", argv[0], argv[1]);
        std::cout << argv[0] << ": signals blocked - sleeping " << argv[1] << " seconds" << std::endl;;
        sleep(atoi(argv[1]));
        // printf("%s: sleep complete\n", argv[0]);
        std::cout << argv[0] << ": sleep complete" << std::endl;

        if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1) {
            perror("sigprocmask");
            return 1;
        }
    }

    while (!allDone)                    /* Wait for incoming signals */
        pause();
}
