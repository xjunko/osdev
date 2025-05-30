// stupid pci that might work
#include <kernel/idt.h>
#include <kernel/pci.h>
#include <kernel/ports.h>
#include <kernel/types.h>
#include <stdio.h>

#define PCI_DATA_PORT 0xCFC
#define PCI_COMMAND_PORT 0xCF8

struct base_addr_register pci_get_base_address_register(u16 bus, u16 device,
                                                        u16 function, u16 bar) {
  struct base_addr_register res = {};

  u32 header_type = pci_read(bus, device, function, 0x0E) & 0x7F;
  int max_bars = 6 - (4 * header_type);

  if (bar >= max_bars) {
    return res;
  }

  u32 bar_value = pci_read(bus, device, function, 0x10 + 4 * bar);
  res.type = (bar_value & 0x1) ? input_output : memory;

  if (res.type == memory) {
    switch ((bar_value >> 1) & 0x3) {
      case 0:
      case 1:
      case 2:
        break;
    }

    res.prefetch = ((bar_value >> 3) & 0x1) == 0x1;
  } else {
    res.addr = (u8*)(bar_value & ~0x3);
    res.prefetch = false;
  }

  return res;
}

struct pci_desc pci_get_device_descriptor(u16 bus, u16 decice, u16 function) {
  struct pci_desc res = {};

  res.bus = bus;
  res.device = decice;
  res.func = function;

  res.vendor = pci_read(bus, decice, function, 0x00);
  res.dev = pci_read(bus, decice, function, 0x02);

  res.class = pci_read(bus, decice, function, 0x0B);
  res.subclass = pci_read(bus, decice, function, 0x0A);
  res.interface = pci_read(bus, decice, function, 0x09);

  res.rev = pci_read(bus, decice, function, 0x08);
  res.interrupt = pci_read(bus, decice, function, 0x3C);

  return res;
}

u32 pci_read(u16 bus, u16 dev, u16 func, u32 reg_offset) {
  u32 id = 0x1 << 31 | (bus & 0xFF) << 16 | ((dev & 0x1F) << 11) |
           ((func & 0x07) << 8) | ((reg_offset & 0xFC));

  outportl(PCI_COMMAND_PORT, id);
  return inportl(PCI_DATA_PORT) >> (8 * (reg_offset % 4));
}

void pci_write(u16 bus, u16 dev, u16 func, u32 reg_ofset, u32 data) {
  u32 id = 0x1 << 31 | (bus & 0xFF) << 16 | ((dev & 0x1F) << 11) |
           ((func & 0x07) << 8) | ((reg_ofset & 0xFC));

  outportl(PCI_COMMAND_PORT, id);
  outportl(PCI_DATA_PORT, data);
}

bool pci_device_has_function(u16 bus, u16 device) {
  return pci_read(bus, device, 0, 0x0E) & (1 << 7);
}

void pci_init() {
  for (int bus = 0; bus < 8; bus++) {
    for (int device = 0; device < 32; device++) {
      int num_functions = pci_device_has_function(bus, device) ? 8 : 1;
      for (int function = 0; function < num_functions; function++) {
        struct pci_desc dev = pci_get_device_descriptor(bus, device, function);

        if (dev.vendor == 0x0000 || dev.vendor == 0xFFFF) {
          continue;
        }

        for (int bar_num = 0; bar_num < 6; bar_num++) {
          struct base_addr_register bar =
              pci_get_base_address_register(bus, device, function, bar_num);

          if (bar.addr && bar.type == input_output) {
            dev.port_base = (u32)bar.addr;
          }
        }

        void* driver = pci_get_driver(dev);
        if (driver != 0) {
          // todo: drivers
        }

        printf(
            "[PCI] Bus: 0x%X | Function: 0x%X | Vendor: 0x%X | Device: 0x%X\n",
            bus & 0xFF, function & 0xFF, (dev.vendor & 0xFF00) >> 8,
            (dev.vendor & 0x00FF));
      }
    }
  }
}

void* pci_get_driver(struct pci_desc dev) {
  switch (dev.vendor) {
    case 0x8086:
      printf("[PCI] Intel device found\n");
      break;
  }

  switch (dev.class) {
    case 0x03:
      printf("[PCI] VGA device found\n");
      break;
    case 0x04:
      printf("[PCI] Multimedia device found\n");
      break;
  }
  return 0;
}