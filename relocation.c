#include "relocation.h"

void buildupRelocationList(Relocationlist **rHead, Sectionlist *sectionList, Elf32_Shdr sectionHeader, Elf32_Rel *relocationTable, int shrndx)
{
#ifdef DEBUG
	printf("Build up relocation lists\n");
#endif

	char *relocationname;
	char *symbolname;
	char *sectionname;

	int append;

	int position;
	int relocationnum;
	int i;
	int symbolindex;
	Relocationlist *rCurrent;
	Relocationlist *rTemp;
	Symbollist *sHead;
	Symbollist *sCurrent;
	Symbollist *referingSymbol;
	Elf32_Sym cSymbol;
	unsigned long refering_offset;
	unsigned long modifying_offset;


	sectionname = sectionList[shrndx].sectioncontents;

	for(i=0; sectionList[i].sectionindex != -1; i++)
	{
		if(sectionList[i].sectionHeader.sh_type == SHT_STRTAB && i != shrndx)
		{
			symbolname = sectionList[i].sectioncontents;
			break;
		}
	}

	sHead = sectionList[sectionHeader.sh_link].symbolhead;

	if(*rHead == NULL)
	{
		(*rHead) = (Relocationlist *)malloc(sizeof(Relocationlist));
		(*rHead)->prev = NULL;
		(*rHead)->next = NULL;
	}
	rCurrent = (*rHead);

	while(rCurrent->next != NULL)
		rCurrent = rCurrent->next;

	relocationnum = sectionHeader.sh_size / sizeof(Elf32_Rel);

	for(i=0; i<relocationnum; i++)
	{
		rTemp = (Relocationlist *)malloc(sizeof(Relocationlist));
		rTemp->relocationinfo = relocationTable[i];
		rTemp->modifying_instruction = NULL;
		rTemp->refering_instruction = NULL;
		rTemp->modifying_target = NULL;
		rTemp->refering_target = NULL;
		rTemp->refering_symbol = NULL;
		rTemp->prev = NULL;
		rTemp->next = NULL;

		symbolindex = ELF32_R_SYM(relocationTable[i].r_info);

		sCurrent = sHead;

		referingSymbol = NULL;
		while(sCurrent != NULL)
		{
			if(sCurrent->symbolindex == symbolindex)
			{
				referingSymbol = sCurrent;
				break;
			}
			sCurrent = sCurrent->next;
		}

		if(referingSymbol != NULL)
		{
			rTemp->refering_symbol = referingSymbol;
			refering_offset = 0;
			modifying_offset = 0;

			modifying_offset += relocationTable[i].r_offset;

			rTemp->modifying_instruction = findInstructionbyUncertainOffset(sectionList[sectionHeader.sh_info].instructionhead, modifying_offset);
			if(rTemp->modifying_instruction != NULL)
			{
				rTemp->modifying_instruction->isRelocationlist = 1;
				rTemp->offset_in_instruction = modifying_offset - rTemp->modifying_instruction->offset;
				if(rTemp->offset_in_instruction >= 	rTemp->modifying_instruction->prefix_size +
													rTemp->modifying_instruction->opcode_size +
													rTemp->modifying_instruction->modrm_size +
													rTemp->modifying_instruction->sib_size +
													rTemp->modifying_instruction->displacement_size)
				{
					rTemp->position = RELOC_AT_IMMEDIATE;
					rTemp->modifying_target = rTemp->modifying_instruction->immediate;
				}
				else
				{
					rTemp->position = RELOC_AT_DISPLACEMENT;
					rTemp->modifying_target = rTemp->modifying_instruction->displacement;
				}
			}
			else
			{
				rTemp->offset_in_instruction = modifying_offset;
				if(sectionList[sectionHeader.sh_info].sectioncontents != NULL)
					rTemp->modifying_target = sectionList[sectionHeader.sh_info].sectioncontents + modifying_offset;
			}
				
			refering_offset = (referingSymbol->symbolinfo).st_value;
			if(rTemp->modifying_target != NULL)
			{
				append = *(int *)rTemp->modifying_target;
				refering_offset += append;
			/*
				append = 0;
				append += (short)(rTemp->modifying_target[3]);
				append = append << 8;
				append += (short)(rTemp->modifying_target[2]);
				append = append << 8;
				append += (short)(rTemp->modifying_target[1]);
				append = append << 8;
				append += (short)(rTemp->modifying_target[0]);
				refering_offset += append;
			*/
			}
			if(ELF32_ST_TYPE(referingSymbol->symbolinfo.st_info) == STT_FUNC || ELF32_ST_TYPE(referingSymbol->symbolinfo.st_info) == STT_SECTION)
				rTemp->refering_instruction = findInstructionbyOffset(sectionList[(referingSymbol->symbolinfo).st_shndx].instructionhead, refering_offset);
			else
				rTemp->refering_instruction = NULL;
		}
		rCurrent->next = rTemp;
		rTemp->prev = rCurrent;
		rTemp->next = NULL;
		rCurrent = rTemp;

#ifdef DEBUG
//		printf("modifying instruction :");
//		printOneInstruction(rCurrent->modifying_instruction);
//		printf("refering instruction :");
//		printOneInstruction(rCurrent->refering_instruction);
//		printf("\n");
		if(referingSymbol == NULL)
			relocationname = NULL;
		else
		{
			if(ELF32_ST_TYPE(referingSymbol->symbolinfo.st_info) == STT_SECTION)
				relocationname = sectionname + sectionList[referingSymbol->symbolinfo.st_shndx].sectionHeader.sh_name;
			else
				relocationname = symbolname + referingSymbol->symbolinfo.st_name;

			printf("Relocation info : offset = %d, symbol name : %s, refering instruction : %p\n", rCurrent->relocationinfo.r_offset, relocationname, rCurrent->refering_instruction);
			//printf("Symbol info : index = %d\n", referingSymbol->symbolindex);
		}
#endif

	}

#ifdef DEBUG
	printf("End of building relocation lists\n");
#endif
}

