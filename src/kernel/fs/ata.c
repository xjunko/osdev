// ata.c - super simple ata driver
#include <kernel/ata.h>
#include <kernel/ports.h>
#include <kernel/types.h>
#include <stdio.h>
#include <stdlib.h>

// data port is 16bit else is 8bit
#define DATA_PORT 0x0
#define ERROR_PORT 0x1
#define SECTOR_COUNT_PORT 0x2
#define LBA_LOW_PORT 0x3
#define LBA_MID_PORT 0x4
#define LBA_HIGH_PORT 0x5
#define DEVICE_PORT 0x6
#define COMMAND_PORT 0x7
#define CONTROL_PORT 0x206

#define BSY 0x80
#define DRQ 0x08
#define ERR 0x01

struct ata_device* ata_new(bool is_master, u16 base) {
  struct ata_device* dev = malloc(sizeof(struct ata_device));
  dev->base = base;
  dev->master = is_master;
  return dev;
}

void ata_identify(struct ata_device* dev) {
  // device info
  u8 lba_mid = inportb(dev->base + LBA_MID_PORT);
  u8 lba_high = inportb(dev->base + LBA_HIGH_PORT);

  if (lba_mid == 0x00 && lba_high == 0x00) {
    dev->type = dev_ata;
    printf("[ATA] Device is ATA\n");
  } else if (lba_mid == 0x14 && lba_high == 0xEB) {
    dev->type = dev_atapi;
    printf("[ATA] Device is ATAPI\n");
  } else if (lba_mid == 0x3C && lba_high == 0xC3) {
    dev->type = dev_sata;
    printf("[ATA] Device is SATA (non-PATA)\n");
  } else {
    dev->type = dev_unknown;
    printf("[ATA] Unknown device type: LBA Mid = %x, High = %x\n", lba_mid,
           lba_high);
    while (1);
  }

  ata_delay(dev);
  ata_delay(dev);
  ata_delay(dev);
  ata_delay(dev);

  // ident starts here
  outportb(dev->base + DEVICE_PORT, dev->master ? 0xA0 : 0xB0);
  outportb(dev->base + CONTROL_PORT, 0x0);

  outportb(dev->base + DEVICE_PORT, 0xA0);
  volatile u8 status = inportb(dev->base + COMMAND_PORT);
  if (status == 0xFF) {
    return;  // not master
  }

  outportb(dev->base + DEVICE_PORT, dev->master ? 0xA0 : 0xB0);
  outportb(dev->base + SECTOR_COUNT_PORT, 0x0);
  outportb(dev->base + LBA_LOW_PORT, 0x0);
  outportb(dev->base + LBA_MID_PORT, 0x0);
  outportb(dev->base + LBA_HIGH_PORT, 0x0);
  outportb(dev->base + COMMAND_PORT, 0xEC);  // IDENT

  status = inportb(dev->base + COMMAND_PORT);
  if (status == 0x00) {
    printf("[ATA] Device not ready\n");
    while (1);
    return;
  }

  while (((status & 0x80) == 0x80) && ((status & 0x01) != 0x01))
    status = inportb(dev->base + COMMAND_PORT);

  if (status & 0x01) {
    printf("[ATA] Error reading device\n");
    return;
  }

  u16 identify_data[256];
  for (int i = 0; i < 256; i++) {
    identify_data[i] = inportw(dev->base + DATA_PORT);
  }

  // // get model name
  for (int i = 0; i < 20; i++) {
    dev->model[i * 2] = (identify_data[27 + i] >> 8) & 0xFF;
    dev->model[i * 2 + 1] = identify_data[27 + i] & 0xFF;
  }
  dev->model[40] = '\0';
  printf("[ATA] Model: %s\n", dev->model);

  ata_delay(dev);
}

