#include <kernel/mikufs.h>
#include <kernel/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* fopen(const char* filename, const char* mode) {
  if (strcmp(mode, "r") != 0) {
    printf("[stdio] Only read mode supported.\n");
    return 0;
  }

  if (!global_mikufs_device) {
    printf("[fopen] invalid global device");
    return 0;
  }

  struct miku_fs_file* file_data = mikufs_read(global_mikufs_device, filename);
  if (!file_data) return 0;

  FILE* f = malloc(sizeof(FILE));
  memcpy(file_data->filename, f->filename, 16);
  f->data = file_data->data;
  f->size = file_data->sector_size * 512;  // approximate size
  f->pos = file_data->sector_start;
  f->file = file_data;
  return f;
}