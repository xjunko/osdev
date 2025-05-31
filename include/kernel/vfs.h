#pragma once
#include <kernel/types.h>

#define VFS_MAX_MOUNTPOINTS 16
#define VFS_FD_OFFSET 10
#define VFS_MAX_FDS 128

typedef int (*fs_read)(const char* path, void* buffer, size_t size);
typedef int (*fs_write)(const char* path, const void* buffer, size_t size);
typedef bool (*fs_exists)(const char* path);

struct vfs_impl {
  fs_read read;
  fs_write write;
  fs_exists exists;
};

struct vfs_file {
  int id;
  const char* loc;

  int flags;
  int mode;

  struct vfs* fs;
};

struct vfs {
  const char* mnt;
  struct vfs_impl* impl;
};

// kernel-api
void vfs_init();
struct vfs_file* vfs_open(const char*, int, int);
struct vfs_file* vfs_get_file(int fd);
int vfs_close(struct vfs_file*);
int vfs_read(struct vfs_file*, char*, size_t);
int vfs_write(struct vfs_file*, char*, size_t);
int vfs_exists(struct vfs_file*);

// syscall-api
int vfs_sys_open(const char* path, int flags, int mode);
int vfs_sys_read(int fd, char* buffer, size_t size);
int vfs_sys_write(int fd, const char* buffer, size_t size);
int vfs_sys_close(int fd);