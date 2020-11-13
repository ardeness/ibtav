#include "ibtav.h"

char *name_table;

int getElf32(char *elfimage, machine32info *machineinfo, char **nametbl)
{
	int i;
	char buf[1024];
	extern char *name_table;
	name_table = *nametbl;

#ifdef DEBUG
	printf("%d\n", sizeof(Elf32_Ehdr));
#endif

	Elf32_Ehdr *ia32 = (Elf32_Ehdr *)elfimage;

#ifdef DEBUG
	printf("Entering elf header parsing mode.........\n");
#endif

	// object type checking. only executable image is allowed
	if(ia32->e_type != ET_EXEC)
	{
		#ifdef DEBUG
		printf("Not a executable image.\n");
		#endif
	//	return 0;
	}

	// magic number check
	if
	(!(	ia32->e_ident[EI_MAG0]==ELFMAG0 &&
		ia32->e_ident[EI_MAG1]==ELFMAG1 &&
		ia32->e_ident[EI_MAG2]==ELFMAG2 &&
		ia32->e_ident[EI_MAG3]==ELFMAG3
	))
	{
		#ifdef DEBUG
		printf("Wrong magic number.\n");
		#endif
		return 0;
	}

	// machine check - targetting only ia
	if(ia32->e_machine != EM_386)
	{
		#ifdef DEBUG
		printf("Wrong executable image : do not targeting Intel Architecture\n");
		#endif
		return 0;
	}

	// version check - only EV_CURRENT is allowed
	if(ia32->e_version != EV_CURRENT)
	{
		#ifdef DEBUG
		printf("Wrong ELF version.\n");
		#endif
		return 0;
	}

	// CPU type check. only 32bit object is allowed
	if(ia32->e_ident[EI_CLASS] != ELFCLASS32)
	{
		#ifdef DEBUG
		printf("Wrong executable. only 32bit object is allowed.\n");
		#endif
		return 0;
	}

	machineinfo->elfHeader     = (Elf32_Ehdr *)elfimage;

	// initialize structures
	// section header and program header

	machineinfo->sectionHeader = (Elf32_Shdr *)(elfimage + ia32->e_shoff);
	machineinfo->programHeader = (Elf32_Phdr *)(elfimage + ia32->e_phoff);

	// find string table
	*nametbl = (char *)(elfimage + machineinfo->sectionHeader[ia32->e_shstrndx].sh_offset);
	name_table = *nametbl;

#ifdef DEBUG
	printf("phnum : %d\n", machineinfo->elfHeader->e_phnum);
	// find symbol table
	//printf("%s\n", symboltable);
#endif

	// showing section header's informations
#ifdef DEBUG
	printf("Section header's informations\n");
	printf("type\t\t\toffset\tsize\tname");
	for(i=0; i<ia32->e_shnum; i++)
	{
		switch(machineinfo->sectionHeader[i].sh_type)
		{
			case SHT_NULL:
				printf("SHT_NULL\t\t\t");
				break;

			case SHT_PROGBITS:
				printf("SHT_PROGBITS\t\t");
				break;

			case SHT_SYMTAB:
				printf("SHT_SYMTAB\t\t");
				break;

			case SHT_STRTAB:
				printf("SHT_STRTAB\t\t");
				break;

			case SHT_RELA:
				printf("SHT_RELA\t\t\t");
				break;

			case SHT_HASH:
				printf("SHT_HASH\t\t\t");
				break;

			case SHT_DYNAMIC:
				printf("SHT_DYNAMIC\t\t");
				break;

			case SHT_NOTE:
				printf("SHT_NOTE\t\t");
				break;

			case SHT_NOBITS:
				printf("SHT_NOBITS\t\t");
				break;

			case SHT_REL:
				printf("SHT_REL\t\t\t");
				break;

			case SHT_SHLIB:
				printf("SHT_SHLIB\t\t");
				break;

			case SHT_DYNSYM:
				printf("SHT_DYNSYM\t\t");
				break;

			case SHT_LOPROC:
				printf("SHT_LOPROC\t\t");
				break;

			case SHT_HIPROC:
				printf("SHT_HIPROC\t\t");
				break;

			case SHT_LOUSER:
				printf("SHT_LOUSER\t\t");
				break;

			case SHT_HIUSER:
				printf("SHT_HIUSER\t\t");
				break;

			default:
				printf("Unrecognized type\t");
				break;
		}
		printf("%d\t%d\t%s\n",
				machineinfo->sectionHeader[i].sh_offset,
				machineinfo->sectionHeader[i].sh_size,
				name_table + machineinfo->sectionHeader[i].sh_name
			);
	}
#endif


	// showing program header's informations
#ifdef DEBUG
	printf("\nProgram header's informations\n");
	printf("header name\toffset\t vaddr\t\t paddr\t\tfilesz\tmemsize\talign\tflags\n");
	for(i=0; i<machineinfo->elfHeader->e_phnum; i++)
	{
		switch(machineinfo->programHeader[i].p_type)
		{
			case PT_NULL:
				printf("PT_NULL\t\t");
				break;

			case PT_LOAD:
				printf("PT_LOAD\t\t");
				break;

			case PT_DYNAMIC:
				printf("PT_DYNAMIC\t");
				break;

			case PT_INTERP:
				printf("PT_INTERP\t");
				break;

			case PT_NOTE:
				printf("PT_NOTE\t\t");
				break;

			case PT_SHLIB:
				printf("PT_SHLIB\t");
				break;

			case PT_PHDR:
				printf("PT_PHDR\t\t");
				break;

			case PT_LOPROC:
				printf("PT_LOPROC\t");
				break;

			case PT_TLS:
				printf("PT_TLS\t\t");
				break;

			case PT_NUM:
				printf("PT_NUM\t\t");
				break;

			case PT_LOOS:
				printf("PT_LOOS\t\t");
				break;

			case PT_GNU_EH_FRAME:
				printf("PT_GNU_EH_FRAME\t");
				break;

			case PT_GNU_STACK:
				printf("PT_GNU_STACK\t");
				break;

			case PT_GNU_RELRO:
				printf("PT_GNU_RELRO\t");
				break;


/* sun specified values
			case PT_LOSUNW:
				printf("PT_LOSUNW\t");
				break;

			case PT_SUNWBSS:
				printf("PT_SUNWBSS\t");
				break;

			case PT_SUNWSTACK:
				printf("PT_SUNWSTACK\t");
				break;

			case PT_HISUNW:
				printf("PT_HISUNW\t");
				break;

			case PT_HIOS:
				printf("PT_HIOS\t");
				break;
end of os-specific*/

			case PT_HIPROC:
				printf("PT_HIPROC\t");
				break;

			default:
				printf("Unrecognizable type of program header\n");
		}
		printf("%d\t%10p\t%10p\t%d\t%d\t%d\t",
			machineinfo->programHeader[i].p_offset,
			machineinfo->programHeader[i].p_vaddr,
			machineinfo->programHeader[i].p_paddr,
			machineinfo->programHeader[i].p_filesz,
			machineinfo->programHeader[i].p_memsz,
			machineinfo->programHeader[i].p_align
			);

		if(machineinfo->programHeader[i].p_flags & PF_X)
			printf("PF_X ");
		if(machineinfo->programHeader[i].p_flags & PF_W)
			printf("PF_W ");
		if(machineinfo->programHeader[i].p_flags & PF_R)
			printf("PF_R ");
		if(machineinfo->programHeader[i].p_flags & PF_MASKOS)
			printf("PF_MASKOS ");
		if(machineinfo->programHeader[i].p_flags & PF_MASKPROC)
			printf("PF_MASKPROC ");

		printf("\n");

	}

#endif

	// all job finished successfully
#ifdef DEBUG
	printf("Elf header parsing success.\n");
#endif

	return 1;
}
