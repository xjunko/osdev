#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static inline uint8_t asm_inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

static inline void asm_outb(uint16_t port, uint8_t val) {
  asm volatile("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}