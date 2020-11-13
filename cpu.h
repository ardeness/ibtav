#ifndef _CPU_H_
#define _CPU_H_

// 16bit register
#define AX		0x00
#define BX		0x01
#define CX		0x02
#define DX		0x03
#define BP		0x04
#define SI		0x05
#define DI		0x06
#define SP		0x07
#define AL		0x08
#define AH		0x09
#define BL		0x0A
#define BH		0x0B
#define CL		0x0C
#define CH		0x0D
#define DL		0x0E
#define DH		0x0F

// 32bit register
#define EAX		0x10
#define EBX		0x11
#define ECX		0x12
#define EDX		0x13
#define EBP		0x14
#define ESI		0x15
#define EDI		0x16
#define ESP		0x17
#define EAL		0x18
#define EAH		0x19
#define EBL		0x1A
#define EBH		0x1B
#define ECL		0x1C
#define ECH		0x1D
#define EDL		0x1E
#define EDH		0x1F

// 64bit register
#define RAX		0x20
#define RBX		0x21
#define RCX		0x22
#define RDX		0x23
#define RBP		0x24
#define RSI		0x25
#define RDI		0x26
#define RSP		0x27
#define R8D		0x28
#define R9D		0x29
// these registers are available only in 64-bit mode
#define R10D	0x2A
#define R11D	0x2B
#define R12D	0x2C
#define R13D	0x2D
#define R14D	0x2E
#define R15D	0x2F

// segment register
#define CS		0x30
#define SS		0x31
#define DS		0x32
#define ES		0x33
#define FS		0x34
#define GS		0x35

#define modrm_mod(c) ( (c >> 6) & 0x03 )
#define modrm_reg(c) ( (c >> 3) & 0x07 )
#define modrm_rm(c) (c & 0x07)
#define sib_scale(c) modrm_mod(c)
#define sib_index(c) modrm_reg(c)
#define sib_base(c) modrm_rm(c)


#define INSTRUCTION_OVERFLOW (-2)
#define WRONG_INSTRUCTION (-1)
#define PARSE_SUCCESS 0
int parse_inst(unsigned char **byte, unsigned char *endbyte, Instruction *inst);
void getOpcode(unsigned char **byte, unsigned char *endbyte, Instruction *inst, int size);
void getModRMParams(unsigned char **byte, unsigned char *endbyte, Instruction *inst);
void getImmediate(unsigned char **byte, unsigned char *endbyte, Instruction *inst, int size);
void getDisplacement(unsigned char **byte, unsigned char *endbyte, Instruction *inst, int size);
int isPrefix(Instruction *inst, unsigned char prefix);
void getPrefix(unsigned char **byte, unsigned char *endbyte, Instruction *inst);
int parse_error(int error);
#endif
