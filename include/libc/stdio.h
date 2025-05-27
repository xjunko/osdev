#pragma once
#include <kernel/types.h>

int printf(string __restrict__ fmt, ...);
int snprintf(char*, u32 max, string __restrict__ fmt, ...);
int putchar(int c);
int puts(string str);