void ata_read28(struct ata_device* dev, u8* out_buffer, u32 sector_num,
                i32 size) {
  if (sector_num > 0x0FFFFFFF) return;

  outportb(dev->base + DEVICE_PORT,
           (dev->master ? 0xE0 : 0xF0) | ((sector_num & 0x0F000000) >> 24));
  outportb(dev->base + ERROR_PORT, 0x0);
  outportb(dev->base + SECTOR_COUNT_PORT, 0x1);
  outportb(dev->base + LBA_LOW_PORT, (sector_num & 0xFF));
  outportb(dev->base + LBA_MID_PORT, (sector_num & 0x0000FF00) >> 8);
  outportb(dev->base + LBA_HIGH_PORT, (sector_num & 0x00FF0000) >> 16);
  outportb(dev->base + COMMAND_PORT, 0x20);  // READ

  printf("[ATA] Reading sector %d size=%d\n", sector_num, size);
  ata_poll(dev);
  for (int i = 0; i < size; i += 2) {
    u16 cur_data = inportw(dev->base + DATA_PORT);
    *(u16*)(out_buffer + i) = cur_data;
  }
  for (int i = size + (size % 2); i < 512; i += 2) {
    inportw(dev->base + DATA_PORT);  // leftovers
  }

  ata_delay(dev);
  ata_delay(dev);
  ata_delay(dev);
  ata_delay(dev);

  printf("[ATA] Read %d bytes from sector %d\n", size, sector_num);
}

void ata_write28(struct ata_device* dev, u8* data, u32 sector_num, i32 size) {
  if (sector_num > 0x0FFFFFFF) return;
  if (size > 512) {
    printf("[ATA] Write size exceeds 512 bytes\n");
    return;
  }

  outportb(dev->base + DEVICE_PORT,
           (dev->master ? 0xE0 : 0xF0) | ((sector_num & 0x0F000000) >> 24));
  outportb(dev->base + ERROR_PORT, 0x0);
  outportb(dev->base + SECTOR_COUNT_PORT, 0x1);
  outportb(dev->base + LBA_LOW_PORT, (sector_num & 0x000000FF));
  outportb(dev->base + LBA_MID_PORT, (sector_num & 0x0000FF00) >> 8);
  outportb(dev->base + LBA_HIGH_PORT, (sector_num & 0x00FF0000) >> 16);
  outportb(dev->base + COMMAND_PORT, 0x30);  // WRITE

  printf("[ATA] Writing sector %d size=%d\n", sector_num, size);
  ata_poll(dev);
  for (int i = 0; i < size; i += 2) {
    u16 c_data = data[i];
    if (i + 1 < size) {
      c_data |= ((u16)data[i + 1]) << 8;
    }
    outportw(dev->base + DATA_PORT, c_data);

    printf("[ATA] Writing byte %d: %d %d\n", i, c_data & 0xFF,
           (c_data >> 8) & 0xFF);
  }
  for (int i = size + (size % 2); i < 512; i += 2) {
    outportw(dev->base + DATA_PORT, 0x0);
  }

  ata_delay(dev);
  ata_delay(dev);
  ata_delay(dev);
  ata_delay(dev);

  printf("[ATA] Wrote %d bytes to sector %d\n", size, sector_num);
}

void ata_flush(struct ata_device* dev) {
  // noop, unused.
}

void ata_poll(struct ata_device* dev) {
  // wait for bsy to clear
  for (int i = 0; i < 1024; i++) {
    u8 status = inportb(dev->base + COMMAND_PORT);
    if (!(status & BSY)) break;
  }

  // read status and check for err
  u8 status = inportb(dev->base + COMMAND_PORT);
  if (status & ERR) {
    printf("[ATA] Error during polling\n");
    return;
  }

  // ensure drq is set
  if (!(status & DRQ)) {
    printf("[ATA] Device not ready for data transfer\n");
    return;
  }
}

void ata_delay(struct ata_device* dev) {
  for (int i = 0; i < 4; i++) {
    inportb(dev->base + CONTROL_PORT);
  }
}