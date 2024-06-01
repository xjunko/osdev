#include <commons/types.h>
#include <hardware/communication/port.h>

using namespace RinOS::Hardware::Communication;

// Base Port
Port::Port(u16 port_number) { this->port_number = port_number; }
Port::~Port() {}

// 8Bit Port
Port8Bit::Port8Bit(u16 port_number) : Port(port_number) {}

Port8Bit::~Port8Bit() {}

void Port8Bit::Write(u8 data) {
  __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port_number));
}

u8 Port8Bit::Read() {
  u8 result;
  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port_number));
  return result;
}

// Slow 8Bit Port
Port8BitSlow::Port8BitSlow(u16 port_number) : Port8Bit(port_number) {}
Port8BitSlow::~Port8BitSlow() {}

void Port8BitSlow::Write(u8 data) {
  __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:"
                   :
                   : "a"(data), "Nd"(port_number));
}

// 16Bit Port
Port16Bit::Port16Bit(u16 port_number) : Port(port_number) {}

Port16Bit::~Port16Bit() {}

void Port16Bit::Write(u16 data) {
  __asm__ volatile("outw %0, %1" : : "a"(data), "Nd"(port_number));
}

u16 Port16Bit::Read() {
  u16 result;
  __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(port_number));
  return result;
}

// 32Bit Port
Port32Bit::Port32Bit(u16 port_number) : Port(port_number) {}

void Port32Bit::Write(u32 data) {
  __asm__ volatile("outl %0, %1" : : "a"(data), "Nd"(port_number));
}

u32 Port32Bit::Read() {
  u32 result;
  __asm__ volatile("inl %1, %0" : "=a"(result) : "Nd"(port_number));
  return result;
}