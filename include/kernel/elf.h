#pragma once
#include <kernel/types.h>

#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6

#define ELFCLASS32 1
#define ELFDATA2LSB 1

#define EM_386 3
#define EV_CURRENT 1
#define ET_EXEC 2
#define ET_DYN 3

#define PT_LOAD 1
#define PT_DYNAMIC 2

#define SHT_REL 9
#define SHT_RELA 4

static const uint8_t ELF_MAGIC[] = {0x7f, 'E', 'L', 'F'};

struct elf_header {
  uint8_t ident[16];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  uint32_t entry;
  uint32_t phoff;
  uint32_t shoff;
  uint32_t flags;
  uint16_t ehsize;
  uint16_t phentsize;
  uint16_t phnum;
  uint16_t shentsize;
  uint16_t shnum;
  uint16_t shstrndx;
};

struct elf_program_header {
  uint32_t type;
  uint32_t offset;
  uint32_t vaddr;
  uint32_t paddr;
  uint32_t filesz;
  uint32_t memsz;
  uint32_t flags;
  uint32_t align;
};

struct elf_section_header {
  uint32_t name;
  uint32_t type;
  uint32_t flags;
  uint32_t addr;
  uint32_t offset;
  uint32_t size;
  uint32_t link;
  uint32_t info;
  uint32_t addralign;
  uint32_t entsize;
};

struct elf_relocation {
  uint32_t offset;
  uint32_t info;
};

int elf_run(uint8_t*, int, char**);