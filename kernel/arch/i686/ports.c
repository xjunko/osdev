#include <kernel/ports.h>
#include <kernel/types.h>

uint8_t inportb(uint16_t _port) {
  uint8_t result = 0;
  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(_port));
  return result;
}

void outportb(uint16_t _port, uint8_t data) {
  __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(_port));
}

uint8_t inportbslow(uint16_t _port) {
  uint8_t result = 0;
  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(_port));
  return result;
}

void outportbslow(uint16_t _port, uint8_t data) {
  __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:"
                   :
                   : "a"(data), "Nd"(_port));
}

uint16_t inportw(uint16_t _port) {
  uint16_t result = 0;
  __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(_port));
  return result;
}

void outportw(uint16_t _port, uint16_t _data) {
  __asm__ volatile("outw %0, %1" : : "a"(_data), "Nd"(_port));
}

uint32_t inportl(uint16_t _port) {
  uint32_t result = 0;
  __asm__ volatile("inl %1, %0" : "=a"(result) : "Nd"(_port));
  return result;
}

void outportl(uint16_t _port, uint32_t _data) {
  __asm__ volatile("outl %0, %1" : : "a"(_data), "Nd"(_port));
}