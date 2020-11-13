#include "ibtav.h"

unsigned short alignFactor;
int stringtablesize;
char *stringtable;
machine32info ia32;

unsigned long adjustSectionlist(Sectionlist *sectionlist, int sectionnum, int shstrndx)
{
	extern unsigned short alignFactor;
	extern int stringtablesize;
	extern char *stringtable;
	int i;

	unsigned long offset;

	for(i=0; i<sectionnum; i++)
	{
		if(sectionlist[i].instructionhead != NULL)
		{
			sectionlist[i].sectionHeader.sh_size = adjustInstructionlist(sectionlist[i].instructionhead, sectionlist, i);	
			if(alignFactor > 0)
			{
			//	alignInstructionList(sectionlist[i].instructionhead, sectionlist, i, alignFactor);
				sectionlist[i].sectionHeader.sh_size = adjustInstructionlist(sectionlist[i].instructionhead, sectionlist, i);
			}
		}
		if(	sectionlist[i].sectionHeader.sh_type == SHT_NULL)
			sectionlist[i].sectionHeader.sh_size = 0;
	}
	for(i=0; i<sectionnum; i++)
	{
		if(sectionlist[i].symbolhead != NULL)
		{
			sectionlist[i].sectionHeader.sh_size = adjustSymbollist(sectionlist[i].symbolhead, sectionlist, i);
		}
	}
	for(i=0; i<sectionnum; i++)
	{
		if(sectionlist[i].relocationhead != NULL)
		{
			sectionlist[i].sectionHeader.sh_size = adjustRelocationlist(sectionlist[i].relocationhead, sectionlist, i);
		}
		else if(sectionlist[i].sectionHeader.sh_type == SHT_STRTAB && i != shstrndx)
		{
			sectionlist[i].sectioncontents = stringtable;
			sectionlist[i].sectionHeader.sh_size = stringtablesize;
		}
	}

	for(i=0; i<sectionnum; i++)
	{
		if(sectionlist[i].jHead != NULL)
		{
			adjustJumptargetlist(sectionlist[i].jHead);
		}
	}

	offset = sizeof(Elf32_Ehdr) + sizeof(Elf32_Shdr) * sectionnum;
	for(i=0; i<sectionnum; i++)
	{
		sectionlist[i].sectionHeader.sh_offset = offset;
		offset += sectionlist[i].sectionHeader.sh_size;
	}
	return offset;
}

int main(int argc, char **argv)
{
	extern unsigned short alignFactor;
	extern int stringtablesize;
	extern char* stringtable;
	extern unsigned int inst_count;
	extern unsigned int modified_inst_count;

	extern machine32info ia32;

	int		i;
	int		originalfile;
	int		targetFile;
	char	*emptybuf;
	char	*targetFilename;
	char	*targetFileimage;
	char	*elfimage;
	char	*nametable;

	Sectionlist	*sectionlist = NULL;
	Jumptarget	*jumplistHead = NULL;

	Elf32_Sym	*symboltagble = NULL;
	Elf32_Sym	*dynsymboltable = NULL;

	Elf32_Rel *relocationtable = NULL;
	Elf32_Rela *relocationatable = NULL;

	Symbollist *sHead;
	Relocationlist *rHead;

	struct stat statbuf;
	unsigned long size = 0;
	unsigned long pos = 0;


	if(argc < 2)
	{
		printf("Usage : %s [file_to_duplicate_code]\n", argv[0]);
		return 0;
	}

	alignFactor=2;

	if(argc>2)
		alignFactor=atoi(argv[2]);

	originalfile = open(argv[1], O_RDWR);
	// check original file
	if(originalfile < 0)	// fail to open file
	{
		perror(argv[1]);
		return 0;
	}

	// modify target name : original file name + "_result"
	targetFilename = (char *)malloc(sizeof(char) * (strlen(argv[1]) + strlen("_ibtav.o") + 1));
	memset(targetFilename, 0, sizeof(char) * (strlen(argv[1]) + strlen("_ibtav.o") + 1));
	memcpy(targetFilename, argv[1], strlen(argv[1]));
	memcpy(targetFilename+strlen(targetFilename)-2, "_ibtav.o", strlen("_ibtav.o"));

	fstat(originalfile, &statbuf);
	elfimage = mmap(NULL, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, originalfile, 0);

	if(elfimage == (void *)-1)
	{
		perror(NULL);
		return 0;
	}

	// distill elf informations
	if(!getElf32(elfimage, &ia32, &nametable))
	{
		return 0;
	}

	sectionlist = (Sectionlist *)malloc(sizeof(Sectionlist) * (ia32.elfHeader->e_shnum + 1));
	sectionlist[ia32.elfHeader->e_shnum].sectionindex = -1; 

	for(i=0; i<ia32.elfHeader->e_shnum;i++)
	{
		sectionlist[i].sectionindex = i;
		sectionlist[i].instructionhead = NULL;
		sectionlist[i].symbolhead = NULL;
		sectionlist[i].relocationhead = NULL;
		sectionlist[i].jHead = NULL;
		sectionlist[i].sectioncontents = NULL;
		memcpy(&(sectionlist[i].sectionHeader), (ia32.sectionHeader)+i, sizeof(Elf32_Shdr));

		if(ia32.sectionHeader[i].sh_flags & SHF_EXECINSTR)
		{
			parse_instruction(&(sectionlist[i].instructionhead), ia32.sectionHeader, i, elfimage);
		}
		else if(ia32.sectionHeader[i].sh_type == SHT_NULL || ia32.sectionHeader[i].sh_type == SHT_NOBITS || ia32.sectionHeader[i].sh_type >= SHT_NUM)
			sectionlist[i].sectioncontents = NULL;
		else
		{
			sectionlist[i].sectioncontents = (char *)malloc(sectionlist[i].sectionHeader.sh_size);
			memcpy(sectionlist[i].sectioncontents, elfimage + sectionlist[i].sectionHeader.sh_offset, sectionlist[i].sectionHeader.sh_size);
		}
	}


	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		if(ia32.sectionHeader[i].sh_type == SHT_SYMTAB || ia32.sectionHeader[i].sh_type == SHT_DYNSYM)
		{
			buildupSymbolList(&(sectionlist[i].symbolhead), sectionlist, ia32.sectionHeader[i], elfimage + ia32.sectionHeader[i].sh_offset, ia32.elfHeader->e_shstrndx);
			sHead = sectionlist[i].symbolhead;
		}
	}

	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		if(ia32.sectionHeader[i].sh_type == SHT_REL)
		{
			buildupRelocationList(&(sectionlist[i].relocationhead), sectionlist, ia32.sectionHeader[i], elfimage + ia32.sectionHeader[i].sh_offset, ia32.elfHeader->e_shstrndx);
		}
	}

	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		if(sectionlist[i].instructionhead != NULL)
		{
			buildupJumptargetList(sectionlist, i);
		}
	}

	// find string table
	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		if(ia32.sectionHeader[i].sh_type == SHT_STRTAB)
		{
			stringtablesize = ia32.sectionHeader[i].sh_size;
		//	sectionlist[i].sectioncontents = (char *)malloc(sizeof(char) * stringtablesize);
		//	memcpy(sectionlist[i].sectioncontents, elfimage + ia32.sectionHeader[i].sh_offset, stringtablesize);
			stringtable = sectionlist[i].sectioncontents;
		}
	}
	
