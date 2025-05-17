#include <global/coreutils/sleep.h>
#include <kernel/hardware/timer/pit.h>

void RinOS::Utility::sleep(int millisecond) {
  RinOS::Hardware::Timer::BasicPIT pit;

  for (int i = 0; i < millisecond; i++) {
    pit.set_count(0x2E9A);

    u32 start = pit.read_count();
    while ((start - pit.read_count()) < 1000) {
    }
  }
}
