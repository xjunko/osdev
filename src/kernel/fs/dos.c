// dos.c - sus
#include <kernel/ata.h>
#include <kernel/dos.h>
#include <kernel/types.h>
#include <stdio.h>

void dos_read_partition(struct ata_device *dev) {
  struct dos_mbr mbr = {};

  ata_read28(dev, (u8 *)&mbr, 0, sizeof(struct dos_mbr));

  if (mbr.magic_number != 0xAA55) {
    printf("[DOS] Invalid MBR: %X\n", mbr.magic_number);
    return;
  }

  for (int i = 0; i < 4; i++) {
    if (mbr.partitions[i].partition_id == 0x00) {
      continue;
    }

    printf("[DOS] Partition %d:\n", i + 1);

    if (mbr.partitions[i].bootable == 0x80) {
      printf("  Bootable: Yes\n");
    } else {
      printf("  Bootable: No\n");
    }
  }
}