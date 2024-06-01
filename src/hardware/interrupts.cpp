#include <commons/types.h>
#include <hardware/interrupts.h>
#include <hardware/port.h>
#include <terminal/term.h>

InterruptManager::GateDescriptor
    InterruptManager::interrupt_descriptor_table[256];

void InterruptManager::set_interrupt_descriptor_table_entry(
    u8 interrupt_number, u16 code_segment_selector_offset, void (*handler)(),
    u8 privilege, u8 type) {
  interrupt_descriptor_table[interrupt_number].handler_address_low =
      ((u32)handler) & 0xFFFF;
  interrupt_descriptor_table[interrupt_number].handler_address_high =
      (((u32)handler) >> 16) & 0xFFFF;
  interrupt_descriptor_table[interrupt_number].gdt_code_segment_selector =
      code_segment_selector_offset;
  interrupt_descriptor_table[interrupt_number].access =
      0x80 | ((privilege & 3) << 5) | type;
  interrupt_descriptor_table[interrupt_number].reserved = 0;
}

InterruptManager::InterruptManager(Memory::GlobalDescriptorTable* gdt)
    : pic_master_command(0x20),
      pic_master_data(0x21),
      pic_slave_command(0xA0),
      pic_slave_data(0xA1) {
  u32 code_segment = gdt->CodeSegmentSelector();
  const u8 IDT_INTERRUPT_GATE = 0xE;

  for (u16 i = 0; i < 256; i++) {
    set_interrupt_descriptor_table_entry(
        i, code_segment, &ignore_interrupt_request, 0, IDT_INTERRUPT_GATE);
  }

  set_interrupt_descriptor_table_entry(0x20, code_segment,
                                       &handle_interrupt_request_0x00, 0,
                                       IDT_INTERRUPT_GATE);
  set_interrupt_descriptor_table_entry(0x21, code_segment,
                                       &handle_interrupt_request_0x01, 0,
                                       IDT_INTERRUPT_GATE);

  pic_master_command.Write(0x11);
  pic_slave_command.Write(0x11);

  pic_master_data.Write(0x20);
  pic_slave_data.Write(0x28);

  pic_master_data.Write(0x04);
  pic_slave_data.Write(0x02);

  pic_master_data.Write(0x01);
  pic_slave_data.Write(0x01);

  pic_master_data.Write(0x00);
  pic_slave_data.Write(0x00);

  InterruptDescriptorTablePointer idt;
  idt.size = 256 * sizeof(GateDescriptor) - 1;
  idt.base = (u32)interrupt_descriptor_table;

  asm volatile("lidt %0" : : "m"(idt));
}

void InterruptManager::activate() { asm("sti"); }

u32 InterruptManager::handle_interrupt(u8 interrupt_number, u32 esp) {
  Terminal::printf("\n --> INTERRUPT ! \n");
  return esp;
}