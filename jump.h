#ifndef _JUMP_H_
#define _JUMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "inst.h"

void findJumptargetandAddreferinginstruction(Sectionlist *cSection, Instruction *targetinstruction, Instruction *referinstruction);
void buildupJumptargetList(Sectionlist *sectionlist, int sectionNumber);
void printJumptarget(Jumptarget *jHead);
void adjustJumptargetlist(Jumptarget *jHead);

#endif
