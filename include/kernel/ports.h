#pragma once
#include <kernel/types.h>

u8 inportb(u16 _port);
void outportb(u16 _port, u8 data);

u8 inportbslow(u16 _port);
void outportbslow(u16 _port, u8 data);

u16 inportw(u16 _port);
void outportw(u16 _port, u16 _data);

u32 inportl(u16 _port);
void outportl(u16 _port, u32 _data);