#pragma once
#include <kernel/types.h>

enum ata_dev_type {
  dev_ata,
  dev_atapi,
  dev_sata,
  dev_unknown,
};

struct ata_device {
  u16 base;
  bool master;

  u8 model[41];
  enum ata_dev_type type;
};

struct ata_device* ata_new(bool, u16);
void ata_identify(struct ata_device*);
void ata_read28(struct ata_device*, u8*, u32, i32);
void ata_write28(struct ata_device*, u8*, u32, i32);
void ata_flush(struct ata_device*);

void ata_poll(struct ata_device* dev);
void ata_delay(struct ata_device*);