#ifndef __LOVE_INTERRUPTS_H
#define __LOVE_INTERRUPTS_H

#include <commons/types.h>
#include <hardware/port.h>
#include <memory/gdt.h>

class InterruptManager;

class InterruptHandler {
 protected:
  u8 interrupt_number;
  InterruptManager* interrupt_manager;

  InterruptHandler(u8 interrupt_number, InterruptManager* interrupt_manager);
  ~InterruptHandler();

 public:
  virtual u32 handle_interrupt(u32 esp);
};

class InterruptManager {
  friend class InterruptHandler;

 protected:
  static InterruptManager* active_interrupt_manager;
  InterruptHandler* handlers[256];

  struct GateDescriptor {
    u16 handler_address_low;
    u16 gdt_code_segment_selector;
    u8 reserved;
    u8 access;
    u16 handler_address_high;
  } __attribute__((packed));

  static GateDescriptor interrupt_descriptor_table[256];

  struct InterruptDescriptorTablePointer {
    u16 size;
    u32 base;
  } __attribute__((packed));

  static void set_interrupt_descriptor_table_entry(
      u8 interrupt_number, u16 code_segment_selector_offset, void (*handler)(),
      u8 privilege, u8 type);

  Port8BitSlow pic_master_command;
  Port8BitSlow pic_master_data;
  Port8BitSlow pic_slave_command;
  Port8BitSlow pic_slave_data;

 public:
  InterruptManager(Memory::GlobalDescriptorTable* gdt);
  ~InterruptManager();

  void activate();
  void deactivate();

  static u32 handle_interrupt(u8 interrupt_number, u32 esp);
  u32 do_handle_interrupt(u8 interrupt_number, u32 esp);

  static void ignore_interrupt_request();
  static void handle_interrupt_request_0x00();
  static void handle_interrupt_request_0x01();
  static void handle_interrupt_request_0x0C();
};

#endif  // __LOVE_INTERRUPTS_H