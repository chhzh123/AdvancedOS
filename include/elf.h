// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** elf.h ******/

#ifndef ELF_H
#define ELF_H

#include "stdio.h"

// borrowed from ucore

#define ELF_MAGIC   0x464C457FU         // "\x7FELF" in little endian

/* file header */
typedef struct elfhdr {
    uint32_t e_magic;     // must equal ELF_MAGIC
    uint8_t  e_elf[12];
    uint16_t e_type;      // 1=relocatable, 2=executable, 3=shared object, 4=core image
    uint16_t e_machine;   // 3=x86, 4=68K, etc.
    uint32_t e_version;   // file version, always 1
    uint32_t e_entry;     // entry point if executable
    uint32_t e_phoff;     // file position of program header or 0
    uint32_t e_shoff;     // file position of section header or 0
    uint32_t e_flags;     // architecture-specific flags, usually 0
    uint16_t e_ehsize;    // size of this elf header
    uint16_t e_phentsize; // size of an entry in program header
    uint16_t e_phnum;     // number of entries in program header or 0
    uint16_t e_shentsize; // size of an entry in section header
    uint16_t e_shnum;     // number of entries in section header or 0
    uint16_t e_shstrndx;  // section number that contains section name strings
} elfhdr;

/* program section header */
typedef struct prghdr {
    uint32_t p_type;   // loadable code or data, dynamic linking info, etc.
    uint32_t p_offset; // file offset of segment
    uint32_t p_va;     // virtual address to map segment
    uint32_t p_pa;     // physical address
    uint32_t p_filesz; // size of segment in file
    uint32_t p_memsz;  // size of segment in memory (bigger if contains bss）
    uint32_t p_flags;  // read/write/execute bits
    uint32_t p_align;  // required alignment, invariably hardware page size
} prghdr;

/* values for Proghdr::p_type */
#define ELF_PT_LOAD                     1

/* flag bits for Proghdr::p_flags */
#define ELF_PF_X                        1
#define ELF_PF_W                        2
#define ELF_PF_R                        4

void print_elfhdr(struct elfhdr *eh){
    printf("print_elfhdr:\n");
    printf("    entry: 0x%x\n", eh->e_entry);
    printf("    hdr_size: 0x%x\n", eh->e_ehsize);
    printf("    phoff: 0x%x\n", eh->e_phoff);
    printf("    phnum: %d\n", eh->e_phnum);
    printf("    phentsize: %d\n", eh->e_phentsize);
}

void print_prghdr(struct prghdr *ph){
    printf("print_prghdr:\n");
    printf("    offset: 0x%x\n", ph->p_offset);
    printf("    vaddr: 0x%x\n", ph->p_va);
    printf("    paddr: 0x%x\n", ph->p_pa);
    printf("    filesz: 0x%x\n", ph->p_filesz);
    printf("    memsz: 0x%x\n", ph->p_memsz);
    printf("    flags: 0x%x\n", ph->p_flags);
    printf("    align: 0x%x\n", ph->p_align);
}

static uint8_t bin_img[30 * 512];
uint32_t parse_elf(uintptr_t addr_exec) {

    uintptr_t addr = (uintptr_t)bin_img;

    // parse elf header
    elfhdr eh;
    memcpy((void*)&eh,(void*)addr,sizeof(elfhdr));
#ifdef DEBUG
    print_elfhdr(&eh);
    show_one_sector(addr);
#endif

    if (eh.e_magic != ELF_MAGIC){
        put_error("Bad elf file!");
        for(;;){}
    }

    // parse program header
    prghdr ph;
    for (int i = 0, offset = eh.e_phoff;
            i < eh.e_phnum; i++, offset += eh.e_phentsize){
        memcpy((void*)&ph,(void*)(addr+offset),eh.e_phentsize);
#ifdef DEBUG
        print_prghdr(&ph);
#endif
        if (i == 0)
            memcpy((void*)addr_exec,(void*)(addr+ph.p_offset),ph.p_memsz);
    }

    return eh.e_entry;
}

#endif // ELF_H