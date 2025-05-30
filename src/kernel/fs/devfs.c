#include <kernel/fs/devfs.h>
#include <kernel/fs/vfs.h>
#include <kernel/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct devfs_dev* head_dev;
static struct devfs_dev* tail_dev;

struct devfs_dev* _devfs_find_dev(const char* path) {
  struct devfs_dev* current = head_dev;

  while (current) {
    if (strcmp(current->loc, path) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

size_t devfs_read(const char* path, void* buffer, size_t size) {
  if (!path) return 1;
  if (!buffer) return 2;

  struct devfs_dev* dev = _devfs_find_dev(path);

  if (!dev) return 3;
  if (!dev->read) return 4;

  size_t bytes_read = dev->read(path, buffer, size);
  return bytes_read;
}

size_t devfs_write(const char* path, const void* buffer, size_t size) {
  // Implement device-specific write logic here
  return size;  // Placeholder return value
}

void devfs_register_dev(const char* loc, fs_read read, fs_write write) {
  struct devfs_dev* dev = (struct devfs_dev*)malloc(sizeof(struct devfs_dev));
  if (!dev) {
    printf("[devfs] Failed to allocate memory for device %s\n", loc);
    return;
  }

  dev->loc = loc;
  dev->read = read;
  dev->write = write;
  dev->next = NULL;

  if (!head_dev) {
    head_dev = dev;
    tail_dev = dev;
  } else {
    tail_dev->next = dev;
    tail_dev = dev;
  }
}

size_t _devfs_test_read(const char* path, void* buffer, size_t size) {
  memset(buffer, 1, size);
  return size;
}

void devfs_init(struct vfs_impl* fs) {
  devfs_register_dev("/test", _devfs_test_read, NULL);

  fs->read = devfs_read;
  fs->write = devfs_write;
}