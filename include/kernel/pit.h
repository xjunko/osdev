#include <kernel/types.h>

#define PIT_CHAN_0 0x40
#define PIT_CHAN_1 0x41
#define PIT_CHAN_2 0x42
#define PIT_CTRL 0x43

#define PIT_MASK 0xFF
#define PIT_HZ 120
#define PIT_SCALE 1193180
#define PIT_SET 0x36

void pit_write(uint32_t);
uint32_t pit_read();
void pit_sleep(uint32_t);