#ifdef DEBUG
	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		if(sectionlist[i].instructionhead != NULL)
		{
			printInst(sectionlist[i].instructionhead);
		}
	}
#endif

#ifdef DEBUG
	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		if(sectionlist[i].jHead != NULL)
		{
			printJumptarget(sectionlist[i].jHead);
		}
	}
#endif

	// Modify instruction to achieve my goal - Main algorithm of RETMAS
	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		if(sectionlist[i].instructionhead != NULL)
		{
			modifyInstruction(sectionlist[i].instructionhead, &sectionlist, i);
		}
	}

	// recalcaulate all section informations
	// and write it all to target file
	size = adjustSectionlist(sectionlist, ia32.elfHeader->e_shnum, ia32.elfHeader->e_shstrndx);

#ifdef DEBUG
	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		if(sectionlist[i].instructionhead != NULL)
		{
			printInst(sectionlist[i].instructionhead);
		}
	}
#endif

	emptybuf = (char *)malloc(sizeof(char) * size);
	memset(emptybuf, '1', size);
	targetFile = open(targetFilename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH);
	if(targetFile < 0)
		return 0;
	write(targetFile, emptybuf, size);
	fstat(targetFile, &statbuf);
	targetFileimage = mmap(NULL, size, PROT_WRITE, MAP_SHARED, targetFile, 0);
	memset(targetFileimage, 0, size);
	memcpy(targetFileimage, ia32.elfHeader, sizeof(Elf32_Ehdr));
	ia32.elfHeader = (Elf32_Ehdr *)targetFileimage;
	ia32.elfHeader->e_shoff = sizeof(Elf32_Ehdr);
	pos = sizeof(Elf32_Ehdr);

	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		memcpy(targetFileimage + pos, &(sectionlist[i].sectionHeader), sizeof(Elf32_Shdr));
		pos += sizeof(Elf32_Shdr);
	}

	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		sectionlist[i].sectionHeader.sh_offset = pos;
		if(sectionlist[i].instructionhead != NULL)
			pos = writeInstructionlist(sectionlist[i].instructionhead, targetFileimage, pos);
		else if(sectionlist[i].symbolhead != NULL)
			pos = writeSymbollist(sectionlist[i].symbolhead, targetFileimage, pos);
		else if(sectionlist[i].relocationhead != NULL)
			pos = writeRelocationlist(sectionlist[i].relocationhead, targetFileimage, pos);
		else if(sectionlist[i].sectioncontents != NULL)
		{
			memcpy(targetFileimage + pos, sectionlist[i].sectioncontents, sectionlist[i].sectionHeader.sh_size);
			pos += sectionlist[i].sectionHeader.sh_size;
		}
	}
	pos = sizeof(Elf32_Ehdr);

	for(i=0; i<ia32.elfHeader->e_shnum; i++)
	{
		memcpy(targetFileimage + pos, &(sectionlist[i].sectionHeader), sizeof(Elf32_Shdr));
		pos += sizeof(Elf32_Shdr);
	}

	printf("total instruction : %u, modified instruction : %u\n", inst_count, modified_inst_count);

	return 0;

}
