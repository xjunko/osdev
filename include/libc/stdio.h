#pragma once
#include <kernel/mikufs.h>
#include <kernel/types.h>

typedef struct {
  const char* filename;
  u8* data;
  size_t size;
  size_t pos;
  struct miku_fs_file* file;
} FILE;

int printf(string __restrict__ fmt, ...);
int putchar(int c);
int puts(string str);

FILE* fopen(const char* filename, const char* mode);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
int fclose(FILE* stream);