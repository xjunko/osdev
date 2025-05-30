#pragma once
#include <kernel/types.h>

typedef int (*fs_read)(const char* path, void* buffer, size_t size);
typedef int (*fs_write)(const char* path, const void* buffer, size_t size);

struct vfs_impl {
  fs_read read;
  fs_write write;
};

struct vfs_file {
  const char* name;
  char* content;
  size_t size;

  u64 ctime;
  u64 mtime;
  u64 atime;
};

struct vfs_file_desc {
  const char* path;
};

struct vfs {
  const char* mnt;
  struct vfs_impl* impl;
};

void vfs_init();
int vfs_open_file(const char* path);
int vfs_close_file(int);
struct vfs_file_desc* vfs_get_file(int);
int vfs_read(const char*, char*, size_t);
int vfs_write(const char*, const char*, size_t);