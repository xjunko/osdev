#include <kernel/ports.h>
#include <kernel/types.h>

u8 inportb(u16 _port) {
  u8 result = 0;
  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(_port));
  return result;
}

void outportb(u16 _port, u8 data) {
  __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(_port));
}

u8 inportbslow(u16 _port) {
  u8 result = 0;
  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(_port));
  return result;
}

void outportbslow(u16 _port, u8 data) {
  __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:"
                   :
                   : "a"(data), "Nd"(_port));
}

u16 inportw(u16 _port) {
  u16 result = 0;
  __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(_port));
  return result;
}

void outportw(u16 _port, u16 _data) {
  __asm__ volatile("outw %0, %1" : : "a"(_data), "Nd"(_port));
}

u32 inportl(u16 _port) {
  u32 result = 0;
  __asm__ volatile("inl %1, %0" : "=a"(result) : "Nd"(_port));
  return result;
}

void outportl(u16 _port, u32 _data) {
  __asm__ volatile("outl %0, %1" : : "a"(_data), "Nd"(_port));
}