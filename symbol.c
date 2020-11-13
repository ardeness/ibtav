#include <memory.h>
#include "symbol.h"
#include "inst.h"
void buildupSymbolList(Symbollist **sHead, Sectionlist *sectionList, Elf32_Shdr sectionHeader, Elf32_Sym *symbolTable, int shrndx)
{
	char* sectionheadername;
	char* symbolname;
	char* currentsymbolname;
	int symbolnum;
	int i;
	Symbollist *sCurrent;
	Symbollist *sTemp;

	sectionheadername = sectionList[shrndx].sectioncontents;
	for(i=0; sectionList[i].sectionindex != -1; i++)
	{
		if(sectionList[i].sectionHeader.sh_type == SHT_STRTAB && i != shrndx)	// symbol string table
		{
			symbolname = sectionList[i].sectioncontents;
			break;
		}
	}
	if(*sHead == NULL)
	{
		(*sHead) = (Symbollist *)malloc(sizeof(Symbollist));
		(*sHead)->prev = NULL;
		(*sHead)->next = NULL;
		(*sHead)->symbolindex = -1;
	}
	sCurrent = (*sHead);

	while(sCurrent->next != NULL)
		sCurrent = sCurrent->next;

	symbolnum = sectionHeader.sh_size / sizeof(Elf32_Sym);

	for(i=0; i<symbolnum; i++)
	{
#ifdef DEBUG
		if(ELF32_ST_TYPE(symbolTable[i].st_info) == STT_SECTION)
			currentsymbolname = sectionheadername + sectionList[symbolTable[i].st_shndx].sectionHeader.sh_name;
		else
			currentsymbolname = symbolname + symbolTable[i].st_name;
		printf("Symbol %s :  index : %d , st_name : %d \n", currentsymbolname, i, symbolTable[i].st_name);
#endif
		sTemp = (Symbollist*)malloc(sizeof(Symbollist));
		memset(sTemp, 0, sizeof(Symbollist));
		sTemp->symbolinfo = symbolTable[i];
		sTemp->symbolindex = i;

		switch(ELF32_ST_TYPE(symbolTable[i].st_info))
		{
			case STT_FUNC:	// find function entry instruction and end instruction
				sTemp->start_instruction=
					findInstructionbyOffset(
						sectionList[symbolTable[i].st_shndx].instructionhead,
						(long)symbolTable[i].st_value
						);

#ifdef DEBUG
				if(sTemp->start_instruction != NULL)
				{
					printf("start instruction : ");
					printOneInstruction(sTemp->start_instruction);
				}
#endif

				sTemp->end_instruction=
					findInstructionbyUncertainOffset(
						sectionList[symbolTable[i].st_shndx].instructionhead,
						symbolTable[i].st_value + symbolTable[i].st_size
						);
#ifdef DEBUG
				if(sTemp->start_instruction != NULL)
				{
					printf("end instruction : ");
					printOneInstruction(sTemp->end_instruction);
				}
#endif

				break;

			case STT_SECTION:	// find section entry instruction and end instruction only if executable section
				sTemp->start_instruction=
					findInstructionbyOffset(
						sectionList[symbolTable[i].st_shndx].instructionhead,
						symbolTable[i].st_value
						);
#ifdef DEBUG
				if(sTemp->start_instruction != NULL)
				{
					printf("start instruction : ");
					printOneInstruction(sTemp->start_instruction);
				}
#endif
				sTemp->end_instruction = NULL;
				break;

		}

		sCurrent->next = sTemp;
		sTemp->prev = sCurrent;
		sTemp->next = NULL;
		sCurrent=sTemp;
	}
}

unsigned long writeSymbollist(Symbollist *sHead, char *target, unsigned long pos)
{
	Symbollist *sCurrent;

	if(sHead == NULL)
		return 0;

	sCurrent = sHead->next;

	while(sCurrent != NULL)
	{
		memcpy(target + pos, &(sCurrent->symbolinfo), sizeof(Elf32_Sym));
		pos += sizeof(Elf32_Sym);
		sCurrent = sCurrent->next;
	}
	return pos;
}

unsigned long adjustSymbollist(Symbollist *sHead, Sectionlist *sectionlist, int sectionnumber)
{
	Symbollist *sCurrent = NULL;
	unsigned long size = 0;

	if(sHead == NULL)
		return;

	sCurrent = sHead->next;
	while(sCurrent != NULL)
	{
		if(sCurrent->start_instruction != NULL)
		{
			sCurrent->symbolinfo.st_value = sCurrent->start_instruction->offset;
		}

		if(sCurrent->start_instruction != NULL && sCurrent->end_instruction != NULL)
		{
			sCurrent->symbolinfo.st_size = sCurrent->end_instruction->offset + sCurrent->end_instruction->instruction_size - sCurrent->start_instruction->offset;
		}
		size += sizeof(sCurrent->symbolinfo);
		sCurrent = sCurrent->next;
	}
	return size;
}
