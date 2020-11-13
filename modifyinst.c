#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <elf.h>
#include "inst.h"
#include "cpu.h"
#include "ibtav.h"

#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))

unsigned int inst_count = 0;
unsigned int modified_inst_count = 0;
unsigned int maxstringtablesize = 0;
int currentsymbolposition = -1;
int symbolindex = -1;

Symbollist* addSymbol(char* symbolName, Sectionlist *sectionList)
{
	// add detecting function name to string table

	extern int stringtablesize;
	extern char* stringtable;
	char* oldstringtable = NULL;
	int i,instsize,functionname;

	Symbollist* sTail = NULL;
	Symbollist* sTemp = NULL;

	if(maxstringtablesize == 0)
		maxstringtablesize = stringtablesize;

	functionname = currentsymbolposition = stringtablesize;
	oldstringtable = stringtable;

	if((stringtablesize + strlen(symbolName) + 1) >= maxstringtablesize)
	{
		maxstringtablesize = stringtablesize + strlen(symbolName) + 1;
		maxstringtablesize *= 2;
		stringtable = (char *)malloc(maxstringtablesize * sizeof(char));
		memset(stringtable, 0, sizeof(char) * maxstringtablesize);
		memcpy(stringtable, oldstringtable, stringtablesize);
		if(oldstringtable != NULL)
		{
			//free(oldstringtable);
			oldstringtable = NULL;
		}
	}
	memcpy(stringtable + stringtablesize, symbolName, strlen(symbolName));
	stringtablesize += strlen(symbolName);
	stringtable[stringtablesize] = '\0';
	stringtablesize += 1;
	stringtable[stringtablesize] = '\0';

	// find symbol table
	if(symbolindex == -1)
	{
		for(i=0; sectionList[i].sectionindex != -1; i++)
		{
			if(sectionList[i].sectionHeader.sh_type == SHT_SYMTAB)
			{
				symbolindex = i;
			}
		}
	}
	sTail = sectionList[symbolindex].symbolhead;
	if(sTail == NULL)
		return NULL;
	while(sTail->next != NULL)
		sTail = sTail->next;

	sTemp = (Symbollist*)malloc(sizeof(Symbollist));
	sTemp->start_instruction = NULL;
	sTemp->symbolindex = sTail->symbolindex+1;
	sTemp->symbolinfo.st_name = functionname;
	sTemp->symbolinfo.st_value = 0;
	sTemp->symbolinfo.st_size = 0;
	sTemp->symbolinfo.st_info = ((STB_GLOBAL) << 4) + ((STT_FUNC) & 0xF);
	sTemp->symbolinfo.st_other = 0;
	sTemp->symbolinfo.st_shndx = SHN_UNDEF;

	sTemp->prev = sTail;
	sTemp->next = NULL;
	sTail->next = sTemp;
	sTail = sTemp;
	return sTemp;
}

Relocationlist *addRelocationSection(Sectionlist **sectionListOrigin, int sectionnum, Relocationlist *rTemp)
{
	extern char* name_table;
	extern machine32info ia32;
	extern int stringtablesize;
	extern char* stringtable;
	extern int currentsymbolposition;
	extern unsigned int maxstringtablesize;

	Relocationlist *rHead = NULL;
	int newSectionnameLength;
	char *newSectionname;	
	int oldstringtablesize;
	int oldsymbolindex;
	int oldcurrentsymbolposition;
	int oldmaxstringtablesize;
	char* oldstringtable = NULL;
	int i;
	int changed = 0;
	Sectionlist *oldSectionlist = *sectionListOrigin;

	rHead = (Relocationlist *)malloc(sizeof(Relocationlist));
	rHead->next = rTemp;
	rTemp->prev = rHead;
	rHead->prev = NULL;

	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		if(oldSectionlist[i].sectionHeader.sh_type == SHT_STRTAB &&
			!strcmp((char *)(name_table + oldSectionlist[i].sectionHeader.sh_name), ".shstrtab"))
		{
			changed = 1;
			oldstringtablesize = stringtablesize;
			oldstringtable = stringtable;
			oldcurrentsymbolposition = currentsymbolposition;
			oldsymbolindex = symbolindex;
			oldmaxstringtablesize = maxstringtablesize;

			currentsymbolposition = 0;
			stringtable = oldSectionlist[i].sectioncontents;
			stringtablesize = oldSectionlist[i].sectionHeader.sh_size;
			symbolindex = i;
			maxstringtablesize = stringtablesize;
		}
	}
			
	ia32.elfHeader->e_shnum = ia32.elfHeader->e_shnum + 1;
	newSectionnameLength = strlen(name_table + oldSectionlist[sectionnum].sectionHeader.sh_name) + 5;
	newSectionname = (char *)malloc(sizeof(char) * newSectionnameLength);
	memset(newSectionname, 0, sizeof(char) * newSectionnameLength);
	memcpy(newSectionname, ".rel", 4);
	memcpy(newSectionname + 4, name_table + oldSectionlist[sectionnum].sectionHeader.sh_name, newSectionnameLength - 5);