unsigned long writeRelocationlist(Relocationlist *rHead, char *target, unsigned long pos)
{
	Relocationlist *rCurrent;

	if(rHead == NULL)
		return 0;

	rCurrent = rHead->next;

	while(rCurrent != NULL)
	{
		memcpy(target + pos, &(rCurrent->relocationinfo), sizeof(Elf32_Rel));
		pos += sizeof(Elf32_Rel);
		rCurrent = rCurrent->next;
	}
	return pos;
}

unsigned long adjustRelocationlist(Relocationlist *rHead, Sectionlist *sectionlist, int sectionnumber)
{
#ifdef DEBUG
	printf("Adjust relocation lists\n");
#endif
	Relocationlist *rCurrent = NULL;
	char *refering_displacement;
	char target_offset[4];
	unsigned long offset;
	unsigned long size = 0;

	if(rHead == NULL)
		return 0;

	rCurrent = rHead->next;

	while(rCurrent != NULL)
	{
		if(rCurrent->modifying_instruction != NULL)
		{
			rCurrent->relocationinfo.r_offset = rCurrent->modifying_instruction->offset +
												rCurrent->modifying_instruction->prefix_size +
												rCurrent->modifying_instruction->opcode_size +
												rCurrent->modifying_instruction->modrm_size +
												rCurrent->modifying_instruction->sib_size;
			if(rCurrent->position == RELOC_AT_IMMEDIATE)
				rCurrent->relocationinfo.r_offset += rCurrent->modifying_instruction->displacement_size;
#ifdef DEBUG
			printf("rcurrent : %x:%d:%d:%d:%d\n", 	rCurrent->modifying_instruction->offset,
													rCurrent->modifying_instruction->prefix_size,
													rCurrent->modifying_instruction->opcode_size,
													rCurrent->modifying_instruction->modrm_size,
													rCurrent->modifying_instruction->sib_size);
#endif
		}
		if(rCurrent->refering_instruction != NULL && rCurrent->modifying_target != NULL)
		{
			offset = rCurrent->refering_instruction->offset;
			if(rCurrent->refering_symbol != NULL)
			{
				offset = offset - rCurrent->refering_symbol->symbolinfo.st_value;
			}
			if(ELF32_R_TYPE(rCurrent->relocationinfo.r_info) != R_386_PC32)
			{
				memcpy(target_offset, &offset, sizeof(unsigned long));
				refering_displacement = (char *)(rCurrent->modifying_target);
				refering_displacement[0] = target_offset[0];
				refering_displacement[1] = target_offset[1];
				refering_displacement[2] = target_offset[2];
				refering_displacement[3] = target_offset[3];
			}
		}
		size += sizeof(rCurrent->relocationinfo);
		rCurrent = rCurrent->next;
	}
#ifdef DEBUG
	printf("End of adjusting relocation list\n");
#endif
	return size;
}

