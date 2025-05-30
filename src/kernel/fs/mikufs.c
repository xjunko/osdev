// mikufs.c - a very simple linear fs
/*
    mikuFS
    - files only
    - no folder
    - get bent

    entry:
        u8  filename[16];
        u32 size;

    raw_data:
        whatevs

    structure:
        entry
        raw_data
        entry
        raw_data
*/

#include <kernel/ata.h>
#include <kernel/fs/mikufs.h>
#include <kernel/types.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static u32 miku_sector_ends_at = 0;
struct ata_device* global_mikufs_device;

void mikufs_init(struct ata_device* dev) {
  int cur_sector = 0;

  while (1) {
    struct miku_fs_entry entry;
    ata_read28(dev, (u8*)&entry, cur_sector, sizeof(struct miku_fs_entry));
    if (!entry.valid) break;

    cur_sector = entry.sector_start;       // Current
    cur_sector += entry.sector_size;       // Size
    miku_sector_ends_at = cur_sector + 1;  // Offset
  }

  printf("[MikuFS] Sector ends at %d \n", miku_sector_ends_at);

  global_mikufs_device = dev;
}

void mikufs_write(struct ata_device* dev, const char* filename, u8* data,
                  i32 size) {
  struct miku_fs_entry new_entry = {};
  new_entry.valid = true;

  for (int i = 0; i < 16; i++) {
    if (filename[i] == '\0') {
      break;
    }
    new_entry.filename[i] = filename[i];
  }

  new_entry.sector_start = miku_sector_ends_at + 1;
  new_entry.sector_size = (int)roundf((((f32)size + (f32)511.0) / (f32)512.0));

  if (new_entry.sector_size <= 0) {
    new_entry.sector_size = 1;
  }

  printf("[MikuFS] SectorCount: %d | StartAt: %d | Name: %s \n",
         new_entry.sector_size, new_entry.sector_start, new_entry.filename);

  ata_write28(dev, (u8*)&new_entry, new_entry.sector_start - 1,
              sizeof(struct miku_fs_entry));

  for (int i = 0; i < new_entry.sector_size; i++) {
    if (size <= 512) {
      ata_write28(dev, data + (i * 512), new_entry.sector_start + i, size);
    } else {
      size -= 512;
      ata_write28(dev, data + (i * 512), new_entry.sector_start + i, 512);
    }
  }

  miku_sector_ends_at = new_entry.sector_start + new_entry.sector_size;
}

struct miku_fs_file* mikufs_read(struct ata_device* dev, const char* filename) {
  int cur_sector = 0;

  while (1) {
    struct miku_fs_entry entry;
    ata_read28(dev, (u8*)&entry, cur_sector, sizeof(struct miku_fs_entry));
    if (!entry.valid) break;

    // Found filename
    if (strcmp(filename, (const char*)entry.filename) == 0) {
      // Read the data
      u8* data = malloc(entry.sector_size * 512);
      if (!data) {
        printf("[MikuFS] Failed to malloc!");
        while (1);
      }

      for (int i = 0; i < entry.sector_size; i++) {
        ata_read28(dev, data + (i * 512), entry.sector_start + i, 512);
      }

      printf("[MikuFS] Read file '%s' (%d sectors)\n", entry.filename,
             entry.sector_size);

      struct miku_fs_file* cur_file = malloc(sizeof(struct miku_fs_file));
      memcpy(cur_file->filename, entry.filename, 16);
      cur_file->sector_start = entry.sector_start;
      cur_file->sector_size = entry.sector_size;
      cur_file->data = data;
      return cur_file;
    }
    cur_sector += entry.sector_start + entry.sector_size;  // Skip this sector.
  }

  return 0;
}