#ifdef DEBUG
	printf("Adding relocation section : %s\n", newSectionname);
#endif
	addSymbol(newSectionname, *sectionListOrigin);
//	if(newSectionname != NULL)
//	{
//		free(newSectionname);
//		newSectionname = NULL;
//	}

	*sectionListOrigin = (Sectionlist *)malloc(sizeof(Sectionlist) * (ia32.elfHeader->e_shnum + 1));
	memset(*sectionListOrigin, 0, sizeof(Sectionlist) * (ia32.elfHeader->e_shnum + 1));
	memcpy(*sectionListOrigin, oldSectionlist, sizeof(Sectionlist) * (ia32.elfHeader->e_shnum));

	Sectionlist tempsectionList = (*sectionListOrigin)[ia32.elfHeader->e_shnum-1];
	(*sectionListOrigin)[ia32.elfHeader->e_shnum] = tempsectionList;

	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionindex = ia32.elfHeader->e_shnum-1;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectioncontents=NULL;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_name = currentsymbolposition;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_type = SHT_REL;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_flags = 0;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_addr = 0;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_offset = 0;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_size = 0;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_link = symbolindex;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_info = sectionnum;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_addr = 4;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].sectionHeader.sh_entsize = 8;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].instructionhead = NULL;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].symbolhead = NULL;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].relocationhead = rHead;
	(*sectionListOrigin)[ia32.elfHeader->e_shnum-1].jHead = NULL;

	if(changed)
	{
		(*sectionListOrigin)[symbolindex].sectioncontents = stringtable;
		(*sectionListOrigin)[symbolindex].sectionHeader.sh_size = stringtablesize;

		stringtablesize = oldstringtablesize;
		stringtable = oldstringtable;
		currentsymbolposition = oldcurrentsymbolposition;
		symbolindex = oldsymbolindex;
		maxstringtablesize = oldmaxstringtablesize;
	}
	return rTemp;
}

