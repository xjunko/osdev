#include <kernel/devfs.h>
#include <kernel/types.h>

int vfs_sys_open(const char* path, int flags, int mode) {
  struct vfs_file* file = vfs_open(path, flags, mode);
  if (!file) {
    return -1;
  }
  return file->id;
}

int vfs_sys_read(int fd, char* buffer, size_t size) {
  struct vfs_file* file = vfs_get_file(fd);
  if (!file || !file->fs->impl || !file->fs->impl->read) {
    return -1;
  }
  return file->fs->impl->read(file->loc, buffer, size);
}

int vfs_sys_write(int fd, const char* buffer, size_t size) {
  struct vfs_file* file = vfs_get_file(fd);
  if (!file || !file->fs->impl || !file->fs->impl->write) {
    return -1;
  }

  return file->fs->impl->write(file->loc, buffer, size);
}

int vfs_sys_close(int fd) {
  struct vfs_file* file = vfs_get_file(fd);
  if (!file) {
    return -1;
  }

  vfs_close(file);
  return 0;
}