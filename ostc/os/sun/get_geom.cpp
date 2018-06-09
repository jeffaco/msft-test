#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/dkio.h>
#include <sys/vtoc.h>

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

    struct dk_cinfo dkinfo;

    int status = ioctl(fd, DKIOCINFO, &dkinfo);

    if (0 == status)
    {
        printf("Results from ioctl DKIOCINFO:\n");
        printf("\tcname=%s, dname=%s\n", dkinfo.dki_cname, dkinfo.dki_dname);
        printf("\tctype=%u, unit=%u, partition=%u\n",
               dkinfo.dki_ctype, dkinfo.dki_unit, dkinfo.dki_partition);
    }
    else {
        perror("Error executing ioctl for DKIOCINFO");
        return 1;
    }

    struct dk_geom dkgeom;

    status = ioctl(fd, DKIOCGGEOM, &dkgeom);

    if (0 == status)
    {
        printf("\nResults from ioctl DKIOCGGEOM:\n");
        printf("\tncyl=%u, nhead=%u, nsect=%u, rpm=%u, pcyl=%u\n",
               dkgeom.dkg_ncyl, dkgeom.dkg_nhead, dkgeom.dkg_nsect,
               dkgeom.dkg_rpm, dkgeom.dkg_pcyl);
    }
    else {
        perror("Error executing ioctl for DKIOCGGEOM");
        return 1;
    }

    close(fd);

    return 0;
}