void modifyInstruction(Instruction *iHead, Sectionlist **sectionListOrigin, int sectionnum)
{
	Instruction *iOriginal;
	Instruction *iCurrent;
	Instruction *iTemp;

	extern int stringtablesize;
	extern char* stringtable;

	Symbollist *sTail;
	Symbollist *sTemp;

	Symbollist *sReturn;
	Symbollist *sCall;

	Relocationlist *rTail;
	Relocationlist *rTemp;

	Relocationlist *rReturn;
	Relocationlist *rCall;

	int i;
	int instsize;
	int functionname;

	Sectionlist *sectionList = *sectionListOrigin;

	// add detecting function name to string table
	functionname = stringtablesize;
	stringtable = (char *)realloc(stringtable, sizeof(char) * (stringtablesize + strlen("return_monitor")+1));
	memcpy(stringtable + stringtablesize, "return_monitor", strlen("return_monitor"));
	stringtablesize += strlen("return_monitor") + 1;
	stringtable[stringtablesize-1] = '\0';

	// find symbol table
	for(i=0; sectionList[i].sectionindex != -1; i++)
	{
		if(sectionList[i].sectionHeader.sh_type == SHT_SYMTAB)
		{
			sTail = sectionList[i].symbolhead;
			if(sTail == NULL)
				break;
			while(sTail->next != NULL)
				sTail = sTail->next;

			sTemp = (Symbollist*)malloc(sizeof(*sTemp));
			memset(sTemp, 0, sizeof(Symbollist));
			sTemp->symbolindex = sTail->symbolindex+1;
			sTemp->symbolinfo.st_name = functionname;
			sTemp->symbolinfo.st_value = 0;
			sTemp->symbolinfo.st_size = 0;
			sTemp->symbolinfo.st_info = ((STB_GLOBAL) << 4) + ((STT_FUNC) & 0xF);
			sTemp->symbolinfo.st_other = 0;
			sTemp->symbolinfo.st_shndx = SHN_UNDEF;

			sTemp->prev = sTail;
			sTemp->next = NULL;
			sTail->next = sTemp;
			sTail = sTemp;
			sReturn = sTemp;
		}
	}
	// add detecting function name to string table
	functionname = stringtablesize;
	stringtable = (char *)realloc(stringtable, sizeof(char) * (stringtablesize + strlen("call_monitor")+1));
	memcpy(stringtable + stringtablesize, "call_monitor", strlen("call_monitor"));
	stringtablesize += strlen("call_monitor") + 1;
	stringtable[stringtablesize-1] = '\0';

	// find symbol table
	for(i=0; sectionList[i].sectionindex != -1; i++)
	{
		if(sectionList[i].sectionHeader.sh_type == SHT_SYMTAB)
		{
			sTail = sectionList[i].symbolhead;
			if(sTail == NULL)
				break;
			while(sTail->next != NULL)
				sTail = sTail->next;

			sTemp = (Symbollist*)malloc(sizeof(*sTemp));
			memset(sTemp, 0, sizeof(Symbollist));
			sTemp->symbolindex = sTail->symbolindex+1;
			sTemp->symbolinfo.st_name = functionname;
			sTemp->symbolinfo.st_value = 0;
			sTemp->symbolinfo.st_size = 0;
			sTemp->symbolinfo.st_info = ((STB_GLOBAL) << 4) + ((STT_FUNC) & 0xF);
			sTemp->symbolinfo.st_other = 0;
			sTemp->symbolinfo.st_shndx = SHN_UNDEF;

			sTemp->prev = sTail;
			sTemp->next = NULL;
			sTail->next = sTemp;
			sTail = sTemp;
			sCall = sTemp;
		}
	}
	// find relocation table
	rTail = NULL;
	for(i = 0; sectionList[i].sectionindex != -1; i++)
	{
		if(sectionList[i].sectionHeader.sh_type == SHT_REL && sectionList[i].sectionHeader.sh_info == sectionnum)
		{
			rTail = sectionList[i].relocationhead;

			if(rTail == NULL)
				break;
			while(rTail->next != NULL)
				rTail = rTail->next;
		}
	}

	if(iHead == NULL)
		return;

	iCurrent = iHead->next;
	while(iCurrent != NULL)
	{
		inst_count++;
		switch(iCurrent->opcode[0])
		{
			case 0xEB:	// jmp relavite displacement(1)
				iCurrent->opcode[0] = 0xE9;
				iCurrent->displacement_size = 4;
				iCurrent->instruction_size =	iCurrent->prefix_size +
								iCurrent->opcode_size +
								iCurrent->modrm_size +
								iCurrent->sib_size +
								iCurrent->displacement_size +
								iCurrent->immediate_size;
				break;

			case 0x77:	// JA
			case 0x73:	// JAE
			case 0x72:	// JB, JC
			case 0x76:	// JBE
			case 0x74:	// JE
			case 0x7F:	// JG
			case 0x7D:	// JGE
			case 0x7C:	// JL
			case 0x7E:	// JLE
			case 0x75:	// JNE
			case 0x71:	// JNO
			case 0x7B:	// JNP
			case 0x79:	// JGE
			case 0x70:	// JO
			case 0x7A:	// JP
			case 0x78:	// JPO
				iCurrent->opcode[1] = iCurrent->opcode[0] + 0x10;
				iCurrent->opcode[0] = 0x0F;
				iCurrent->opcode_size = 2;
				iCurrent->displacement_size = 4;
				iCurrent->instruction_size =	iCurrent->prefix_size +
								iCurrent->opcode_size +
								iCurrent->modrm_size +
								iCurrent->sib_size +
								iCurrent->displacement_size +
								iCurrent->immediate_size;
				break;
/*
			case 0xC9:	// LEAVE
				iCurrent->instruction_type = INSTTYPE_JMP;
				iCurrent->instruction_code = INSTCODE_JMP;
				iCurrent->prefix_size = 0;
				iCurrent->opcode_size = 1;
				iCurrent->opcode[0] = 0xE9;
				iCurrent->modrm_size = 0;
				iCurrent->sib_size = 0;
				iCurrent->displacement_size = 4;
				iCurrent->displacement[0] = 0xFC;
				iCurrent->displacement[1] = 0xFF;
				iCurrent->displacement[2] = 0xFF;
				iCurrent->displacement[3] = 0xFF;
				iCurrent->immediate_size = 0;
				iCurrent->instruction_size = 0;
				if(iCurrent->next!=NULL)
				{
					iTemp = iCurrent->next;
					if(iTemp->next!=NULL)
						iTemp->next->prev = iCurrent;
					iCurrent->next = iTemp->next;
					//free(iTemp);
				}
				rTemp = (Relocationlist *)malloc(sizeof(*rTemp));
				memset(rTemp, 0, sizeof(*rTemp));
				rTemp->modifying_instruction = iCurrent;
				rTemp->offset_in_instruction = 1;
				rTemp->relocationinfo.r_info = ((sReturn->symbolindex) << 8) + (unsigned char)(R_386_PC32);
				if(rTail == NULL)
					rTail = addRelocationSection(sectionListOrigin, sectionnum, rTemp);
				else
				{
					rTemp->prev = rTail;
					rTemp->next = NULL;
					rTail->next = rTemp;
					rTail = rTemp;
				}
				break;
*/
			case 0xC3:	// RET
				iCurrent->instruction_type = INSTTYPE_JMP;
				iCurrent->instruction_code = INSTCODE_JMP;
				iCurrent->prefix_size = 0;
				iCurrent->opcode_size = 1;
				iCurrent->opcode[0] = 0xE9;
				iCurrent->modrm_size = 0;
				iCurrent->sib_size = 0;
				iCurrent->displacement_size = 4;
				iCurrent->displacement[0] = 0xFC;
				iCurrent->displacement[1] = 0xFF;
				iCurrent->displacement[2] = 0xFF;
				iCurrent->displacement[3] = 0xFF;
				iCurrent->immediate_size = 0;
				iCurrent->instruction_size = 5;

				rTemp = (Relocationlist *)malloc(sizeof(*rTemp));
				memset(rTemp, 0, sizeof(*rTemp));
				rTemp->modifying_instruction = iCurrent;
				rTemp->offset_in_instruction = 1;
				rTemp->relocationinfo.r_info = ((sReturn->symbolindex) << 8) + (unsigned char)(R_386_PC32);
				if(rTail == NULL)
					rTail = addRelocationSection(sectionListOrigin, sectionnum, rTemp);
				else
				{
					rTemp->prev = rTail;
					rTemp->next = NULL;
					rTail->next = rTemp;
					rTail = rTemp;
				}
				break;

			case 0xFF:
				if(modrm_reg(iCurrent->modrm) == 2 || modrm_reg(iCurrent->modrm) == 3)	// indirect call
				{
					iTemp = (Instruction *)malloc(sizeof(Instruction));
					memset(iTemp, 0, sizeof(Instruction));
					iTemp->instruction_type = INSTTYPE_CALL;
					iTemp->instruction_code = INSTCODE_CALL;
					iTemp->prefix_size = 0;
					iTemp->opcode_size = 1;
					iTemp->opcode[0] = 0xE8;
					iTemp->modrm_size = 0;
					iTemp->sib_size = 0;
					iTemp->displacement_size = 4;
					iTemp->displacement[0] = 0xFC;
					iTemp->displacement[1] = 0xFF;
					iTemp->displacement[2] = 0xFF;
					iTemp->displacement[3] = 0xFF;
					iTemp->immediate_size = 0;
					iTemp->instruction_size = 5;
	
					rTemp = (Relocationlist *)malloc(sizeof(*rTemp));
					memset(rTemp, 0, sizeof(*rTemp));
					rTemp->modifying_instruction = iTemp;
					rTemp->offset_in_instruction = 1;
					rTemp->relocationinfo.r_info = ((sCall->symbolindex) << 8) + (unsigned char)(R_386_PC32);
					if(rTail == NULL)
						rTail = addRelocationSection(sectionListOrigin, sectionnum, rTemp);
					else
					{
						rTemp->prev = rTail;
						rTemp->next = NULL;
						rTail->next = rTemp;
						rTail = rTemp;
					}
					iTemp->next = iCurrent->next;
					if(iCurrent->next != NULL)
						iCurrent->next->prev = iTemp;
					iCurrent->next = iTemp;
					iTemp->prev=iCurrent;
					iCurrent = iTemp;
				}
				break;

				
			case 0x9A:
				iTemp = (Instruction *)malloc(sizeof(Instruction));
				memset(iTemp, 0, sizeof(Instruction));
				iTemp->instruction_type = INSTTYPE_CALL;
				iTemp->instruction_code = INSTCODE_CALL;
				iTemp->prefix_size = 0;
				iTemp->opcode_size = 1;
				iTemp->opcode[0] = 0xE8;
				iTemp->modrm_size = 0;
				iTemp->sib_size = 0;
				iTemp->displacement_size = 4;
				iTemp->displacement[0] = 0xFC;
				iTemp->displacement[1] = 0xFF;
				iTemp->displacement[2] = 0xFF;
				iTemp->displacement[3] = 0xFF;
				iTemp->immediate_size = 0;
				iTemp->instruction_size = 5;
	
				rTemp = (Relocationlist *)malloc(sizeof(*rTemp));
				memset(rTemp, 0, sizeof(*rTemp));
				rTemp->modifying_instruction = iTemp;
				rTemp->offset_in_instruction = 1;
				rTemp->relocationinfo.r_info = ((sCall->symbolindex) << 8) + (unsigned char)(R_386_PC32);
				if(rTail == NULL)
					rTail = addRelocationSection(sectionListOrigin, sectionnum, rTemp);
				else
				{
					rTemp->prev = rTail;
					rTemp->next = NULL;
					rTail->next = rTemp;
					rTail = rTemp;
				}
				iTemp->next = iCurrent->next;
				if(iCurrent->next != NULL)
					iCurrent->next->prev = iTemp;
				iCurrent->next = iTemp;
				iTemp->prev = iCurrent;
				iCurrent = iTemp;
				break;

			default:
				break;
		}
		iCurrent = iCurrent->next;
	}
}

