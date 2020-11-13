#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <elf.h>
#include "inst.h"
#include "cpu.h"

int parse_instruction(Instruction **disasmHead, Elf32_Shdr *sectionHeader, int i, char *elfimage)
{
	int stop;

	char **pcur;
	char *pstart;
	char *pend;

	Instruction* temp;
	Instruction* current;

	if((*disasmHead) == NULL)
	{
		(*disasmHead) = (Instruction*)malloc(sizeof(Instruction));
		(*disasmHead)->next = NULL;
		(*disasmHead)->prev = NULL;
	}
	stop = 0;
	pcur = (char **)malloc(sizeof(char *));

	*pcur = elfimage + sectionHeader[i].sh_offset;
	pstart = elfimage + sectionHeader[i].sh_offset;
	pend = pstart + sectionHeader[i].sh_size -1;

	current = *disasmHead;
	while(current->next != NULL)
		current = current->next;

	while(1)
	{
		temp = (Instruction*)malloc(sizeof(Instruction));
		memset(temp, 0, sizeof(Instruction));
		temp->next = NULL;
		temp->section_index = i;
		temp->offset = *pcur - elfimage - sectionHeader[i].sh_offset;

		temp->absolute_address = sectionHeader[i].sh_addr + temp->offset;

		if((unsigned long)pcur >= (unsigned long)pend)
		{
			free(temp);
			break;
		}
		stop =parse_inst((unsigned char **)pcur, pend, temp);
		if(stop)
		{
			free(temp);
			break;
		}
		temp->isAdded = 0;
		temp->instruction_size=	temp->prefix_size +
								temp->opcode_size +
								temp->modrm_size +
								temp->sib_size +
								temp->displacement_size +
								temp->immediate_size;
		temp->prev = current;
		temp->next = NULL;
		current->next = temp;
		current = temp;
	}

	return 1;
}

void printOneInstruction(Instruction *current)
{
	int i;
	if(current!=NULL)
	{
		printf("%x\t", (unsigned int)current->offset);
		printf("(p)");
		for(i=0; i<current->prefix_size; i++)
			printf("%2x ", current->prefix[i]);
		printf("(op)");
		for(i=0; i<current->opcode_size; i++)
			printf("%2x ", current->opcode[i]);
		printf("(m)");
		if(current->modrm_size)
			printf("%2x ", current->modrm);
		printf("(s)");
		if(current->sib_size)
			printf("%2x ", current->sib);
		printf("(d)");
		for(i=0; i<current->displacement_size; i++)
			printf("%2x ", current->displacement[i]);
		printf("(i)");
		for(i=0; i<current->immediate_size; i++)
			printf("%2x ", current->immediate[i]);
		printf("\n");
	}
}

void printInst(Instruction *instHead)
{
	Instruction *current;

	current = instHead;

	while(current!=NULL)
	{
		printOneInstruction(current);
		current = current->next;
	}
}

Instruction *findInstructionbyOffset(Instruction *iHead, long offset)
{
	Instruction *current = iHead;

	if(iHead == NULL)
		return NULL;

	current = iHead->next;

	while(current!=NULL)
	{
		if(current->offset == offset)
			return current;

		current = current->next;
	}
	return NULL;

}

Instruction *findInstructionbyUncertainOffset(Instruction *iHead, long offset)
{
	Instruction *current = iHead;

	if(iHead == NULL)
		return NULL;

	current = iHead->next;

	while(current != NULL)
	{
		if(current->offset < offset && current->offset + current->instruction_size >= offset)
			return current;

		current = current->next;
	}
	return NULL;
}

unsigned long writeInstructionlist(Instruction *iHead, char *target, unsigned long pos)
{
	Instruction *iCurrent = NULL;

	if(iHead == NULL)
		return 0;

	iCurrent = iHead->next;

	while(iCurrent != NULL)
	{
		if(iCurrent->prefix_size)
		{
			memcpy(target + pos, iCurrent->prefix, iCurrent->prefix_size);
			pos += iCurrent->prefix_size;
		}
		if(iCurrent->opcode_size)
		{
			memcpy(target + pos, iCurrent->opcode, iCurrent->opcode_size);
			pos += iCurrent->opcode_size;
		}
		if(iCurrent->modrm_size)
		{
			memcpy(target + pos, &(iCurrent->modrm), iCurrent->modrm_size);
			pos += iCurrent->modrm_size;
		}
		if(iCurrent->sib_size)
		{
			memcpy(target + pos, &(iCurrent->sib), iCurrent->sib_size);
			pos += iCurrent->sib_size;
		}
		if(iCurrent->displacement_size)
		{
			memcpy(target + pos, iCurrent->displacement, iCurrent->displacement_size);
			pos += iCurrent->displacement_size;
		}
		if(iCurrent->immediate_size)
		{
			memcpy(target + pos, iCurrent->immediate, iCurrent->immediate_size);
			pos += iCurrent->immediate_size;
		}
		iCurrent = iCurrent->next;
	}
	return pos;
}
