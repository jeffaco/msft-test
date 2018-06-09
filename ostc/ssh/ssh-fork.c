// ssh-fork.c sample program
//
// Compile this with a command like: cc -o ssh-fork ssh-fork.c

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int do_cmd(char *host, char *remuser, char *cmd, int *fdin, int *fdout);

pid_t do_cmd_pid = -1;

const char *ssh_program = "ssh";	// Program to execute for the secured connection
int verbose_mode = 1;			// Set to zero for non-verbose mode


static void
killchild(int signo)
{
	if (do_cmd_pid > 1) {
		kill(do_cmd_pid, signo ? signo : SIGTERM);
		waitpid(do_cmd_pid, NULL, 0);
	}

	if (signo)
		_exit(1);
	exit(1);
}

static void
suspchild(int signo)
{
	int status;

	if (do_cmd_pid > 1) {
		kill(do_cmd_pid, signo);
		while (waitpid(do_cmd_pid, &status, WUNTRACED) == -1 &&
		    errno == EINTR)
			;
		kill(getpid(), SIGSTOP);
	}
}

/*
 * This function executes the given command as the specified user on the
 * given host.  This returns < 0 if execution fails, and >= 0 otherwise. This
 * assigns the input and output file descriptors on success.
 */

int do_cmd(char *host, char *remuser, char *cmd, int *fdin, int *fdout)
{
	int pin[2], pout[2], reserved[2];

	if (verbose_mode)
		fprintf(stderr,
		    "Executing: program %s host %s, user %s, command %s\n",
		    ssh_program, host,
		    remuser ? remuser : "(unspecified)", cmd);

	/*
	 * Reserve two descriptors so that the real pipes won't get
	 * descriptors 0 and 1 because that will screw up dup2 below.
	 */
	if (pipe(reserved) < 0)
		fatal("pipe: %s", strerror(errno));

	/* Create a socket pair for communicating with ssh. */
	if (pipe(pin) < 0)
		fatal("pipe: %s", strerror(errno));
	if (pipe(pout) < 0)
		fatal("pipe: %s", strerror(errno));

	/* Free the reserved descriptors. */
	close(reserved[0]);
	close(reserved[1]);

	signal(SIGTSTP, suspchild);
	signal(SIGTTIN, suspchild);
	signal(SIGTTOU, suspchild);

	/* Fork a child to execute the command on the remote host using ssh. */
	do_cmd_pid = fork();
	if (do_cmd_pid == 0) {
		/* Child. */
		close(pin[1]);
		close(pout[0]);
		dup2(pin[0], 0);
		dup2(pout[1], 1);
		close(pin[0]);
		close(pout[1]);

                /* replacearg(&args, 0, "%s", ssh_program); */
                /* if (remuser != NULL) { */
                /*     addargs(&args, "-l"); */
                /*     addargs(&args, "%s", remuser); */
                /* } */
		/* addargs(&args, "--"); */
		/* addargs(&args, "%s", host); */
		/* addargs(&args, "%s", cmd); */

		execvp(ssh_program, args.list);
		perror(ssh_program);
		exit(1);
	} else if (do_cmd_pid == -1) {
		fatal("fork: %s", strerror(errno));
	}
	/* Parent.  Close the other side, and return the local side. */
	close(pin[0]);
	*fdout = pin[1];
	close(pout[1]);
	*fdin = pout[0];
	signal(SIGTERM, killchild);
	signal(SIGINT, killchild);
	signal(SIGHUP, killchild);
	return 0;
}


int main(int argc, char **argv)
{
    return 0;
}