unsigned long adjustInstructionlist(Instruction *iHead, Sectionlist *sectionlist, int sectionnumber)
{
	Instruction *iCurrent;
	unsigned long offset;

	if(iHead == NULL)
		return;

	iCurrent = iHead->next;

	offset = 0;

	while(iCurrent != NULL)
	{
		iCurrent->offset = offset;
		iCurrent->instruction_size = 	iCurrent->prefix_size +
										iCurrent->opcode_size +
										iCurrent->modrm_size +
										iCurrent->sib_size +
										iCurrent->displacement_size +
										iCurrent->immediate_size;
		offset += iCurrent->instruction_size;
		iCurrent = iCurrent->next;
	}
	return offset;
}

void alignInstructionList(Instruction *iHead, Sectionlist *sectionList, int sectionnumber, int alignFactor)
{
	if(alignFactor==0)
		return;
	Instruction *iCurrent;
	unsigned long prev_offset = 0;
	unsigned long offset = 0;
	unsigned long address;
	unsigned long blockStart;
	unsigned long blockEnd;
	unsigned long base;

	Instruction *originalHead;
	Instruction *modifyHead;
	Instruction *iTemp;

	unsigned short state = 0;

	int insttype;

	int size;
	int i;

	iCurrent = iHead;

	base = sectionList[sectionnumber].sectionHeader.sh_addr;
	base = roundup(base, 2<<(alignFactor-1));
	sectionList[sectionnumber].sectionHeader.sh_addr = base;
	sectionList[sectionnumber].sectionHeader.sh_addralign = 2<<(alignFactor-1);
//	blockStart = base;
	blockStart = 0;
	blockEnd = blockStart + (2<<(alignFactor-1)) - 1;

	if(iHead == NULL)
		return;

	iCurrent = iHead->next;
	while(iCurrent!=NULL)
	{
		while(offset > blockEnd)
		{
			blockStart = blockEnd+1;
			blockEnd = blockStart + (2<<(alignFactor-1)) - 1;
		}

		address = base + iCurrent->offset;
		insttype = iCurrent->instruction_type;

		if(	(offset != blockStart)&&
			(insttype == INSTTYPE_JMP	|| insttype == INSTTYPE_CALL	||
			insttype == INSTTYPE_JCC	|| insttype == INSTTYPE_BRANCH	||
			insttype == INSTTYPE_RET))
		{
			size = blockEnd - offset;

			for(i=0; i<=size; i++)
			{
				iTemp = (Instruction *)malloc(sizeof(Instruction));
				iTemp = (Instruction *)malloc(sizeof(*iTemp));
				memset(iTemp, 0, sizeof(*iTemp));
				iTemp->isAdded = 1;
				iTemp->section_index = sectionnumber;
				iTemp->absolute_address = 0;
				iTemp->offset = 0;
				iTemp->instruction_type = INSTTYPE_NORMAL;
				iTemp->instruction_code = INSTCODE_NOP;
				iTemp->prefix_size = 0;
				iTemp->opcode_size = 1;
				iTemp->opcode[0] = 0x90;
				iTemp->modrm_size = 0;
				iTemp->sib_size = 0;
				iTemp->displacement_size = 0;
				iTemp->immediate_size = 0;
				iTemp->instruction_size = 1;
				iCurrent->prev->next=iTemp;
				iTemp->prev = iCurrent->prev;
				iTemp->next = iCurrent;
				offset++;
				iCurrent->prev = iTemp;
			}
			offset += iCurrent->instruction_size;
			iCurrent = iCurrent->next;
		}
		else if( 	iCurrent->isAdded == 1 &&
				iCurrent->blockSize > iCurrent->instruction_size && 
				(offset + iCurrent->blockSize) > (blockEnd-2))
		{
			//printf("There?\n");
			size = blockEnd - offset;

			for(i=0; i<=size; i++)
			{
				iTemp = (Instruction *)malloc(sizeof(Instruction));
				iTemp = (Instruction *)malloc(sizeof(*iTemp));
				memset(iTemp, 0, sizeof(*iTemp));
				iTemp->isAdded = 1;
				iTemp->section_index = sectionnumber;
				iTemp->absolute_address = 0;
				iTemp->offset = 0;
				iTemp->instruction_type = INSTTYPE_NORMAL;
				iTemp->instruction_code = INSTCODE_NOP;
				iTemp->prefix_size = 0;
				iTemp->opcode_size = 1;
				iTemp->opcode[0] = 0x90;
				iTemp->modrm_size = 0;
				iTemp->sib_size = 0;
				iTemp->displacement_size = 0;
				iTemp->immediate_size = 0;
				iTemp->instruction_size = 1;
				iCurrent->prev->next=iTemp;
				iTemp->prev = iCurrent->prev;
				iTemp->next = iCurrent;
				offset++;
				iCurrent->prev = iTemp;
			}
			offset += iCurrent->instruction_size;
			iCurrent = iCurrent->next;
		}
		else if(	iCurrent->isAdded == 0 && blockEnd - offset <= 2)
		{
			size = blockEnd - offset;

			for(i=0; i<=size; i++)
			{
				iTemp = (Instruction *)malloc(sizeof(Instruction));
				iTemp = (Instruction *)malloc(sizeof(*iTemp));
				memset(iTemp, 0, sizeof(*iTemp));
				iTemp->isAdded = 1;
				iTemp->section_index = sectionnumber;
				iTemp->absolute_address = 0;
				iTemp->offset = 0;
				iTemp->instruction_type = INSTTYPE_NORMAL;
				iTemp->instruction_code = INSTCODE_NOP;
				iTemp->prefix_size = 0;
				iTemp->opcode_size = 1;
				iTemp->opcode[0] = 0x90;
				iTemp->modrm_size = 0;
				iTemp->sib_size = 0;
				iTemp->displacement_size = 0;
				iTemp->immediate_size = 0;
				iTemp->instruction_size = 1;
				iCurrent->prev->next=iTemp;
				iTemp->prev = iCurrent->prev;
				iTemp->next = iCurrent;
				offset++;
				iCurrent->prev = iTemp;
			}
		}
		else if(	iCurrent->isAdded == 0 && blockEnd - offset - iCurrent->instruction_size < 2)
		{
			//printf("Here?\n");
			size = blockEnd - offset;

			for(i=0; i<=size; i++)
			{
				iTemp = (Instruction *)malloc(sizeof(Instruction));
				//iTemp = (Instruction *)malloc(sizeof(*iTemp));
				if(iTemp == NULL)
				{
					printf("Not enough heap\n");
					exit(0);
				}
				memset(iTemp, 0, sizeof(Instruction));
				iTemp->isAdded = 1;
				iTemp->section_index = sectionnumber;
				iTemp->absolute_address = 0;
				iTemp->offset = 0;
				iTemp->instruction_type = INSTTYPE_NORMAL;
				iTemp->instruction_code = INSTCODE_NOP;
				iTemp->prefix_size = 0;
				iTemp->opcode_size = 1;
				iTemp->opcode[0] = 0x90;
				iTemp->modrm_size = 0;
				iTemp->sib_size = 0;
				iTemp->displacement_size = 0;
				iTemp->immediate_size = 0;
				iTemp->instruction_size = 1;
				iCurrent->prev->next=iTemp;
				iTemp->prev = iCurrent->prev;
				iTemp->next = iCurrent;
				offset++;
				iCurrent->prev = iTemp;
			}
		}
		else
		{
			offset += iCurrent->instruction_size;
			iCurrent = iCurrent->next;
		}
	}
}
