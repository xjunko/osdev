#include <kernel/pic.h>
#include <kernel/ports.h>
#include <kernel/types.h>

#define IRQ_MASTER_COMMAND 0x20
#define IRQ_MASTER_DATA 0x21
#define IRQ_SLAVE_COMMAND 0xA0
#define IRQ_SLAVE_DATA 0xA1

#define IRQ_EOI 0x20

void irq_remap() {
  outportbslow(IRQ_MASTER_COMMAND, 0x11);
  outportbslow(IRQ_SLAVE_COMMAND, 0x11);

  outportbslow(IRQ_MASTER_DATA, 0x20);
  outportbslow(IRQ_SLAVE_DATA, 0x28);

  outportbslow(IRQ_MASTER_DATA, 0x04);
  outportbslow(IRQ_SLAVE_DATA, 0x02);

  outportbslow(IRQ_MASTER_DATA, 0x01);
  outportbslow(IRQ_SLAVE_DATA, 0x01);

  outportbslow(IRQ_MASTER_DATA, 0x00);
  outportbslow(IRQ_SLAVE_DATA, 0x00);
}

void irq_ack(u8 interrupt_number) {
  if (0x20 <= interrupt_number && interrupt_number < 0x30) {
    outportb(IRQ_MASTER_COMMAND, IRQ_EOI);

    if (0x28 <= interrupt_number) {
      outportb(IRQ_SLAVE_COMMAND, IRQ_EOI);
    }
  }
}