#include <kernel/mikufs.h>
#include <kernel/types.h>
#include <stdio.h>
#include <stdlib.h>

int fclose(FILE* stream) {
  if (!stream) return -1;
  free((void*)stream->data);  // mikuFS buf
  free(stream->file);         // mikuFS layer
  free(stream);               // stdio FILE
  return 0;
}