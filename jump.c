#include "jump.h"

void findJumptargetandAddreferinginstruction(Sectionlist *cSection,
											Instruction *targetinstruction,
											Instruction *referinstruction)
{
	Instruction **temp;
	Jumptarget *jCurrent = NULL;
	Jumptarget *jTemp = NULL;

	if((cSection->jHead) == NULL)
	{
		cSection->jHead = (Jumptarget *)malloc(sizeof(Jumptarget));
		memset(cSection->jHead, 0, sizeof(Jumptarget));
		cSection->jHead->target_instruction = NULL;
		cSection->jHead->refering_instruction = NULL;
		cSection->jHead->next = NULL;
		cSection->jHead->prev = NULL;
	}
	jCurrent = cSection->jHead;

	while(jCurrent != NULL)
	{
		if(jCurrent->target_instruction == targetinstruction)
		{
			jTemp = jCurrent;
			break;	// we found jump target
		}
		jCurrent = jCurrent->next;
	}

	if(jTemp == NULL)
	{
		jCurrent = cSection->jHead;

		while(jCurrent->next != NULL)
			jCurrent = jCurrent->next;

		jTemp = (Jumptarget *)malloc(sizeof(Jumptarget));
		memset(jTemp, 0, sizeof(Jumptarget));
		jTemp->next = NULL;

		jTemp->target_instruction = targetinstruction;
		jTemp->ref_count = 1;
		jTemp->refering_instruction = (Instruction **)malloc(sizeof(Instruction *) * jTemp->ref_count);
		jTemp->refering_instruction[jTemp->ref_count - 1] = referinstruction;

		jCurrent->next = jTemp;
		jTemp->prev = jCurrent;
	}
	else
	{
		jTemp->ref_count++;
		temp = (Instruction **)malloc(sizeof(Instruction *) * jTemp->ref_count);
		if(jTemp->refering_instruction != NULL)
		{
			memcpy(temp, jTemp->refering_instruction, sizeof(Instruction *) * jTemp->ref_count);
			free(jTemp->refering_instruction);
			jTemp->refering_instruction = temp;
			jTemp->refering_instruction[jTemp->ref_count - 1] = referinstruction;
		}
	}
}
void buildupJumptargetList(Sectionlist *sectionlist, int sectionNumber)
{
	int i;
	Instruction *iCurrent = sectionlist[sectionNumber].instructionhead;// = Head;
	Instruction *targetInstruction = NULL;
	Jumptarget *jCurrent;
	long offset;
	long long_offset;
	char short_offset;
	
	if(iCurrent == NULL)	// no instruction in this section
		return;

	iCurrent = iCurrent->next;
	while(iCurrent != NULL)
	{
		if(iCurrent->instruction_type == INSTTYPE_CALL  && iCurrent->opcode[0] == 0xE8 && iCurrent->isRelocationlist != 1 && iCurrent->displacement_size == 4)
		{
			long_offset = 0;
			for(i=0; i<iCurrent->displacement_size; i++)
			{
				long_offset = *(unsigned long*)(iCurrent->displacement);
			}
			offset = long_offset;
			offset += iCurrent->instruction_size + iCurrent->offset;
			targetInstruction = findInstructionbyOffset(sectionlist[iCurrent->section_index].instructionhead, offset);
			if(targetInstruction != NULL)
				findJumptargetandAddreferinginstruction(sectionlist+targetInstruction->section_index, targetInstruction, iCurrent);
		}
		else if(	(iCurrent->instruction_type == INSTTYPE_JMP &&
						(iCurrent->opcode[0] != 0xEA && iCurrent->opcode[0] != 0xFF)) ||
					iCurrent->instruction_type == INSTTYPE_JCC) 
		{
			if(iCurrent->displacement_size == 1)	// 1 byte displacement
			{
				short_offset = (unsigned short)(iCurrent->displacement[0]);
				offset = short_offset;
			}
			else
			{
				long_offset=0;
				for(i=0; i<iCurrent->displacement_size; i++)
				{
					long_offset=*(unsigned long*)(iCurrent->displacement);
				}
				offset = long_offset;
			}
//			long_offset = 0;
			offset += iCurrent->instruction_size + iCurrent->offset;
			targetInstruction = findInstructionbyOffset(sectionlist[iCurrent->section_index].instructionhead, offset);
			if(targetInstruction != NULL)
				findJumptargetandAddreferinginstruction(sectionlist+targetInstruction->section_index, targetInstruction, iCurrent);
		}
		iCurrent = iCurrent->next;
	}
}

void printJumptarget(Jumptarget *jHead)
{
	int i;

	Jumptarget *jCurrent;

	if(jHead == NULL)
		return;

	jCurrent = jHead->next;

	while(jCurrent!=NULL)
	{
		printf("Target instruction\n");
		printOneInstruction(jCurrent->target_instruction);
		printf("Refering instructions\n");
		for(i=0; i<jCurrent->ref_count; i++)
			printOneInstruction(jCurrent->refering_instruction[i]);

		jCurrent = jCurrent->next;
	}
}
void adjustJumptargetlist(Jumptarget *jHead)
{
	Jumptarget *jCurrent;
	unsigned long target_address;
	Instruction *iCurrent;
	long offset;
	char offset_hex[4];
	int i;

	if(jHead == NULL)
		return;

	jCurrent = jHead->next;

	while(jCurrent != NULL)
	{
		target_address = jCurrent->target_instruction->offset;
		for(i=0; i<jCurrent->ref_count; i++)
		{
			offset = target_address;
			iCurrent = jCurrent->refering_instruction[i];
			offset -= (iCurrent->offset + iCurrent->prefix_size + iCurrent->opcode_size + iCurrent->modrm_size 
						+ iCurrent->sib_size + iCurrent->displacement_size + iCurrent->immediate_size);
			memcpy(offset_hex, &offset, sizeof(long));
			iCurrent->displacement[3] = offset_hex[0];
			iCurrent->displacement[2] = offset_hex[1];
			iCurrent->displacement[1] = offset_hex[2];
			iCurrent->displacement[0] = offset_hex[3];
			memcpy(iCurrent->displacement, &offset, sizeof(long));
		}
		jCurrent = jCurrent->next;
	}
}
