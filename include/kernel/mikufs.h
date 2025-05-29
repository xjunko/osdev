#pragma once
#include <kernel/ata.h>
#include <kernel/types.h>

struct miku_fs_entry {
  bool valid;
  u8 filename[16];
  u8 sector_start;
  u8 sector_size;
} __attribute__((packed));

struct miku_fs_file {
  u8 filename[16];
  u8 sector_start;
  u8 sector_size;
  u8* data;
} __attribute__((packed));

extern struct ata_device* global_mikufs_device;

void mikufs_init(struct ata_device*);
void mikufs_write(struct ata_device*, const char*, u8*, i32);
struct miku_fs_file* mikufs_read(struct ata_device*, const char*);