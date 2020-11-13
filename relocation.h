#ifndef _RELOCATION_H_
#define _RELOCATION_H_

#include "inst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void buildupRelocationList(Relocationlist **rHead, Sectionlist *sectionList, Elf32_Shdr sectionHeader, Elf32_Rel *relocationTable, int shrndx);
unsigned long adjustRelocationlist(Relocationlist *rHead, Sectionlist *sectionlist, int sectionnumber);
unsigned long writeRelocationlist(Relocationlist *rHead, char *target, unsigned long pos);

#endif
