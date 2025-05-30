#include <kernel/types.h>
#include <stddef.h>
#include <stdio.h>

typedef ssize_t (*read_type_t)(struct fs_node*, long, size_t, u8*);

typedef struct fs_node {
  char name[256];
  void* dev;
  int mask;
  int uid;
  int gid;
  u64 flags;
  u64 inode;
  u64 length;
  u64 impl;
  u64 open_flags;

  long atime;
  long mtime;
  long ctime;

} fs_node_t;

struct vfs_entry {
  char* name;
  fs_node_t* file;
  char* device;
  char* fs_type;
};

vfs_open(const char* filename, u32 flags) {}