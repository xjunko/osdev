#pragma once
#include <kernel/types.h>

int atoi(const char*);
long atol(const char*);
long long atoll(const char*);
double atof(const char*);

void* malloc(u32 size);
void* realloc(void*, u32);
void free(void* ptr);

int abs(int);
long labs(long);
long long llabs(long long);