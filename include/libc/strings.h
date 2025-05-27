#pragma once
#include <kernel/types.h>

int bcmp(const void *, const void *, u32);
void bcopy(const void *, void *, u32);
void bzero(void *, u32);
char *index(const char *, int);
char *rindex(const char *, int);