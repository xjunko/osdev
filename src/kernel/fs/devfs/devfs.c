#include <kernel/fs/devfs.h>
#include <kernel/fs/vfs.h>
#include <kernel/serial.h>
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

  printf("[devfs] Reading from device %s\n", path);
  size_t bytes_read = dev->read(path, buffer, size);
  return bytes_read;
}

size_t devfs_write(const char* path, const void* buffer, size_t size) {
  if (!path) return -1;
  if (!buffer) return -2;

  struct devfs_dev* dev = _devfs_find_dev(path);

  printf("[devfs] Attempting to write to device %s\n", path);

  if (!dev) return -3;
  if (!dev->write) return -4;

  printf("[devfs] Writing to device %s\n", path);
  size_t bytes_written = dev->write(path, buffer, size);
  return bytes_written;
}

size_t devfs_exists(const char* path) {
  if (!path) return 0;

  struct devfs_dev* dev = _devfs_find_dev(path);
  if (dev) {
    return 1;
  }

  return 0;
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

// /test
size_t _devfs_test_read(const char* path, void* buffer, size_t size) {
  memset(buffer, 1, size);
  return size;
}

// /com1
size_t _devfs_com1_read(const char* path, void* buffer, size_t size) {
  memset(buffer, 0, size);
  return size;
}

size_t _devfs_com1_write(const char* path, const void* buffer, size_t size) {
  for (size_t i = 0; i < size; i++) {
    serial_putchar(((const char*)buffer)[i]);
  }
  return size;
}

void devfs_init(struct vfs_impl* fs) {
  devfs_register_dev("/test", _devfs_test_read, NULL);
  devfs_register_dev("/com1", _devfs_com1_read, _devfs_com1_write);

  fs->read = devfs_read;
  fs->write = devfs_write;
  fs->exists = devfs_exists;
}