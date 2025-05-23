#include <kernel/types.h>
#include <libc/stdio.h>

int puts(string str) { return printf("%s\n", str); }