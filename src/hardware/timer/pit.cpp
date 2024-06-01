#include <hardware/timer/pit.h>

using namespace RinOS::Hardware::Timer;

BasicPIT::BasicPIT()
    : channel_0(0x40), channel_1(0x41), channel_2(0x42), command_port(0x43) {}

BasicPIT::~BasicPIT() {}

u32 BasicPIT::read_count() {
  u32 count = 0;

  asm("cli");

  command_port.Write(0b0000000);

  count = channel_0.Read();
  count |= channel_0.Read() << 8;
  asm("sti");

  return count;
}

void BasicPIT::set_count(u32 count) {
  asm("cli");

  channel_0.Write(count & 0xFF);
  channel_0.Write((count >> 8) & 0xFF);

  asm("sti");
}