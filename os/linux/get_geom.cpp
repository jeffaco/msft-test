#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <linux/types.h>

#include <linux/fs.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>


static int do_blkgetsize (int fd, unsigned long long *blksize64)
{
        int             rc;
        unsigned int    blksize32 = 0;

#ifdef BLKGETSIZE64
        if (0 == ioctl(fd, BLKGETSIZE64, blksize64)) {  // returns bytes
                *blksize64 /= 512;
                return 0;
        }
#endif
        rc = ioctl(fd, BLKGETSIZE, &blksize32); // returns sectors
        if (rc) {
                rc = errno;
                perror(" BLKGETSIZE failed");
        }
        *blksize64 = blksize32;
        return rc;
}


int main(int argc, char* argv[])
{
    int fd = 0;

    if (argc < 2)
    {
        printf("USAGE: get_geom <device file>\n");
        return 0;
    }

    fd = open(argv[1], O_RDONLY);

    if (0 > fd)
    {
        perror("Failed to open input file");
        return 0;
    }

    // Fetch the drive model information (only available for IDE drives)
    __u16 id[256];

    if (!ioctl(fd, HDIO_GET_IDENTITY, id)) {
        printf(" Model=%.40s,\n\tFirmware Revision=%.8s,\n\tSerialNo=%.20s\n\n",
               (char *) &id[27], (char *) &id[23], (char *) &id[10]);
    }
    else if (errno == -ENOMSG) {
        printf(" no identification info available\n");
    }
    else {
        perror(" HDIO_GET_IDENTITY failed (SCSI drive?)");
        printf("\n");
    }

    // Fetch drive read/only status
    static long parm;
    if (ioctl(fd, BLKROGET, &parm)) {
        perror(" ioctl BLKROGET failed");
        return 1;
    }
    else {
        printf(" readonly      = %2ld\n", parm);
    }

    // Fetch the block read-ahead size
    if (ioctl(fd, BLKRAGET, &parm)) {
        perror(" ioctl BLKRAGET failed");
        return 1;
    }
    else {
        printf(" readahead     = %2ld\n", parm);
    }

    // Fetch the geometry information
    unsigned long long blksize;
    struct hd_geometry g;
    static const char msg[] = " geometry      = %u/%u/%u, sectors = %lld, start = %ld\n";

    if (! do_blkgetsize(fd, &blksize)) {
        if (ioctl(fd, HDIO_GETGEO, &g)) {
            perror(" HDIO_GETGEO failed");
        }
        else {
            printf(msg, g.cylinders, g.heads, g.sectors, blksize, g.start);
        }
    }

    close(fd);

    return 0;
}
