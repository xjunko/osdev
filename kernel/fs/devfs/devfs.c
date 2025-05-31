#include <kernel/devfs.h>
#include <kernel/ps2hid.h>
#include <kernel/serial.h>
#include <kernel/types.h>
#include <kernel/vfs.h>
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
#ifdef KERNEL_DEBUG
  printf("[devfs] Reading from device %s\n", path);
#endif
  size_t bytes_read = dev->read(path, buffer, size);
  return bytes_read;
}

size_t devfs_write(const char* path, const void* buffer, size_t size) {
  if (!path) return -1;
  if (!buffer) return -2;

  struct devfs_dev* dev = _devfs_find_dev(path);

  if (!dev) return -3;
  if (!dev->write) return -4;
#ifdef KERNEL_DEBUG
  printf("[devfs] Writing to device %s\n", path);
#endif
  size_t bytes_written = dev->write(path, buffer, size);
  return bytes_written;
}

bool devfs_exists(const char* path) {
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

// /null
size_t _devfs_null_read(const char* path, void* buffer, size_t size) {
  UNUSED(path);
  UNUSED(buffer);
  UNUSED(size);
  return 0;
}
size_t _devfs_null_write(const char* path, const void* buffer, size_t size) {
  UNUSED(path);
  UNUSED(buffer);
  return size;
}

// /zero
size_t _devfs_zero_read(const char* path, void* buffer, size_t size) {
  UNUSED(path);
  memset(buffer, 0, size);
  return size;
}
size_t _devfs_zero_write(const char* path, const void* buffer, size_t size) {
  UNUSED(path);
  UNUSED(buffer);
  return size;
}

// /stdout
size_t _devfs_stdout_read(const char* path, void* buffer, size_t size) {
  UNUSED(path);
  UNUSED(buffer);
  return size;
}

size_t _devfs_stdout_write(const char* path, const void* buffer, size_t size) {
  UNUSED(path);

  for (size_t i = 0; i < size; i++) {
    serial_putchar(((const char*)buffer)[i]);
  }
  return size;
}

// /keyboard
size_t _devfs_keyboard_read(const char* path, void* buffer, size_t size) {
  UNUSED(path);
  UNUSED(size);

  char* buf = (char*)buffer;
  buf[0] = 'n';
  buf[1] = 'o';
  buf[2] = 't';
  buf[3] = '_';
  buf[4] = 'i';
  buf[5] = 'm';
  buf[6] = 'p';
  buf[7] = 'l';
  buf[8] = 'e';
  buf[9] = 'm';
  buf[10] = 'e';
  buf[11] = 'n';
  buf[12] = 't';
  buf[13] = 'e';
  buf[14] = 'd';
  buf[15] = '\n';

  return strlen(buf) + 1;
}

size_t _devfs_keyboard_write(const char* path, const void* buffer,
                             size_t size) {
  UNUSED(path);
  UNUSED(buffer);
  UNUSED(size);
  return 0;
}

void devfs_init(struct vfs_impl* fs) {
  devfs_register_dev("/null", _devfs_null_read, _devfs_null_write);
  devfs_register_dev("/zero", _devfs_zero_read, _devfs_zero_write);
  devfs_register_dev("/stdout", _devfs_stdout_read, _devfs_stdout_write);
  devfs_register_dev("/keyboard", _devfs_keyboard_read, _devfs_keyboard_write);

  fs->read = devfs_read;
  fs->write = devfs_write;
  fs->exists = devfs_exists;
}