#pragma once
#include <kernel/types.h>

struct dos_partition {
  u8 bootable;
  u8 start_head;
  u8 start_sector : 6;
  u16 start_cylinder : 10;

  u8 partition_id;

  u8 end_head;
  u8 end_sector : 6;
  u16 end_cylinder : 10;

  u32 start_lba;
  u32 length;
} __attribute__((packed));

struct dos_mbr {
  u8 bootloader[440];
  u32 signature;
  u16 unused;

  struct dos_partition partitions[4];

  u16 magic_number;
} __attribute__((packed));

void dos_read_partition(struct ata_device *dev);