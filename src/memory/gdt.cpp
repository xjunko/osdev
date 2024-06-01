#include <memory/gdt.h>
#include <terminal/term.h>

using namespace RinOS::Memory;

GlobalDescriptorTable::GlobalDescriptorTable()
    : null_segment_selector(0, 0, 0),
      unused_segment_selector(0, 0, 0),
      code_segment_selector(0, 64 * 1024 * 1024, 0x9A),
      data_segment_selector(0, 64 * 1024 * 1024, 0x92) {
  RinOS::Terminal::printf("[System] GlobalDescriptorTable Initialized\n");
  u32 i[2];
  i[1] = (u32)this;
  i[0] = sizeof(GlobalDescriptorTable) << 16;

  asm volatile("lgdt (%0)" : : "p"(((u8 *)i) + 2));
}

GlobalDescriptorTable::~GlobalDescriptorTable() {}

u16 GlobalDescriptorTable::DataSegmentSelector() {
  RinOS::Terminal::printf("[DEBUG] DataSegmentSelector called! \n");
  return (u8 *)&data_segment_selector - (u8 *)this;
}

u16 GlobalDescriptorTable::CodeSegmentSelector() {
  RinOS::Terminal::printf("[DEBUG] CodeSegmentSelector called! \n");
  return (u8 *)&code_segment_selector - (u8 *)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(u32 base, u32 limit,
                                                            u8 flags) {
  RinOS::Terminal::printf("[DEBUG] SegmentDescriptor Initialized! \n");
  u8 *target = (u8 *)this;

  if (limit <= 65536) {
    target[6] = 0x40;
  } else {
    if ((limit & 0xFFF) != 0xFFF) {
      limit = (limit >> 12) - 1;
    } else {
      limit = limit >> 12;
    }

    target[6] = 0xC0;
  }

  target[0] = limit & 0xFF;
  target[1] = (limit >> 8) & 0xFF;
  target[6] |= (limit >> 16) & 0xF;

  target[2] = base & 0xFF;
  target[3] = (base >> 8) & 0xFF;
  target[4] = (base >> 16) & 0xFF;
  target[7] = (base >> 24) & 0xFF;

  target[5] = flags;
}

u32 GlobalDescriptorTable::SegmentDescriptor::Base() {
  RinOS::Terminal::printf("[DEBUG] SegmentDescriptor::Base Called! \n");
  u8 *target = (u8 *)this;
  u32 result = target[7];
  result = (result << 8) + target[4];
  result = (result << 8) + target[3];
  result = (result << 8) + target[2];
  return result;
}

u32 GlobalDescriptorTable::SegmentDescriptor::Limit() {
  RinOS::Terminal::printf("[DEBUG] SegmentDescriptor::Limit Called! \n");
  u8 *target = (u8 *)this;
  u32 result = target[6] & 0xF;
  result = (result << 8) + target[1];
  result = (result << 8) + target[0];

  if ((target[6] & 0xC0) == 0xC0) {
    result = (result << 12) | 0xFFF;
  }

  return result;
}
