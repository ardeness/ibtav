#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include "inst.h"

void buildupSymbolList(Symbollist **sHead, Sectionlist *sectionList, Elf32_Shdr sectionHeader, Elf32_Sym *symbolTable, int shrndx);
unsigned long writeSymbollist(Symbollist *sHead, char *target, unsigned long pos);
unsigned long adjustSymbollist(Symbollist *sHead, Sectionlist *sectionlist, int sectionnumber);

#endif
