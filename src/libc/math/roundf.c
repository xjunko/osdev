#include <kernel/types.h>
#include <math.h>

f32 roundf(f32 i) {
  if (i >= 0.0f) {
    return (f32)((int)(i + 0.5));
  } else {
    return (f32)((int)(i - 0.5));
  }
}