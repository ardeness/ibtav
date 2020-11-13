#ifndef _RETMAS_H_
#define _RETMAS_H_
#include <elf.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <memory.h>
#include "inst.h"
#include "cpu.h"

typedef struct
{
	Elf32_Ehdr	*elfHeader;
	Elf32_Shdr	*sectionHeader;
	Elf32_Sym	*symbolTable;
	Elf32_Phdr	*programHeader;
	char		*stringTable;
} machine32info;

typedef struct
{
	Elf64_Ehdr	*elfHeader;
	Elf64_Shdr	*sectionHeader;
	Elf64_Sym	*symbolTable;
	Elf64_Phdr	*programHeader;
	char		*stringTable;
} machine64info;
#endif
