// fat.c - simple fat implementation
#include <kernel/types.h>

struct fat_bios_param_block32 {
  // fat12/16/32
  u8 jump[3];
  u8 softname[3];
  u16 bytes_per_sector;
  u8 sector_per_cluster;
  u16 reserved_sectors;
  u8 fat_copies;
  u16 root_dir_entries;
  u16 total_sectors;
  u8 media_descriptor;
  u16 sectors_per_fat;
  u16 sectors_per_track;
  u16 heads;
  u32 hidden_sectors;
  u32 total_sectors_32;

  // fat32 extended params
  u32 table_size;
  u16 ext_flags;
  u16 fat_version;
  u32 root_cluster;
  u16 fat_info;
  u16 backup_boot_sector;
  u8 reserved[12];
  u8 drive_number;
  u8 reserved1;
  u8 boot_signature;
  u32 volume_id;
  u8 volume_label[11];
  u8 file_system_type[8];
} __attribute__((packed));

struct fat_dir_entry32 {
  u8 name[8];
  u8 ext[3];
  u8 attr;
  u8 reserved;
  u8 create_time_tenth;
  u16 create_time;
  u16 create_date;
  u16 last_access_date;
  u16 first_cluster_high;
  u16 last_write_time;
  u16 last_write_date;
  u16 first_cluster_low;
  u32 file_size;
} __attribute__((packed));