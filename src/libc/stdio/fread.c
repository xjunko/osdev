#include <kernel/mikufs.h>
#include <kernel/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
  size_t total = size * nmemb;
  if (stream->pos + total > stream->size) {
    total = stream->size - stream->pos;
  }

  memcpy(ptr, stream->data + stream->pos, total);
  return total;
}