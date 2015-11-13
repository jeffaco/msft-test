#include <sys/pstat.h>
#include <sys/param.h>
#include <fcntl.h>
#include <stdio.h>

main(int argc , char **argv)
{
  struct pst_status pst[1];
  int count;
  char buf[MAXPATHLEN+1];
  pid_t pid;

  if ( argc != 2 ) {
    printf("Usage: pwdx <pid>\n");
    exit(1);
  }

  pid = atol (argv[1]);


  if ( getppid(pid) < 0 ) {

    fprintf(stderr,"Error: %s %d is not a valid pid.\n",
	    argv[1],pid);
    exit(1);
  }

  bzero(buf,MAXPATHLEN+1);

  if ( (count=pstat_getproc(pst, sizeof(pst[0]), 0, pid )) > 0 ) {

    printf("pid is %d, command is %s\n",
	   pst[0].pst_pid, pst[0].pst_ucomm);

  }

  count = pstat_getpathname(buf,MAXPATHLEN,&pst[0].pst_fid_cdir);

  if (count > 0) {
    printf("%s\n", buf);
    exit(0);
  }
  else {
    fprintf(stderr,"ERROR getting the CWD.\n");
    perror("pstat_getpathname");
    exit(1);
  }
}
