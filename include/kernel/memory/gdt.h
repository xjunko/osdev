#ifndef __LOVE_GDT_H
#define __LOVE_GDT_H

#include <commons/types.h>

namespace RinOS {
namespace Memory {

class GlobalDescriptorTable {
 public:
  class SegmentDescriptor {
   private:
    u16 limit_lo;
    u16 base_lo;
    u8 base_hi;
    u8 type;
    u8 flags;
    u8 base_vhi;

   public:
    SegmentDescriptor(u32 base, u32 limit, u8 type);
    u32 Base();
    u32 Limit();

  } __attribute__((packed));

  SegmentDescriptor null_segment_selector;
  SegmentDescriptor unused_segment_selector;
  SegmentDescriptor code_segment_selector;
  SegmentDescriptor data_segment_selector;

 public:
  GlobalDescriptorTable();
  ~GlobalDescriptorTable();

  u16 CodeSegmentSelector();
  u16 DataSegmentSelector();
};

}  // namespace Memory
}  // namespace RinOS
#endif  // __LOVE_GDT_H