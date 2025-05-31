#pragma once
#include <kernel/types.h>

uint8_t inportb(uint16_t _port);
void outportb(uint16_t _port, uint8_t data);

uint8_t inportbslow(uint16_t _port);
void outportbslow(uint16_t _port, uint8_t data);

uint16_t inportw(uint16_t _port);
void outportw(uint16_t _port, uint16_t _data);

uint32_t inportl(uint16_t _port);
void outportl(uint16_t _port, uint32_t _data);