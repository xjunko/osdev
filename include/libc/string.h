#pragma once
#include <kernel/types.h>

void *memcpy(void *, const void *, u32);
void *memmove(void *, const void *, u32);
void *memset(void *, int, u32);
int memcmp(const void *, const void *, u32);
void *memchr(const void *, int, u32);

char *strcpy(char *, const char *);
char *strncpy(char *, const char *, u32);

char *strcat(char *, const char *);
char *strncat(char *, const char *, u32);

int strcmp(const char *, const char *);
int strncmp(const char *, const char *, u32);

u32 strlen(const char *);