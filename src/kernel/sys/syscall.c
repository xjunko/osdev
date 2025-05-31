#include <errno.h>
#include <kernel/fs/vfs.h>
#include <kernel/idt.h>
#include <kernel/misc/kio.h>
#include <kernel/regs.h>
#include <kernel/serial.h>
#include <kernel/syscall.h>
#include <kernel/types.h>
#include <stdio.h>

#undef errno
extern int errno;

extern u8 __heap_start[];
extern u8 __heap_end[];

static u32 kernel_heap_start = (u32)__heap_start;
static u32 kernel_heap_end = (u32)__heap_start;
static u32 kernel_heap_max = (u32)__heap_end;

void _dump_regs(struct regs* r) {
  kprintf("[regs] a=%d b=%x c=%x d=%x si=%x di=%x bp=%x \n", r->eax, r->ebx,
          r->ecx, r->edx, r->esi, r->edi, r->ebp);
}

// linux style, this should be enough for now
#define SYSCALL_N 512
static syscall_callback* syscall_handlers[SYSCALL_N];
static u8 syscall_irq = 0x80;

void syscall_init() {
  syscall_install();
  idt_set_handler(syscall_irq, syscall_handle);

  printf("[syscall] heaps: start=0x%x, end=0x%x, max=%dMiB \n",
         kernel_heap_start, kernel_heap_end, kernel_heap_max / 1024 / 1024);
}

void syscall_register(u32 syscall_number, syscall_callback* handler) {
  if (syscall_number < SYSCALL_N) {
    syscall_handlers[syscall_number] = handler;
  } else {
    kprintf("[SYSCALL] Invalid syscall number: %d\n", syscall_number);
  }
}

u32 syscall_handle(u32 esp) {
  struct regs* r = (struct regs*)esp;

  u32 syscall_number = r->eax;
#ifdef KERNEL_DEBUG
  _dump_regs(r);
#endif

  if (syscall_handlers[syscall_number] != 0) {
    return (*syscall_handlers[syscall_number])(r);
  } else {
    kprintf("[SYSCALL] Unhandled syscall: %d\n", syscall_number);
  }

  return esp;
}

// implementation
#define SYSCALL_EXIT 0x1
static u32 syscall_exit(struct regs* r) {
  kprintf("[syscall] exit! \n");
  return (u32)r;
}

#define SYSCALL_FORK 0x2
static u32 syscall_fork(struct regs* r) {
  kprintf("[syscall] fork! \n");
  errno = EAGAIN;
  r->eax = -1;
  return (u32)r;
}

#define SYSCALL_READ 0x3
static u32 syscall_read(struct regs* r) {
  int ret = vfs_sys_read((const char*)r->ebx, r->ecx, r->edx);
  if (ret < 0) {
    errno = ENOENT;
    ret = -1;
  }
  r->eax = ret;
  return (u32)r;
}

#define SYSCALL_WRITE 0x4
static u32 syscall_write(struct regs* r) {
  // fd <= 10 is kernel api
  // 1 -> COM1
  u32 fd = r->ebx;
  const char* buffer = (const char*)r->ecx;
  size_t size = r->edx;

  if (fd < VFS_FD_OFFSET) {
    switch (fd) {
      case 1: {  // COM1
        for (size_t i = 0; i < size; i++) {
          serial_putchar(buffer[i]);
        }
        r->eax = size;
      } break;
      default:
        errno = ENOSYS;
        r->eax = -1;
        break;
    }
  }

  if (fd >= VFS_FD_OFFSET) {
    printf("[syscall] write: fd=%d, size=%d \n", fd, size);
    u32 ret = vfs_sys_write(fd, buffer, size);
    if (ret < 0) {
      errno = ENOENT;
      ret = -1;
    }
    r->eax = ret;
  }

  return (u32)r;
}

#define SYSCALL_OPEN 0x5
static u32 syscall_open(struct regs* r) {
  printf("[syscall] open: %s, flags: %d, mode: %d \n", (const char*)r->ebx,
         r->ecx, r->edx);
  int ret = vfs_sys_open((const char*)r->ebx, r->ecx, r->edx);
  if (ret < 0) {
    errno = ENOENT;
    ret = -1;
  }
  r->eax = ret;
  return (u32)r;
}

#define SYSCALL_CLOSE 0x6
static u32 syscall_close(struct regs* r) {
  printf("[syscall] close: %d \n", r->ebx);

  int fd = r->ebx;
  int ret = vfs_sys_close(fd);
  if (ret < 0) {
    errno = ENOENT;
    ret = -1;
  }
  r->eax = ret;
  return (u32)r;
}

#define SYSCALL_TIME 0xD
static u32 syscall_time(struct regs* r) {
  printf("[syscall] time! \n");
  return (u32)r;
}

#define SYSCALL_SBRK_CH 403
static u32 syscall_sbrk_ch(struct regs* r) {
  r->eax = kernel_heap_end;
  return (u32)r;
}

#define SYSCALL_SBRK_NH 404
static u32 syscall_sbrk_nh(struct regs* r) {
  u32 new_break = r->ebx;
  if (new_break >= kernel_heap_start && new_break <= kernel_heap_max) {
    kernel_heap_end = new_break;
  }
  r->eax = kernel_heap_end;
  return (u32)r;
}

// install
void syscall_install() {
  syscall_register(SYSCALL_EXIT, syscall_exit);
  syscall_register(SYSCALL_FORK, syscall_fork);
  syscall_register(SYSCALL_READ, syscall_read);
  syscall_register(SYSCALL_WRITE, syscall_write);
  syscall_register(SYSCALL_OPEN, syscall_open);
  syscall_register(SYSCALL_CLOSE, syscall_close);
  syscall_register(SYSCALL_TIME, syscall_time);

  syscall_register(SYSCALL_SBRK_CH, syscall_sbrk_ch);
  syscall_register(SYSCALL_SBRK_NH, syscall_sbrk_nh);
}