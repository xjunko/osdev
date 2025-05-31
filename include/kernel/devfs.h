#pragma once
#include <kernel/types.h>
#include <kernel/vfs.h>

struct devfs_dev {
  const char* loc;
  struct devfs_dev* next;

  fs_read read;
  fs_write write;
};

void devfs_register_dev(const char* loc, fs_read read, fs_write write);

void devfs_init(struct vfs_impl*);
size_t devfs_read(const char* path, void* buffer, size_t size);
size_t devfs_write(const char* path, const void* buffer, size_t size);
size_t devfs_exists(const char* path);