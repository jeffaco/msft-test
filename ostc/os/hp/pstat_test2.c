#include <sys/pstat.h>

/* Get information about all processes -- 10 at a time. */

#define BURST ((size_t)10)

int main(void)
{
  struct pst_status pst[BURST];
  int i, count;
  int idx = 0; /* index within the context */

  /* loop until count == 0 */

  while ((count = pstat_getproc(pst, sizeof(pst[0]), BURST, idx))
	 > 0) {
    /* got count (max of BURST) this time. Process them */
    for (i = 0; i < count; i++) {
      printf("%d (pid), command is %s\n", pst[i].pst_pid,
	     pst[i].pst_ucomm);
    }
    /*
     * Now go back and do it again, using the next index after
     * the current 'burst'
     */
    idx = pst[count-1].pst_idx + 1;
  }
  if (count == -1) {
    perror("pstat_getproc()");
  }

  return 0;
}
