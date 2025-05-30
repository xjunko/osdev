#include <kernel/fs/vfs.h>
#include <kernel/types.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define VFS_MAX_MOUNTPOINTS 16
static struct vfs mountpoints[VFS_MAX_MOUNTPOINTS];

#define VFS_MAX_FILE_DESC 64
static struct vfs_file_desc file_descs[VFS_MAX_FILE_DESC];

bool _startswith(const char* str, const char* prefix) {
  size_t prefix_len = strlen(prefix);
  return strncmp(str, prefix, prefix_len) == 0;
}

struct vfs* _vfs_find_mnt(const char* path) {
  if (!path) return 0;

  struct vfs* mnt = 0;

  for (u16 i = 0; i < VFS_MAX_MOUNTPOINTS; i++) {
    if (mountpoints[i].mnt) {
      if (_startswith(path, mountpoints[i].mnt)) {
        return &mountpoints[i];
      }
    }
  }

  return 0;
}

int vfs_read(const char* path, char* buffer, size_t size) {
  if (!path) return -1;

  struct vfs* mnt = _vfs_find_mnt(path);
  if (!mnt) return -1;
  if (!mnt->impl->read) return 0;

  const char* path_without_mnt = path + strlen(mnt->mnt) - 1;
  return mnt->impl->read(path_without_mnt, buffer, size);
}

int vfs_open_file(const char* path) {
  if (!path) return -1;

  for (u16 i = 3; i < VFS_MAX_FILE_DESC; i++) {
    if (file_descs[i].path == 0) {
      file_descs[i].path = path;
      return i;
    }
  }

  return -1;
}

int vfs_close_file(int fd) {
  if (fd <= 2) return -1;

  for (u16 i = 3; i < VFS_MAX_FILE_DESC; i++) {
    if (file_descs[i].path != 0) {
      file_descs[i].path = 0;
      return 0;
    }
  }

  return -1;
}

struct vfs_file_desc* vfs_get_file(int fd) {
  if (fd <= 2) return 0;

  for (u16 i = 3; i < VFS_MAX_FILE_DESC; i++) {
    if (file_descs[i].path != 0 && i == fd) {
      return &file_descs[i];
    }
  }

  return 0;
}

void vfs_init() {
  struct vfs_impl* devfs = malloc(sizeof(struct vfs_impl));
  mountpoints[0].mnt = "/dev/";
  mountpoints[0].impl = devfs;
  devfs_init(devfs);
}