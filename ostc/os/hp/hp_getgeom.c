#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/diskio.h>
#include <sys/scsi.h>

void Describe_ext(int dfd)
{
  int ret;
  disk_describe_type_ext_t disk_descr;
  uint64_t  capacity;

  if ((ret = ioctl (dfd, DIOC_DESCRIBE_EXT, &disk_descr)) != 0) {
    perror("Error getting DIOC_DESCRIBE_EXT");
    exit(1);
  }
  printf("\nSuccessful ioctl DIOC_DESCRIBE_EXT \n");
  printf("  model number: %s\n", disk_descr.model_num);
  printf("  interface:    %d  <20=scsi>\n", disk_descr.intf_type);
  capacity = (disk_descr.maxsva_high << 32) + disk_descr.maxsva_low;
  printf("  Capacity:    %llu (blocks)\n", capacity);
  printf("  block size:  %u (bytes)\n", disk_descr.lgblksz);
  printf("  Device type:  %u (0=disk, 5=CD, 7=OM)\n",
	 disk_descr.dev_type);
  printf("  Write Protected:  %s \n",
	 disk_descr.flags & WRITE_PROTECT_FLAG ? "yes" : "No");

}

void SCSI_data(int fd)
{
  int ret;
  inquiry_3_t scsi_descr;

  if ((ret = ioctl (fd, SIOC_INQUIRY, &scsi_descr)) != 0) {
    perror("Error getting SIOC_INQUIRY");
    exit (1);
  }

  printf("\nSuccessful ioctl SIOC_INQUIRY\n");
  printf("  Vendor ID:   %.8s\n", scsi_descr.vendor_id);
  printf("  Product ID:  %.16s\n", scsi_descr.product_id);
  printf("  Rev Number:  %.4s\n", scsi_descr.rev_num);
  printf("  Vendor Spec: %.20s\n", scsi_descr.vendor_spec);
}

int main (int argc, char **argv)
{
  int ret, fd;
  if (argc != 2) {
    printf ("Usage: %s <disk_device> \n", argv[0]);
    exit(1);
  }
  if ((fd = open (argv[1], O_RDONLY)) < 0) {
    perror("Error opening disk device");
    exit (1);
  }
  Describe_ext(fd);
  SCSI_data(fd);

  return 0;
}
