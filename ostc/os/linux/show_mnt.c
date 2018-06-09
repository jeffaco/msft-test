#include <stdio.h>
#include <mntent.h>
#include <sys/statvfs.h>

main(){
    struct mntent *mnt;
    char *table = (char *) MOUNTED;
    FILE *fp;
    int ret;

    fp = setmntent (table, "r");
    if (fp == NULL) {
        perror("Failure calling setmntent");
        return 1;
    }

    while ((mnt = getmntent (fp)) != NULL) {
        struct statvfs fsd;

        // Only report stats on a "real" file system (i.e. a name with "/dev/...")
        if (mnt->mnt_fsname[0] != '/')
            continue;

        printf("Found device:\n\tName: %s\n", mnt->mnt_fsname);
        printf("\tMountDir: %s\n", mnt->mnt_dir);
        printf("\tType: %s\n", mnt->mnt_type);
        printf("\tOptions: %s\n", mnt->mnt_opts);

        if (statvfs (mnt->mnt_dir, &fsd) < 0) {
            perror("Failure calling statvfs");
            return 1;
        }

        printf("\tBlock Size:   %ld\n", fsd.f_frsize);
        printf("\tTotal Blocks: %ld\n", fsd.f_blocks);
        printf("\tBlocks free:  %ld\n", fsd.f_bfree);

        printf("\tInode information:\n");
        printf("\t  Inodes available:  %ld\n", fsd.f_files);
        printf("\t  Inodes free:       %ld\n", fsd.f_ffree);
        printf("\t  Number of files:   %ld\n", fsd.f_files - fsd.f_ffree);

        printf("\n");
    }

    endmntent(fp);
    return 0;
}
