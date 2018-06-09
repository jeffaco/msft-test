#include <stdio.h>
#include <mntent.h>
#include <sys/statvfs.h>

main(int argc, char** argv)
{
    struct statvfs fsd;

    if ( argc != 2 )
    {
        printf("Usage: show_statvfs <filesystem>\n");
        return 1;
    }

    char *filesystem = argv[1];

    if (statvfs (filesystem, &fsd) < 0) {
        perror("Failure calling statvfs");
        return 1;
    }

    printf("\tf_bsize:      %ld\n", fsd.f_bsize);
    printf("\tBlock Size:   %ld\n", fsd.f_frsize);
    printf("\tTotal Blocks: %ld\n", fsd.f_blocks);
    printf("\tBlocks free:  %ld\n", fsd.f_bfree);

    printf("\tInode information:\n");
    printf("\t  Inodes available:  %ld\n", fsd.f_files);
    printf("\t  Inodes free:       %ld\n", fsd.f_ffree);
    printf("\t  Number of files:   %ld\n", fsd.f_files - fsd.f_ffree);

    printf("\n");
    printf("\tf_fsid:       %ld\n", fsd.f_bfree);
    printf("\tf_flag:       %ld\n", fsd.f_flag);

    printf("\n");
    return 0;
}
