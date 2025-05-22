#pragma once
#include <kernel/idt.h>
#include <kernel/types.h>

enum base_addr_register_type { memory, input_output };

struct base_addr_register {
  bool prefetch;
  u8* addr;
  u32 size;
  enum base_addr_register_type type;
};

struct base_addr_register pci_get_base_address_register(u16, u16, u16, u16);

struct pci_desc {
  u32 port_base;
  u32 interrupt;

  u16 bus;
  u16 device;
  u16 func;

  u16 vendor;
  u16 dev;

  u8 class;
  u8 subclass;
  u8 interface;

  u8 rev;
};

struct pci_desc pci_get_device_descriptor(u16, u16, u16);
bool pci_device_has_function(u16, u16);
void pci_select_drivers(struct interrupt_manager*);

u32 pci_read(u16, u16, u16, u32);
void pci_write(u16, u16, u16, u32, u32);

void* pci_get_driver(struct pci_desc, struct interrupt_manager*);