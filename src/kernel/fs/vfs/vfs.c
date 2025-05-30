#include <kernel/fs/vfs.h>
#include <kernel/types.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static struct vfs mountpoints[VFS_MAX_MOUNTPOINTS];
static struct vfs_file* fds[VFS_MAX_FDS];

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

int _vfs_get_fd(struct vfs_file* file) {
  if (!file) return -1;

  for (u16 i = VFS_FD_OFFSET; i < VFS_MAX_FDS; i++) {
    if (!fds[i]) {
      fds[i] = file;
      file->id = i;
      return i;
    }
  }

  return -1;  // No free file descriptor found
}

int vfs_read(struct vfs_file* file, char* buffer, size_t size) {
  if (!file) return -1;
  if (!file->loc) return -2;
  if (!file->fs->mnt) return -3;
  if (!file->fs->impl) return -4;
  if (!file->fs->impl->read) return 0;

  printf("[vfs] Reading from file %s\n", file->loc);

  const char* loc_without_mnt = file->loc + strlen(file->fs->mnt) - 1;
  return file->fs->impl->read(loc_without_mnt, buffer, size);
}

int vfs_write(struct vfs_file* file, char* buffer, size_t size) {
  if (!file) return -1;
  if (!file->loc) return -2;
  if (!file->fs->mnt) return -3;
  if (!file->fs->impl) return -4;
  if (!file->fs->impl->write) return 0;

  printf("[vfs] Attempting to write to file %s\n", file->loc);

  const char* loc_without_mnt = file->loc + strlen(file->fs->mnt) - 1;
  return file->fs->impl->write(loc_without_mnt, buffer, size);
}

struct vfs_file* vfs_open(const char* path, int flags, int mode) {
  if (!path) return 0;

  struct vfs* mnt = _vfs_find_mnt(path);
  if (!mnt) return 0;
  if (!mnt->impl) return 0;
  if (!mnt->impl->exists) return 0;

  const char* loc_without_mnt = path + strlen(mnt->mnt) - 1;
  struct vfs_file* file = malloc(sizeof(struct vfs_file));
  if (!file) return 0;
  if (mnt->impl->exists(loc_without_mnt) == 0) {
    free(file);
    return 0;
  }

  file->id = _vfs_get_fd(file);
  file->loc = loc_without_mnt;
  file->flags = flags;
  file->mode = mode;
  file->fs = mnt;

  printf("[vfs] Opened file %s with flags %d, mode %d and id %d\n", path, flags,
         mode, file->id);

  return file;
}

int vfs_exists(struct vfs_file* file) {
  if (!file) return -1;
  if (!file->loc) return -2;
  if (!file->fs) return -3;

  return file->fs->impl->exists(file->loc) ? 1 : 0;
}

struct vfs_file* vfs_get_file(int fd) {
  if (fd < 0 || fd >= VFS_MAX_FDS || !fds[fd]) {
    return 0;
  }
  return fds[fd];
}

int vfs_close(struct vfs_file* file) {
  if (!file) return -1;
  if (file->id < 0 || file->id >= VFS_MAX_FDS || !fds[file->id]) {
    return -2;
  }

  fds[file->id] = 0;
  free(file);
  return 0;
}

void vfs_init() {
  struct vfs_impl* devfs = malloc(sizeof(struct vfs_impl));
  mountpoints[0].mnt = "/dev/";
  mountpoints[0].impl = devfs;
  devfs_init(devfs);
}