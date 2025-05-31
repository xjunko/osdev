#pragma once
#include <kernel/idt.h>
#include <kernel/types.h>

enum base_addr_register_type { memory, input_output };

struct base_addr_register {
  bool prefetch;
  uint8_t* addr;
  uint32_t size;
  enum base_addr_register_type type;
};

struct base_addr_register pci_get_base_address_register(uint16_t, uint16_t,
                                                        uint16_t, uint16_t);

struct pci_desc {
  uint32_t port_base;
  uint32_t interrupt;

  uint16_t bus;
  uint16_t device;
  uint16_t func;

  uint16_t vendor;
  uint16_t dev;

  uint8_t class;
  uint8_t subclass;
  uint8_t interface;

  uint8_t rev;
};

struct pci_desc pci_get_device_descriptor(uint16_t, uint16_t, uint16_t);
bool pci_device_has_function(uint16_t, uint16_t);
void pci_init();

uint32_t pci_read(uint16_t, uint16_t, uint16_t, uint32_t);
void pci_write(uint16_t, uint16_t, uint16_t, uint32_t, uint32_t);

void* pci_get_driver(struct pci_desc);