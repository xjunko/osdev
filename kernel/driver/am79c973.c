#include <kernel/pci.h>
#include <kernel/ports.h>

struct _initblock {
  uint16_t mode;
  unsigned reserved1 : 4;
  unsigned num_send_buffers : 4;
  unsigned reserved2 : 4;
  unsigned num_recv_buffers : 4;
  uint64_t physical_addr : 48;
  uint16_t reserved3;
  uint64_t logical_addr;
  uint32_t recv_buffer_desc_addr;
  uint32_t send_buffer_desc_addr;
} __attribute__((packed));

struct driver_amd79c973 {
  uint32_t mac_addr0;
  uint32_t mac_addr2;
  uint32_t mac_addr4;

  uint32_t reg_data;
  uint32_t reg_address;
  uint32_t reset;
  uint32_t bus_control;

  struct _initblock init_block;
};

struct driver_amd79c973 new_amd79c973(struct pci_desc dev) {
  struct driver_amd79c973 amd;

  amd.mac_addr0 = dev.port_base + 0x00;
  amd.mac_addr2 = dev.port_base + 0x02;
  amd.mac_addr4 = dev.port_base + 0x04;
  amd.reg_data = dev.port_base + 0x10;
  amd.reg_address = dev.port_base + 0x12;
  amd.reset = dev.port_base + 0x14;
  amd.bus_control = dev.port_base + 0x16;

  uint64_t mac0 = inportw(amd.mac_addr0) % 256;
  uint64_t mac1 = inportw(amd.mac_addr0) / 256;
  uint64_t mac2 = inportw(amd.mac_addr2) % 256;
  uint64_t mac3 = inportw(amd.mac_addr2) / 256;
  uint64_t mac4 = inportw(amd.mac_addr4) % 256;
  uint64_t mac5 = inportw(amd.mac_addr4) / 256;

  uint64_t mac =
      mac5 << 40 | mac4 << 32 | mac3 << 24 | mac2 << 16 | mac1 << 8 | mac0;

  // 32bit mode
  outportw(amd.reg_address, 20);
  outportb(amd.bus_control, 0x102);

  // stop
  outportb(amd.reg_address, 0);
  outportb(amd.reg_data, 0x04);

  // init block
  amd.init_block.mode = 0x0000;
  amd.init_block.reserved1 = 0;
  amd.init_block.num_send_buffers = 3;
  amd.init_block.reserved2 = 0;
  amd.init_block.num_recv_buffers = 3;
  amd.init_block.physical_addr = mac;
  amd.init_block.reserved3 = 0;
  amd.init_block.logical_addr = 0;
}