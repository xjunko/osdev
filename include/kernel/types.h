#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UNUSED(x) (void)(x)
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))