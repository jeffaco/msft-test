#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    const char *filename;

    if (argc != 2)
    {
        printf("Usage: createfile <filename to create>\n");
        return -1;
    }

    filename = argv[1];
    printf("Will create file named: %s\n", filename);

    unlink(filename);
    int fd = open(filename, O_CREAT | O_RDWR,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (-1 == fd)
    {
        printf("Failed to open file %s, error code: %d\n", filename, errno);
        return -1;
    }

    close(fd);
    return 0;
}
