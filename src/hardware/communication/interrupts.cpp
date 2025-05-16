#include <commons/types.h>
#include <commons/utility.h>
#include <hardware/communication/interrupts.h>
#include <hardware/communication/port.h>
#include <hardware/communication/serial/serial.h>
#include <terminal/term.h>

using namespace RinOS::Hardware::Communication;
using namespace RinOS::Hardware::Communication::Serial;

// InterruptHandler
InterruptHandler::InterruptHandler(u8 interrupt_number,
                                   InterruptManager* interrupt_manager) {
  this->interrupt_number = interrupt_number;
  this->interrupt_manager = interrupt_manager;
  interrupt_manager->handlers[interrupt_number] = this;
}

InterruptHandler::~InterruptHandler() {
  if (interrupt_manager->handlers[interrupt_number] == this) {
    interrupt_manager->handlers[interrupt_number] = 0;
  }
}
u32 InterruptHandler::handle_interrupt(u32 esp) { return esp; }

// InterruptManager
InterruptManager::GateDescriptor
    InterruptManager::interrupt_descriptor_table[256];

InterruptManager* InterruptManager::active_interrupt_manager = 0;

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

InterruptManager::InterruptManager(RinOS::Memory::GlobalDescriptorTable* gdt)
    : pic_master_command(0x20),
      pic_master_data(0x21),
      pic_slave_command(0xA0),
      pic_slave_data(0xA1) {
  COM1.Print("[System] InterruptManager Initialized!");
  u32 code_segment = gdt->CodeSegmentSelector();

  const u8 IDT_INTERRUPT_GATE = 0xE;

  for (u8 i = 255; i > 0; i--) {
    handlers[i] = 0;
    set_interrupt_descriptor_table_entry(
        i, code_segment, &ignore_interrupt_request, 0, IDT_INTERRUPT_GATE);
  }

  set_interrupt_descriptor_table_entry(0x20, code_segment,
                                       &handle_interrupt_request_0x00, 0,
                                       IDT_INTERRUPT_GATE);
  set_interrupt_descriptor_table_entry(0x21, code_segment,
                                       &handle_interrupt_request_0x01, 0,
                                       IDT_INTERRUPT_GATE);
  set_interrupt_descriptor_table_entry(0x2C, code_segment,
                                       &handle_interrupt_request_0x0C, 0,
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

void InterruptManager::activate() {
  COM1.Print("[System] Activating Interrupts...");

  if (active_interrupt_manager != 0) {
    active_interrupt_manager->deactivate();
  }

  active_interrupt_manager = this;
  asm("sti");
}

void InterruptManager::deactivate() {
  COM1.Print("[System] Deactivating Interrupts...");

  if (active_interrupt_manager == this) {
    active_interrupt_manager = 0;
    asm("cli");
  }
}

u32 InterruptManager::handle_interrupt(u8 interrupt_number, u32 esp) {
  if (active_interrupt_manager != 0) {
    return active_interrupt_manager->do_handle_interrupt(interrupt_number, esp);
  }

  return esp;
}

u32 InterruptManager::do_handle_interrupt(u8 interrupt_number, u32 esp) {
  if (handlers[interrupt_number] != 0) {
    esp = handlers[interrupt_number]->handle_interrupt(esp);
  } else if (interrupt_number != 0x20) {
    COM1.Print("[System::Error] Unhandled interrupt!");
  }

  if (0x20 <= interrupt_number && interrupt_number < 0x30) {
    pic_master_command.Write(0x20);
    if (0x28 <= interrupt_number) {
      pic_slave_command.Write(0x20);
    }
  }

  return esp;
}