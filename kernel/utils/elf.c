#include <kernel/elf.h>
#include <kernel/types.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t is_elf(uint8_t* buf) {
  struct elf_header* header = (struct elf_header*)buf;

  for (int i = 0; i < 4; i++) {
    if (header->ident[i] != ELF_MAGIC[i]) {
      printf("[elf] invalid elf format!\n");
      return 0;
    }
  }
  return 1;
}

uint8_t elf_compatibility(uint8_t* buf) {
  struct elf_header* header = (struct elf_header*)buf;

  if (header->ident[EI_CLASS] != ELFCLASS32 ||
      header->ident[EI_DATA] != ELFDATA2LSB ||
      header->ident[EI_VERSION] != EV_CURRENT ||
      (header->type != ET_EXEC && header->type != ET_DYN &&
       header->machine != EM_386)) {
    printf("[elf] incompatible elf format!\n");
    return 0;
  }
  return 1;
}

int elf_run(uint8_t* buf, int argc, char** argv) {
  if (!is_elf(buf) || !elf_compatibility(buf)) {
    return -1;
  }

  size_t size = 0;
  struct elf_header* header = (struct elf_header*)buf;
  struct elf_program_header* ph =
      (struct elf_program_header*)(buf + header->phoff);

  for (int i = 0; i < header->phnum; i++) {
    if (size < ph[i].filesz + ph[i].vaddr) {
      size = ph[i].filesz + ph[i].vaddr;
    }
  }

  uint8_t* proc = malloc(size);
  if (proc == NULL) {
    printf("[elf] failed to allocate memory for process!\n");
    return -1;
  }

  for (int i = 0; i < header->phnum; i++) {
    if (ph[i].type == PT_LOAD || ph[i].type == PT_DYNAMIC) {
      for (size_t y = 0; y < ph[i].filesz; y++) {
        proc[y + ph[i].vaddr] = buf[y + ph[i].offset];
      }
    }
  }

  struct elf_section_header* sh =
      (struct elf_section_header*)(buf + header->shoff);

  for (int i = 0; i < header->shnum; i++) {
    struct elf_section_header* shdr = sh + i;
    if (shdr->type == SHT_REL || shdr->type == SHT_RELA) {
      struct elf_relocation* rel = (struct elf_relocation*)(buf + shdr->offset);
      for (int j = 0; j < shdr->size / sizeof(struct elf_relocation); j++) {
        *(uint32_t*)(proc + rel[j].offset) += (uint32_t)proc;
      }
    }
  }

  int (*run)(int, char**) = (int (*)(int, char**))(proc + header->entry);
  int ret = run(argc, argv);
  free(proc);
  return ret;
}