#pragma once
#include <kernel/types.h>

struct miku_fs_entry {
  bool valid;
  u8 filename[16];
  u8 sector_start;
  u8 sector_size;
} __attribute__((packed));

void mikufs_init(struct ata_device*);
void mikufs_write(struct ata_device*, const char*, u8*, i32);
u8* mikufs_read(struct ata_device*, const char*);