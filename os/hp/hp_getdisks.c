#include <devnm.h>
#include <sys/pstat.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

/***********************************************************************/
/*                                                                     */
/* Input Parameters : mjr, mnr - Major and minor number of device from */
/*                    the pst_diskinfo structure.                      */
/*                                                                     */
/* Return Value:  string which contains device name.                   */
/*                                                                     */
/* Globals referenced:  No.                                            */
/*                                                                     */
/* Description:                                                        */
/*                                                                     */
/* Given the  major and minor numbers of device from psd_dev field of  */
/* diskinfo structure; use devnm() to find the device name.            */
/*                                                                     */
/***********************************************************************/

char *device_name (uint32_t mjr, uint32_t mnr)
{
static char     name_buf[80];

if (devnm(S_IFBLK, makedev(mjr,mnr), name_buf, 80, 1) == 0)
        return(strrchr(name_buf,'/')+1);
else
   {
        (void) sprintf(name_buf,"0x%lx/0x%lx",mjr,mnr);
        return (name_buf);
   }
} /* End of device_name (mjr, mnr) */

int main(void)
{
    struct pst_diskinfo pst[10];
    int i, count;

    /* Get information about disks configured in the system */
    count = pstat_getdisk(pst, sizeof(struct pst_diskinfo),
                          sizeof(pst) / sizeof(struct pst_diskinfo), 0);

    if (count < 0) {
        perror("pstat_getdisk()");
        return 1;
    }

    for (i = 0; i < count; i++) {
        printf("Disk hardware path: %s\n", pst[i].psd_hw_path.psh_name);
        printf("Device name:        %s\n",
               device_name(pst[i].psd_dev.psd_major,
                           pst[i].psd_dev.psd_minor));
    }

    return 0;
}
