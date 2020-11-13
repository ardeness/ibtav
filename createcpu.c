#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "inst.h"
#include "cpu.h"

Instruction* create_inst(unsigned char **_byte, unsigned char *_endbyte, Instruction *src)
{
	Instruction *inst; 

	unsigned char buf[17];
	unsigned char *buf_pos;
	unsigned char **byte;
	unsigned char *endbyte;
	int i = 0;

	if(_byte == NULL)
	{
		memcpy(buf, src->prefix, src->prefix_size);
		i += src->prefix_size;
		memcpy(buf+i, src->opcode, src->opcode_size);
		i += src->opcode_size;
		memcpy(buf+i, &src->modrm, src->modrm_size);
		i += src->modrm_size;
		memcpy(buf+i, &src->sib, src->sib_size);
		i += src->sib_size;
		memcpy(buf+i, src->displacement, src->displacement_size);
		i += src->displacement_size;
		memcpy(buf+i, src->immediate, src->immediate_size);
		i += src->immediate_size;

		buf_pos = buf;
		endbyte = buf+i;
		byte = &buf_pos;

		inst = (Instruction *)malloc(sizeof(Instruction));
		if(inst == NULL)
			return NULL;
	
		inst->section_index = src->section_index;
		inst->absolute_address = 0;
		inst->offset = 0;
		inst->isRelocationlist = 0;
		inst->prefix_size = 0;
		inst->opcode_size = 0;
		inst->modrm_size = 0;
		inst->sib_size = 0;
		inst->displacement_size = 0;
		inst->immediate_size = 0;
		inst-> instruction_size = 0;
		inst->prev = NULL;
		inst->next = NULL;
	}
	else
	{
		byte = _byte;
		endbyte = _endbyte;
		inst = src;
	}
	switch(**byte)
	{
		/* one byte opcodes */
		// ADD
		case 0x00:	// 00/r		: add r/m8, r8
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADD;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x01:	// 01/r		: add r/m16, r16 / add r/m32, r32
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADD;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x02:	// 
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADD;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x03:	// 03/r		: add r/m16, r16 / add r/m32, r32
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADD;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x04:	// 04 ib	: add al, imm8
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADD;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
//			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x05:	// 05 iw	: add ax, imm16 / 32
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADD;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
//			modModRMParams(byte, endbyte, inst, 9);
			break;


		// PUSH ES
		case 0x06:	// 06		: push es
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// POP ES
		case 0x07:	// 07		: pop es
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// OR
		case 0x08:	// 08/r		: or r/m8, r8
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_POP;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x09:	// 09/r		: or r/m16, r16 / or r/m32, r32
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_OR;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x0A:	// 0A/r		: or
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_OR;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x0B: // OR
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_OR;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x0C:	// OR AL
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_OR;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		case 0x0D:	// OR eax
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_OR;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// PUSH CS(i64)
		case 0x0E:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		// 2-byte escape
		//case 0x0F:

		// ADC
		case 0x10:
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADC;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x11:
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADC;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x12:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADC;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x13:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADC;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x14:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADC;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		case 0x15:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADC;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// PUSH SS
		case 0x16:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ADC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// POP SS
		case 0x17:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_POP;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// SBB
		case 0x18:
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SBB;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x19:
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SBB;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x1A:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SBB;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x1B:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SBB;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x1C:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SBB;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		case 0x1D:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SBB;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// PUSH DS(i64)
		case 0x1E:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// POP DS(i64)
		case 0x1F:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_POP;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// AND
		case 0x20:
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

		case 0x21:
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

		case 0x22:
		case 0x23:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_AND;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x24:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_AND;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		case 0x25:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_AND;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// SEG=ES (prefix)
		case 0x26:
			modPrefix(byte, endbyte, inst);
			return create_inst(byte, endbyte, inst);
			break;

		// DAA
		case 0x27:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DAA;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// SUB
		case 0x28:
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

		case 0x29:
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

		case 0x2A:
		case 0x2B:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SUB;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x2C:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SUB;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		case 0x2D:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SUB;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// SEG=CS (prefix)
		case 0x2E:
			modPrefix(byte, endbyte, inst);
			return create_inst(byte, endbyte, inst);
			break;

		// DAS(i64)
		case 0x2F:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DAS;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// XOR
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_XOR;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x34:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_XOR;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;
			
		case 0x35:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_XOR;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// SEG=CS (prefix)
		case 0x36:
			modPrefix(byte, endbyte, inst);
			return create_inst(byte, endbyte, inst);
			break;

		// AAA
		case 0x37:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_AAA;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// CMP
		case 0x38:
		case 0x39:
		case 0x3A:
		case 0x3B:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_CMP;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		case 0x3C:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_CMP;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		case 0x3D:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_CMP;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// SEG=DS (prefix)
		case 0x3E:
			modPrefix(byte, endbyte, inst);
			return create_inst(byte, endbyte, inst);
			break;

		// AAS(i64)
		case 0x3F:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_AAS;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// INC(i64) or REX(o64) prefix : no REX in ia32 mode
		// register selected with opcode(40~47)
		case 0x40:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_INC;
			modOpcode(byte, endbyte, inst, 1);
			inst->opcode[inst->opcode_size - 1] = 0x41;
			break;
		case 0x41:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_INC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x42:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_INC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x43:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_INC;
			modOpcode(byte, endbyte, inst, 1);
			inst->opcode[inst->opcode_size - 1] = 0x42;
			break;
		case 0x44:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_INC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x45:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_INC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x46:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_INC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x47:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_INC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// DEC(i64) general register or REX(o64) prefix : no REX in ia32 mode
		// register selected with opcode(48~4F)
		case 0x48:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DEC;
			modOpcode(byte, endbyte, inst, 1);
			inst->opcode[inst->opcode_size - 1] = 0x49;
			break;
		case 0x49:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DEC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x4A:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DEC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x4B:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DEC;
			modOpcode(byte, endbyte, inst, 1);
			inst->opcode[inst->opcode_size - 1] = 0x4A;
			break;
		case 0x4C:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DEC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x4D:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DEC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x4E:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DEC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;
		case 0x4F:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_DEC;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// PUSH(d64) general register
		// register selected with opcode(50~57)
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55:
		case 0x56:
		case 0x57:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// POP(d64) general register
		// register selected with opcode(58~5F)
		case 0x58:
		case 0x59:
		case 0x5A:
		case 0x5B:
		case 0x5C:
		case 0x5D:
		case 0x5E:
		case 0x5F:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// PUSHA / PUSHAD
		case 0x60:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// POPA / POPAD
		case 0x61:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_POP;
			modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
			break;

		// BOUND
		case 0x62:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_BOUND;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		// ARPL(i64) or MOVSXD(o64)
		case 0x63:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ARPL;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		// SEG=FS (prefix)
		case 0x64:
			modPrefix(byte, endbyte, inst);
			return create_inst(byte, endbyte, inst);
			break;

		// SEG=GS (prefix)
		case 0x65:
			modPrefix(byte, endbyte, inst);
			return create_inst(byte, endbyte, inst);
			break;

		// operand size (prefix)
		case 0x66:
			modPrefix(byte, endbyte, inst);
			return create_inst(byte, endbyte, inst);
			break;

		// address size (prefix)
		case 0x67:
			modPrefix(byte, endbyte, inst);
			return create_inst(byte, endbyte, inst);
			break;

		// PUSH(d64)
		case 0x68:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			inst->modrm_size = 2;
			break;

		// IMUL
		case 0x69:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_IMUL;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// PUSH(d64)
		case 0x6A:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			inst->modrm_size = 2;
			break;

		// IMUL
		case 0x6B:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_IMUL;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		// INS / INSB
		case 0x6C:
		// INS / INSW / INSD
		case 0x6D:
			inst->instruction_type = INSTTYPE_IO;
			inst->instruction_code = INSTCODE_INS;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// OUTS / OUTSB
		case 0x6E:
		// OUTS / OUTSW / OUTSD
		case 0x6F:
			inst->instruction_type = INSTTYPE_IO;
			inst->instruction_code = INSTCODE_OUTS;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// JCC
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x76:
		case 0x77:
		case 0x78:
		case 0x79:
		case 0x7A:
		case 0x7B:
		case 0x7C:
		case 0x7D:
		case 0x7E:
		case 0x7F:
			inst->instruction_type = INSTTYPE_JCC;
			inst->instruction_code = INSTCODE_JCC;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, BYTE);
			break;

		// Immediate Grp 1
		case 0x80:
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ADD;
					modModRMParams(byte, endbyte, inst, 0);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 1:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_OR;
					modModRMParams(byte, endbyte, inst, 1);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 2:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ADC;
					modModRMParams(byte, endbyte, inst, 2);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 3:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SBB;
					modModRMParams(byte, endbyte, inst, 3);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 4:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_AND;
					modModRMParams(byte, endbyte, inst, 4);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 5:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SUB;
					modModRMParams(byte, endbyte, inst, 5);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 6:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_XOR;
					modModRMParams(byte, endbyte, inst, 6);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 7:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_CMP;
					modModRMParams(byte, endbyte, inst, 7);
					modImmediate(byte, endbyte, inst, BYTE);
					break;
				
				default:
					return NULL;
					break;
			}
			break;

		case 0x81:
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ADD;
					modModRMParams(byte, endbyte, inst, 0);
					modImmediate(byte, endbyte, inst, WORD);
					break;

				case 1:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_OR;
					modModRMParams(byte, endbyte, inst, 1);
					modImmediate(byte, endbyte, inst, WORD);
					break;

				case 2:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ADC;
					modModRMParams(byte, endbyte, inst, 2);
					modImmediate(byte, endbyte, inst, WORD);
					break;

				case 3:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SBB;
					modModRMParams(byte, endbyte, inst, 3);
					modImmediate(byte, endbyte, inst, WORD);
					break;

				case 4:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_AND;
					modModRMParams(byte, endbyte, inst, 4);
					modImmediate(byte, endbyte, inst, WORD);
					break;

				case 5:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SUB;
					modModRMParams(byte, endbyte, inst, 5);
					modImmediate(byte, endbyte, inst, WORD);
					break;

				case 6:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_XOR;
					modModRMParams(byte, endbyte, inst, 6);
					modImmediate(byte, endbyte, inst, WORD);
					break;

				case 7:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_CMP;
					modModRMParams(byte, endbyte, inst, 7);
					modImmediate(byte, endbyte, inst, WORD);
					break;
				
				default:
					return NULL;
					break;
			}
			break;

		case 0x82:
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ADD;
					modModRMParams(byte, endbyte, inst, 0);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 1:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_OR;
					modModRMParams(byte, endbyte, inst, 1);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 2:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ADC;
					modModRMParams(byte, endbyte, inst, 2);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 3:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SBB;
					modModRMParams(byte, endbyte, inst, 3);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 4:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_AND;
					modModRMParams(byte, endbyte, inst, 4);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 5:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SUB;
					modModRMParams(byte, endbyte, inst, 5);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 6:
//					if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//						return NULL;

					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_XOR;
					modModRMParams(byte, endbyte, inst, 6);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 7:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_CMP;
					modModRMParams(byte, endbyte, inst, 7);
					modImmediate(byte, endbyte, inst, BYTE);
					break;
				
				default:
					return NULL;
					break;
			}
			break;

		case 0x83:
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ADD;
					modModRMParams(byte, endbyte, inst, 0);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 1:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_OR;
					modModRMParams(byte, endbyte, inst, 1);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 2:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ADC;
					modModRMParams(byte, endbyte, inst, 2);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 3:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SBB;
					modModRMParams(byte, endbyte, inst, 3);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 4:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_AND;
					modModRMParams(byte, endbyte, inst, 4);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 5:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SUB;
					modModRMParams(byte, endbyte, inst, 5);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 6:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_XOR;
					modModRMParams(byte, endbyte, inst, 6);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 7:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_CMP;
					modModRMParams(byte, endbyte, inst, 7);
					modImmediate(byte, endbyte, inst, BYTE);
					break;
				
				default:
					return NULL;
					break;
			}
			break;

		// TEST
		case 0x84:
		case 0x85:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_TEST;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;


		// XCHG
		case 0x86:
		case 0x87:
//			if(!isPrefix(inst, 0xF0))	// require LOCK prefix
//				return NULL;

			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_XCHG;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		// MOV
		case 0x88:
		case 0x89:
		case 0x8A:
		case 0x8B:
		case 0x8C:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		// LEA
		case 0x8D:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_LEA;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		// MOV
		case 0x8E:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		// Grp 1A POP(d64)
		case 0x8F:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_POP;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			inst->modrm_size = 2;
			break;

		// NOP, PAUSE, XCHG r8, rAX
		// xchg does not exchange ax with ax
		case 0x90:
			if(isPrefix(inst, 0xF3))
			{
				inst->instruction_type = INSTTYPE_CONTROL;
				inst->instruction_code = INSTCODE_PAUSE;
			}
			else
			{
				inst->instruction_type = INSTTYPE_CONTROL;
				inst->instruction_code = INSTCODE_NOP;
			}
			modOpcode(byte, endbyte, inst, 1);
			break;

		// XCHG r16/32 AX
		case 0x91:
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x96:
		case 0x97:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_XCHG;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// CBW / CWDE / CDQE
		case 0x98:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTTYPE_CONVERT;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// CWD / CDQ / CQO
		case 0x99:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTTYPE_CONVERT;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// CALLF(i64)	: not a correct routine. FIXME!!!!!!!!!!!!!!!!!!!!!
		case 0x9A:
			inst->instruction_type = INSTTYPE_CALL;
			inst->instruction_code = INSTCODE_CALL;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, WORD);
			break;

		//FWAIT / WAIT
		case 0x9B:
			inst->instruction_type = INSTTYPE_CONTROL;
			inst->instruction_code = INSTTYPE_WAIT;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// PUSHF/D/Q(d64)
		case 0x9C:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_PUSH;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// POPF/D/Q(d64)
		case 0x9D:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_POP;
			modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
			break;

		// SAHF
		case 0x9E:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SAHF;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// LAHF
		case 0x9F:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_LAHF;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// MOV
		case 0xA0:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, WORD);
			break;

		case 0xA1:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, WORD);
			break;

		case 0xA2:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, WORD);
			break;

		case 0xA3:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, WORD);
			break;

		// MOVS/B
		case 0xA4:
		// MOVS/W/D/Q
		case 0xA5:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// CMPS/B
		case 0xA6:
		// CMPS/W/D
		case 0xA7:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// TEST
		case 0xA8:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_TEST;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		case 0xA9:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_TEST;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// STOS/B
		case 0xAA:
		// STOS/W/D/Q
		case 0xAB:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_STORE;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// LODS/B
		case 0xAC:
		// LODS/W/D/Q
		case 0xAD:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_LOAD;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// SCAS/B
		case 0xAE:
		// // SCAS/W/D/Q
		case 0xAF:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SCAN;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// MOV immediate byte into byte register
		case 0xB0:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			inst->opcode[inst->opcode_size - 1] = 0xB1;
			break;
		case 0xB1:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			inst->modrm_size = 2;
			break;
		case 0xB2:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			inst->modrm_size = 2;
			break;
		case 0xB3:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			inst->opcode[inst->opcode_size - 1] = 0xB2;
			break;
		case 0xB4:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			inst->modrm_size = 2;
			break;
		case 0xB5:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			inst->modrm_size = 2;
			break;
		case 0xB6:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			inst->modrm_size = 2;
			break;
		case 0xB7:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			inst->modrm_size = 2;
			break;

		case 0xB8:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;
		case 0xB9:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;
		case 0xBA:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;
		case 0xBB:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;
		case 0xBC:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;
		case 0xBD:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;
		case 0xBE:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;
		case 0xBF:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// Shift Grp 2
		case 0xC0:
			modOpcode(byte, endbyte, inst, 1);
			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ROL;
					modModRMParams(byte, endbyte, inst, 0);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 1:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ROR;
					modModRMParams(byte, endbyte, inst, 1);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 2:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_RCL;
					modModRMParams(byte, endbyte, inst, 2);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 3:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_RCR;
					modModRMParams(byte, endbyte, inst, 3);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 4:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SHL;
					modModRMParams(byte, endbyte, inst, 4);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 5:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SHR;
					modModRMParams(byte, endbyte, inst, 5);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 6:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SAL;
					modModRMParams(byte, endbyte, inst, 6);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 7:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SAR;
					modModRMParams(byte, endbyte, inst, 7);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				default:
					return NULL;
					break;
			}
			break;

		case 0xC1:
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;
			
			switch(modrm_reg(**byte))
			{
				case 0:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ROL;
					modModRMParams(byte, endbyte, inst, 0);
					modImmediate(byte, endbyte, inst, WORD);
					break;

				case 1:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ROR;
					modModRMParams(byte, endbyte, inst, 1);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 2:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_RCL;
					modModRMParams(byte, endbyte, inst, 2);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 3:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_RCR;
					modModRMParams(byte, endbyte, inst, 3);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 4:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SHL;
					modModRMParams(byte, endbyte, inst, 4);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 5:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SHR;
					modModRMParams(byte, endbyte, inst, 5);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 6:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SAL;
					modModRMParams(byte, endbyte, inst, 6);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 7:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SAR;
					modModRMParams(byte, endbyte, inst, 7);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				default:
					return NULL;
					break;
			}
			break;

		// RETN
		case 0xC2:
			inst->instruction_type = INSTTYPE_BRANCH;
			inst->instruction_code = INSTCODE_RET;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, HALFWORD);
			break;

		// RETN
		case 0xC3:
			inst->instruction_type = INSTTYPE_BRANCH;
			inst->instruction_code = INSTCODE_RET;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// LES
		case 0xC4:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_LES;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;			

		// LDS
		case 0xC5:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_LDS;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 9);
			break;

		// GRP 11 - MOV
		case 0xC6:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 0);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		case 0xC7:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_MOV;
			modOpcode(byte, endbyte, inst, 1);
			modModRMParams(byte, endbyte, inst, 0);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// ENTER
		case 0xC8:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_ENTER;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		// LEAVE(d64)
		case 0xC9:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTTYPE_LEAVE;
			modOpcode(byte, endbyte, inst, 1);
			inst->modrm_size = 2;
			break;

		// RETF
		case 0xCA:
			inst->instruction_type = INSTTYPE_BRANCH;
			inst->instruction_code = INSTTYPE_RET;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, WORD);
			break;

		case 0xCB:
			inst->instruction_type = INSTTYPE_BRANCH;
			inst->instruction_code = INSTTYPE_RET;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// INT 3
		case 0xCC:
			inst->instruction_type = INSTTYPE_INT;
			inst->instruction_code = INSTCODE_INT;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// INT
		case 0xCD:
			inst->instruction_type = INSTTYPE_INT;
			inst->instruction_code = INSTTYPE_INT;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		// INTO(i64)
		case 0xCE:
			inst->instruction_type = INSTTYPE_CALL;
			inst->instruction_code = INSTTYPE_INTO;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// IRET/D/Q
		case 0xCF:
			inst->instruction_type = INSTTYPE_BRANCH;
			inst->instruction_code = INSTTYPE_RET;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// Shift Grp 2
		case 0xD0:
		case 0xD1:
		case 0xD2:
		case 0xD3:
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ROL;
					modModRMParams(byte, endbyte, inst, 0);
					break;

				case 1:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_ROR;
					modModRMParams(byte, endbyte, inst, 1);
					break;

				case 2:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_RCL;
					modModRMParams(byte, endbyte, inst, 2);
					break;

				case 3:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_RCR;
					modModRMParams(byte, endbyte, inst, 3);
					break;

				case 4:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SHL;
					modModRMParams(byte, endbyte, inst, 4);
					break;

				case 5:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SHR;
					modModRMParams(byte, endbyte, inst, 5);
					break;

				case 6:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SAL;
					modModRMParams(byte, endbyte, inst, 6);
					break;

				case 7:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_SAR;
					modModRMParams(byte, endbyte, inst, 7);
					break;

				default:
					return NULL;
					break;
			}
			break;


		// AAM
		case 0xD4:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_AAM;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			if(**byte != 0x0A)	// imm8
				modImmediate(byte, endbyte, inst, BYTE);
			else
				modOpcode(byte, endbyte, inst, 1);

			break;

		// AAD
		case 0xD5:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_AAD;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			if(**byte != 0x0A)	// imm8
				modImmediate(byte, endbyte, inst, BYTE);
			else
				modOpcode(byte, endbyte, inst, 1);

			break;

		// SALC / SETALC
		case 0xD6:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_SALC;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// XLAT / XLATB
		case 0xD7:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_XLAT;
			modOpcode(byte, endbyte, inst, 1);
			break;
			

		// ESC(Escape to coprocessor instruction set)
		case 0xD8:
			inst->instruction_type = INSTTYPE_FLOAT;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			if(**byte == 0xD1)	// FCOM
			{
				inst->instruction_code = INSTCODE_FCOM;
				modOpcode(byte, endbyte, inst, 1);
			}
			else if(**byte == 0xD9)	// FCOMP
			{
				inst->instruction_code = INSTCODE_FCOMP;
				modOpcode(byte, endbyte, inst, 1);
			}
			else
			{
				switch(modrm_reg(**byte))
				{
					case 0:
						inst->instruction_code = INSTCODE_FADD;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:
						inst->instruction_code = INSTCODE_FMUL;
						modModRMParams(byte, endbyte, inst, 1);
						break;

					case 2:
						inst->instruction_code = INSTCODE_FCOM;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:
						inst->instruction_code = INSTCODE_FCOMP;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					case 4:
						inst->instruction_code = INSTCODE_FSUB;
						modModRMParams(byte, endbyte, inst, 4);
						break;

					case 5:
						inst->instruction_code = INSTCODE_FSUBR;
						modModRMParams(byte, endbyte, inst, 5);
						break;

					case 6:
						inst->instruction_code = INSTCODE_FDIV;
						modModRMParams(byte, endbyte, inst, 6);
						break;

					case 7:
						inst->instruction_code = INSTCODE_FDIVR;
						modModRMParams(byte, endbyte, inst, 7);
						break;

					default:
						return NULL;
						break;
				}
			}
			break;

		case 0xD9:
			inst->instruction_type = INSTTYPE_FLOAT;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			if(**byte == 0xC9)	// FXCH
			{
				inst->instruction_code = INSTCODE_FXCH;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xD0)	// FNOP
			{
				inst->instruction_code = INSTCODE_FNOP;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xE0)	// FCHS
			{
				inst->instruction_code = INSTCODE_FCHS;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xE1)	// FABS
			{
				inst->instruction_code = INSTCODE_FABS;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xE4)	// FTST
			{
				inst->instruction_code = INSTCODE_FTST;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xE5)	// FXAM
			{
				inst->instruction_code = INSTCODE_FXAM;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xE8)	// FLD1
			{
				inst->instruction_code = INSTCODE_FLD1;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xE9)	// FLDL2T
			{
				inst->instruction_code = INSTCODE_FLDL2T;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xEA)	// FLDL2E
			{
				inst->instruction_code = INSTCODE_FLDL2E;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xEB)	// FLDPI
			{
				inst->instruction_code = INSTCODE_FLDPI;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xEC)	// FLDLG2
			{
				inst->instruction_code = INSTCODE_FLDLG2;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xED)	// FLDLN2
			{
				inst->instruction_code = INSTCODE_FLDLN2;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xEE)	// FLDZ
			{
				inst->instruction_code = INSTCODE_FLDZ;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF0)	// F2XM1
			{
				inst->instruction_code = INSTCODE_F2XM1;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF1)	// FYL2X
			{
				inst->instruction_code = INSTCODE_FYL2X;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF2)	// FPTAN
			{
				inst->instruction_code = INSTCODE_FPTAN;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF3)	// FPATAN
			{
				inst->instruction_code = INSTCODE_FPATAN;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF4)	// FXTRACT
			{
				inst->instruction_code = INSTCODE_FXTRACT;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF5)	// FPREM1
			{
				inst->instruction_code = INSTCODE_FPREM1;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF6)	// FDECSTP
			{
				inst->instruction_code = INSTCODE_FPREM1;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF7)	// FINCSTP
			{
				inst->instruction_code = INSTCODE_FINCSTP;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF8)	// FPREM
			{
				inst->instruction_code = INSTCODE_FPREM;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xF9)	// FLY2XP1
			{
				inst->instruction_code = INSTCODE_FLY2XP1;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xFA)	// FSQRT
			{
				inst->instruction_code = INSTCODE_FSQRT;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xFB)	// FSINCOS
			{
				inst->instruction_code = INSTCODE_FSINCOS;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xFC)	// FRNDINT
			{
				inst->instruction_code = INSTCODE_FRNDINT;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xFD)	// FSCALE
			{
				inst->instruction_code = INSTCODE_FSCALE;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xFE)	// FSIN
			{
				inst->instruction_code = INSTCODE_FSIN;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else if(**byte == 0xFF)	// FCOS
			{
				inst->instruction_code = INSTCODE_FCOS;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else
			{
				switch(modrm_reg(**byte))
				{
					case 0:
						inst->instruction_code = INSTCODE_FLD;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:
						inst->instruction_code = INSTCODE_FXCH;
						modModRMParams(byte, endbyte, inst, 1);
						break;

					case 2:
						inst->instruction_code = INSTCODE_FST;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:
						inst->instruction_code = INSTCODE_FSTP;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					case 4:
						inst->instruction_code = INSTCODE_FLDENV;
						modModRMParams(byte, endbyte, inst, 4);
						break;

					case 5:
						inst->instruction_code = INSTCODE_FLDCW;
						modModRMParams(byte, endbyte, inst, 5);
						break;

					case 6:
						if(isPrefix(inst, 0x9B))	// FSTENV
						{
							inst->instruction_code = INSTCODE_FSTENV;
							modModRMParams(byte, endbyte, inst, 6);
						}
						else
						{
							inst->instruction_code = INSTCODE_FNSTENV;
							modModRMParams(byte, endbyte, inst, 6);
						}
						break;

					case 7:
						if(isPrefix(inst, 0x9B))	// FSTCW
						{
							inst->instruction_code = INSTCODE_FNSTCW;
							modModRMParams(byte, endbyte, inst, 7);
						}
						else	// FNSTCW
						{
							inst->instruction_code = INSTCODE_FNSTCW;
							modModRMParams(byte, endbyte, inst, 7);
						}
						break;

					default:
						return NULL;
						break;
				}
			}
			break;

		case 0xDA:
			inst->instruction_type = INSTTYPE_FLOAT;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			if(**byte == 0xE9)	// FUCOMPP
			{
				inst->instruction_code = INSTCODE_FUCOMPP;
				modOpcode(byte, endbyte, inst, 1);
			//	modModRMParams(byte, endbyte, inst, 9);
			}
			else
			{
				switch(modrm_reg(**byte))
				{
					case 0:	// FIADD / FCMOVB : seleted by c flags
						inst->instruction_code = INSTCODE_FIADD;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:	// FIMUL / FCMOVE : seleted by z flags
						inst->instruction_code = INSTCODE_FIMUL;
						modModRMParams(byte, endbyte, inst, 1);
						break;
						
					case 2:	// FICOM / FCMOVU : seleted by z flags
						inst->instruction_code = INSTCODE_FICOM;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:	// FICOMP / FCMOVU : seleted by p flags
						inst->instruction_code = INSTCODE_FICOMP;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					case 4:	// FISUB
						inst->instruction_code = INSTCODE_FISUB;
						modModRMParams(byte, endbyte, inst, 4);
						break;

					case 5:	// FISUBR
						inst->instruction_code = INSTCODE_FISUBR;
						modModRMParams(byte, endbyte, inst, 5);
						break;

					case 6:	// FIDIV
						inst->instruction_code = INSTCODE_FIDIV;
						modModRMParams(byte, endbyte, inst, 6);
						break;

					case 7:	// FIDIVR
						inst->instruction_code = INSTCODE_FIDIVR;
						modModRMParams(byte, endbyte, inst, 7);
						break;

					default:
						return NULL;
						break;
				}
			}
			break;

		case 0xDB:
			inst->instruction_type = INSTTYPE_FLOAT;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			if(**byte == 0xE0)	// FNENI
			{
				inst->instruction_code = INSTCODE_FNENI;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 4);
			}
			else if(**byte == 0xE1)
			{
				inst->instruction_code = INSTCODE_FNDISI;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 4);
			}
			else if(**byte == 0xE2)
			{
				if(isPrefix(inst, 0x9B))	// FCLEX
					inst->instruction_code = INSTCODE_FCLEX;
				else	// FNCLEX
					inst->instruction_code = INSTCODE_FNDISI;

				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 4);
			}
			else if(**byte == 0xE3)
			{
				if(isPrefix(inst, 0x9B))	// FINIT
					inst->instruction_code = INSTCODE_FINIT;
				else	// FNINIT
					inst->instruction_code = INSTCODE_FNINIT;

				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 4);
			}
			else if(**byte == 0xE4)	// FNSETPM
			{
				inst->instruction_code = INSTCODE_FNSETPM;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 4);
			}
			else
			{
				switch(modrm_reg(**byte))
				{
					case 0:	// FILD / FCOMOVNB : selected by c flags
						inst->instruction_code = INSTCODE_FILD;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:	// FISTTP / FCMOVNE : selected by z flags
						inst->instruction_code = INSTCODE_FISTTP;
						modModRMParams(byte, endbyte, inst, 1);
						break;

					case 2:	// FIST / FCMOVNBE : selected by z flags
						inst->instruction_code = INSTCODE_FIST;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:	// FISTP / FCMOVNU : selected by p flags
						inst->instruction_code = INSTCODE_FISTP;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					case 4:	//	no opcode
						return NULL;
						break;

					case 5:	// FLD / FUCOMI : seleted by ozpcl flags
						inst->instruction_code = INSTCODE_FLD;
						modModRMParams(byte, endbyte, inst, 5);
						break;

					case 6:	// FCOMI
						inst->instruction_code = INSTCODE_FCOMI;
						modModRMParams(byte, endbyte, inst, 6);
						break;
						
					case 7:	// FSTP
						inst->instruction_code = INSTCODE_FSTP;
						modModRMParams(byte, endbyte, inst, 7);
						break;
						
					default:
						return NULL;
						break;
				}
			}
			break;

		case 0xDC:
			inst->instruction_type = INSTTYPE_FLOAT;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:	// FADD
					inst->instruction_code = INSTCODE_FADD;
					modModRMParams(byte, endbyte, inst, 0);
					break;

				case 1:	// FCOM
					inst->instruction_code = INSTCODE_FCOM;
					modModRMParams(byte, endbyte, inst, 1);
					break;

				case 2:	// FCOMP
					inst->instruction_code = INSTCODE_FCOMP;
					modModRMParams(byte, endbyte, inst, 2);
					break;

				case 3:	// FCOMP
					inst->instruction_code = INSTCODE_FCOMP;
					modModRMParams(byte, endbyte, inst, 3);
					break;

				case 4:	// FSUB
					inst->instruction_code = INSTCODE_FSUB;
					modModRMParams(byte, endbyte, inst, 4);
					break;

				case 5:	// FSUB
					inst->instruction_code = INSTCODE_FSUB;
					modModRMParams(byte, endbyte, inst, 5);
					break;

				case 6:	// FDIV
					inst->instruction_code = INSTCODE_FDIV;
					modModRMParams(byte, endbyte, inst, 6);
					break;

				case 7:	// FDIV
					inst->instruction_code = INSTCODE_FDIV;
					modModRMParams(byte, endbyte, inst, 7);
					break;

				default:
					return NULL;
					break;

			}
			break;

		case 0xDD:
			inst->instruction_type = INSTTYPE_FLOAT;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			if(**byte == 0xE1)	// FUCOM
			{
				inst->instruction_code = INSTCODE_FUCOM;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 4);
			}
			else if(**byte == 0xE9)	// FUCOMP
			{
				inst->instruction_code = INSTCODE_FUCOMP;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 5);
			}
			else
			{
				switch(modrm_reg(**byte))
				{
					case 0:	// FLD / FFREE : selected by register value
						inst->instruction_code = INSTCODE_FLD;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:	// FXCH4
						inst->instruction_code = INSTCODE_FXCH;
						modModRMParams(byte, endbyte, inst, 1);
						break;

					case 2:	// FST
						inst->instruction_code = INSTCODE_FST;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:	// FSTP
						inst->instruction_code = INSTCODE_FSTP;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					case 4:	// FRSTOR / FUCOM
						inst->instruction_code = INSTCODE_FRSTOR;
						modModRMParams(byte, endbyte, inst, 4);
						break;

					case 5:	// FUCOMP
						inst->instruction_code = INSTCODE_FUCOMP;
						modModRMParams(byte, endbyte, inst, 5);
						break;

					case 6:	// FNSAVE / FSAVE
						inst->instruction_code = INSTCODE_FNSAVE;
						modModRMParams(byte, endbyte, inst, 6);
						break;

					case 7:	// FNSTSW / FSTSW
						inst->instruction_code = INSTCODE_FNSTWSW;
						modModRMParams(byte, endbyte, inst, 7);
						break;

					default:
						return NULL;
						break;
				}
			}
			break;

		case 0xDE:
			inst->instruction_type = INSTTYPE_FLOAT;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			if(**byte == 0xC1)	// FADDP
			{
				inst->instruction_code = INSTCODE_FADDP;
				modOpcode(byte, endbyte, inst, 1);
			}
			else if(**byte == 0xC9)	// FMULP
			{
				inst->instruction_code = INSTCODE_FMULP;
				modOpcode(byte, endbyte, inst, 1);
			}
			else if(**byte == 0xD9)	// FCOMPP
			{
				inst->instruction_code = INSTCODE_FCOMPP;
				modOpcode(byte, endbyte, inst, 1);
			}
			else if(**byte == 0xE1)	// FSUBRP
			{
				inst->instruction_code = INSTCODE_FSUBRP;
				modOpcode(byte, endbyte, inst, 1);
			}
			else if(**byte == 0xE9)	// FSUBP
			{
				inst->instruction_code = INSTCODE_FSUBP;
				modOpcode(byte, endbyte, inst, 1);
			}
			else if(**byte == 0xF1)	// FDIVRP
			{
				inst->instruction_code = INSTCODE_FDIVRP;
				modOpcode(byte, endbyte, inst, 1);
			}
			else if(**byte == 0xF9)	// FDIVP
			{
				inst->instruction_code = INSTCODE_FDIVP;
				modOpcode(byte, endbyte, inst, 1);
			}
			else
			{
				switch(modrm_reg(**byte))
				{
					case 0:	// FIADD / FADDP
						inst->instruction_code = INSTCODE_FIADD;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:	// FIMUL / FMULP
						inst->instruction_code = INSTCODE_FIMUL;
						modModRMParams(byte, endbyte, inst, 1);
						break;

					case 2:	// FICOM / FCOMP5
						inst->instruction_code = INSTCODE_FICOM;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:	// FICOMP
						inst->instruction_code = INSTCODE_FICOMP;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					case 4:	// FISUB / FSUBRP
						inst->instruction_code = INSTCODE_FISUB;
						modModRMParams(byte, endbyte, inst, 4);
						break;

					case 5:	// FISUBR / FSUBP
						inst->instruction_code = INSTCODE_FISUBR;
						modModRMParams(byte, endbyte, inst, 5);
						break;

					case 6:	// FIDIV / FDIVRP
						inst->instruction_code = INSTCODE_FIDIV;
						modModRMParams(byte, endbyte, inst, 6);
						break;

					case 7:	// FIDIVR / FDIVP
						inst->instruction_code = INSTCODE_FIDIVR;
						modModRMParams(byte, endbyte, inst, 7);
						break;

					default:
						return NULL;
						break;
				}
			}
			break;

		case 0xDF:
			inst->instruction_type = INSTTYPE_FLOAT;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			if(**byte == 0xE0)
			{
				if(isPrefix(inst, 0x9B))	// FSTSW
				{
					inst->instruction_code = INSTCODE_FSTSW;
					modOpcode(byte, endbyte, inst, 1);
				}
				else	// FNSTSW
				{
					inst->instruction_code = INSTCODE_FNSTSW;
					modOpcode(byte, endbyte, inst, 1);
				}
			}
			else
			{
				switch(modrm_reg(**byte))
				{
					case 0:	// FILD / FFREEP
						inst->instruction_code = INSTCODE_FILD;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:	// FISTTP / FXCH7
						inst->instruction_code = INSTCODE_FISTTP;
						modModRMParams(byte, endbyte, inst, 1);
						break;

					case 2:	// FIST / FSTP8
						inst->instruction_code = INSTCODE_FIST;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:	// FISTP / FSTP9
						inst->instruction_code = INSTCODE_FISTP;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					case 4:	// FBLD
						inst->instruction_code = INSTCODE_FBLD;
						modModRMParams(byte, endbyte, inst, 4);
						break;

					case 5:	// FILD / FUCOMIP
						inst->instruction_code = INSTCODE_FILD;
						modModRMParams(byte, endbyte, inst, 5);
						break;

					case 6:	// FBSTP / FCOMIP
						inst->instruction_code = INSTCODE_FBSTP;
						modModRMParams(byte, endbyte, inst, 6);
						break;

					case 7:	// FISTP
						inst->instruction_code = INSTCODE_FISTP;
						modModRMParams(byte, endbyte, inst, 7);
						break;

					default:
						return NULL;
						break;
				}
			}
			break;

		// LOOPNE(f64) / LOOPNZ(f64)
		case 0xE0:
			inst->instruction_type = INSTTYPE_BRANCH;
			inst->instruction_code = INSTCODE_LOOPNE;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, BYTE);
			break;

		// LOOPE(f64) / LOOPZ(f64)
		case 0xE1:
			inst->instruction_type = INSTTYPE_BRANCH;
			inst->instruction_code = INSTCODE_LOOPE;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, BYTE);
			break;

		// LOOP(f64)
		case 0xE2:
			inst->instruction_type = INSTTYPE_BRANCH;
			inst->instruction_code = INSTCODE_LOOP;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, BYTE);
			break;

		// JrCXZ(f64)
		case 0xE3:
			inst->instruction_type = INSTTYPE_BRANCH;
			inst->instruction_code = INSTCODE_JRCXZ;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, BYTE);
			break;

		// IN
		case 0xE4:
		case 0xE5:
			inst->instruction_type = INSTTYPE_IO;
			inst->instruction_code = INSTCODE_IN;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		// OUT
		case 0xE6:
		case 0xE7:
			inst->instruction_type = INSTTYPE_IO;
			inst->instruction_code = INSTCODE_OUT;
			modOpcode(byte, endbyte, inst, 1);
			modImmediate(byte, endbyte, inst, BYTE);
			break;

		// CALL(f64)
		case 0xE8:
			inst->instruction_type = INSTTYPE_CALL;
			inst->instruction_code = INSTCODE_CALL;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, WORD);
			break;

		// JMP
		case 0xE9:	// jmp relative
			inst->instruction_type = INSTTYPE_JCC;
			inst->instruction_code = INSTCODE_JMP;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, WORD);
			break;

		case 0xEA:	// jmp absolute
			inst->instruction_type = INSTTYPE_JMP;
			inst->instruction_code = INSTCODE_JMP;
			modOpcode(byte, endbyte, inst, 1);
			break;

		case 0xEB:	// jmp relative
			inst->instruction_type = INSTTYPE_JMP;
			inst->instruction_code = INSTCODE_JMP;
			modOpcode(byte, endbyte, inst, 1);
			modDisplacement(byte, endbyte, inst, BYTE);
			break;

		// IN
		case 0xEC:
		case 0xED:
			inst->instruction_type = INSTTYPE_IO;
			inst->instruction_code = INSTCODE_IN;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// OUT
		case 0xEE:
		case 0xEF:
			inst->instruction_type = INSTTYPE_IO;
			inst->instruction_code = INSTCODE_OUT;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// LOCK (prefix)
		case 0xF0:
		// REPNE (prefix)
		case 0xF2:
		// REP / REPE (prefix)
		case 0xF3:
			modPrefix(byte, endbyte, inst);
			return create_inst(byte, endbyte, inst);
			break;

		// HALT
		case 0xF4:
			inst->instruction_type = INSTTYPE_CONTROL;
			inst->instruction_code = INSTCODE_HALT;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// CMC
		case 0xF5:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_CMC;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// Unary Grp 3
		case 0xF6:
			modOpcode(byte, endbyte, inst, 1);
			
			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:	// TEST
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_TEST;
					modModRMParams(byte, endbyte, inst, 0);
					modImmediate(byte, endbyte, inst, BYTE);
					break;
				case 1: 
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_TEST;
					modModRMParams(byte, endbyte, inst, 1);
					modImmediate(byte, endbyte, inst, BYTE);
					break;

				case 2:	// NOT
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_NOT;
					modModRMParams(byte, endbyte, inst, 2);
					break;

				case 3:	// NEG
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_NEG;
					modModRMParams(byte, endbyte, inst, 3);
					break;

				case 4:	// MUL
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_MUL;
					modModRMParams(byte, endbyte, inst, 4);
					break;

				case 5:	// IMUL
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_IMUL;
					modModRMParams(byte, endbyte, inst, 5);
					break;

				case 6:	// DIV
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_DIV;
					modModRMParams(byte, endbyte, inst, 6);
					break;

				case 7:	// IDIV
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_IDIV;
					modModRMParams(byte, endbyte, inst, 7);
					break;

				default:
					return NULL;
					break;
			}
			break;

		case 0xF7:
			modOpcode(byte, endbyte, inst, 1);
			
			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:	// TEST
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_TEST;
					modModRMParams(byte, endbyte, inst, 0);
					modImmediate(byte, endbyte, inst, WORD);
					break;
				case 1:
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_TEST;
					modModRMParams(byte, endbyte, inst, 1);
					modImmediate(byte, endbyte, inst, WORD);
					break;

				case 2:	// NOT
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_NOT;
					modModRMParams(byte, endbyte, inst, 2);
					break;

				case 3:	// NEG
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_NEG;
					modModRMParams(byte, endbyte, inst, 3);
					break;

				case 4:	// MUL
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_MUL;
					modModRMParams(byte, endbyte, inst, 4);
					break;

				case 5:	// IMUL
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_IMUL;
					modModRMParams(byte, endbyte, inst, 5);
					break;

				case 6:	// DIV
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_DIV;
					modModRMParams(byte, endbyte, inst, 6);
					break;

				case 7:	// IDIV
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_IDIV;
					modModRMParams(byte, endbyte, inst, 7);
					break;

				default:
					return NULL;
					break;
			}
			break;

		// CLC
		case 0xF8:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_CLC;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// STC:
		case 0xF9:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_STC;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// CLI
		case 0xFA:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_CLI;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// STI:
		case 0xFB:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_STI;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// CLD
		case 0xFC:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_CLD;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// STD
		case 0xFD:
			inst->instruction_type = INSTTYPE_NORMAL;
			inst->instruction_code = INSTCODE_STD;
			modOpcode(byte, endbyte, inst, 1);
			break;

		// INC / DEC Grp 4
		case 0xFE:
			inst->instruction_type = INSTTYPE_NORMAL;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:	// INC
					inst->instruction_code = INSTCODE_INC;
					modModRMParams(byte, endbyte, inst, 0);
					break;
				
				case 1:	// DEC
					inst->instruction_code = INSTCODE_DEC;
					modModRMParams(byte, endbyte, inst, 1);
					break;

				default:
					return NULL;
					break;
			}
			break;

		// INC / DEC Grp 5
		case 0xFF:
			inst->instruction_type = INSTTYPE_NORMAL;
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			switch(modrm_reg(**byte))
			{
				case 0:	// INC
					inst->instruction_code = INSTCODE_INC;
					modModRMParams(byte, endbyte, inst, 0);
					break;
				
				case 1:	// DEC
					inst->instruction_code = INSTCODE_DEC;
					modModRMParams(byte, endbyte, inst, 1);
					break;

				case 2:	// CALL
					inst->instruction_type = INSTTYPE_CALL;
					inst->instruction_code = INSTCODE_CALL;
					modModRMParams(byte, endbyte, inst, 2);
					break;

				case 3:	// CALL
					inst->instruction_type = INSTTYPE_CALL;
					inst->instruction_code = INSTCODE_CALL;
					modModRMParams(byte, endbyte, inst, 3);
					break;

				case 4:	// JMP
					inst->instruction_type = INSTTYPE_JMP;
					inst->instruction_code = INSTCODE_JMP;
					modModRMParams(byte, endbyte, inst, 4);
					break;

				case 5:	// JMP
					inst->instruction_type = INSTTYPE_JMP;
					inst->instruction_code = INSTCODE_JMP;
					modModRMParams(byte, endbyte, inst, 5);
					break;

				case 6:	// PUSH
					inst->instruction_type = INSTTYPE_NORMAL;
					inst->instruction_code = INSTCODE_PUSH;
					modModRMParams(byte, endbyte, inst, 6);
					inst->modrm_size = 2;
					break;

				default:
					return NULL;
					break;
			}
			break;


		/* end of one-byte opcodes */

		/* two byte opcodes */
		/* first byte is 0x0F */
		case 0x0F:
			modOpcode(byte, endbyte, inst, 1);

			if(*byte > endbyte)	// overflow
				return NULL;

			switch(**byte)
			{
			// Grp 6
			case 0x00:
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte) // overflow
					return NULL;

				
				switch(modrm_reg(**byte))
				{
					case 0:
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_SLDT;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_STR;
						modModRMParams(byte, endbyte, inst, 1);
						break;

					case 2:
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_LLDT;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_LTR;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					case 4:
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_VERR;
						modModRMParams(byte, endbyte, inst, 4);
						break;

					case 5:
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_VERW;
						modModRMParams(byte, endbyte, inst, 5);
						break;

					default:
						return NULL;
						break;
				}
				break;

			// Grp 7
			case 0x01:
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)	// overflow
					return NULL;

				if(**byte == 0xC8)	// MONITOR
				{
					modOpcode(byte, endbyte, inst, 1);
					if(modrm_reg(**byte) != 1)
						return NULL;
					modModRMParams(byte, endbyte, inst, 1);
				}
				else if(**byte == 0xC9) // MWAIT
				{
					modOpcode(byte, endbyte, inst, 1);
					if(modrm_reg(**byte) != 1)
						return NULL;
					modModRMParams(byte, endbyte, inst, 1);
				}
				else
				{
					switch(modrm_reg(**byte))
					{
						case 0:
							inst->instruction_type = INSTTYPE_NORMAL;
							inst->instruction_code = INSTCODE_SGDT;
							modModRMParams(byte, endbyte, inst, 0);
							break;

						case 1:
							inst->instruction_type = INSTTYPE_NORMAL;
							inst->instruction_code = INSTCODE_SIDT;
							modModRMParams(byte, endbyte, inst, 1);
							break;

						case 2:
							inst->instruction_type = INSTTYPE_NORMAL;
							inst->instruction_code = INSTCODE_LGDT;
							modModRMParams(byte, endbyte, inst, 2);
							break;

						case 3:
							inst->instruction_type = INSTTYPE_NORMAL;
							inst->instruction_code = INSTCODE_LIDT;
							modModRMParams(byte, endbyte, inst, 3);
							break;

						case 4:
							inst->instruction_type = INSTTYPE_NORMAL;
							inst->instruction_code = INSTCODE_SMSW;
							modModRMParams(byte, endbyte, inst, 4);
							break;

						case 6:
							inst->instruction_type = INSTTYPE_NORMAL;
							inst->instruction_code = INSTCODE_LMSW;
							modModRMParams(byte, endbyte, inst, 6);
							break;

						case 7:
							inst->instruction_type = INSTTYPE_NORMAL;
							inst->instruction_code = INSTCODE_INVLPG;
							modModRMParams(byte, endbyte, inst, 7);
							break;

						default:
							return NULL;
							break;
					}
				}
				break;


			// LAR
			case 0x02:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_LAR;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// SYSCALL(o64)
			case 0x05:
				inst->instruction_type = INSTTYPE_JMP;
				inst->instruction_code = INSTCODE_SYSCALL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// CLTS
			case 0x06:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CLTS;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// SYSRET(o64)
			case 0x07:
				inst->instruction_type = INSTTYPE_JMP;
				inst->instruction_code = INSTCODE_SYSRET;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// INVD
			case 0x08:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_INVD;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// WBINVD
			case 0x09:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_WBINVD;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// 2-byte illegal opcodes
			case 0x0B:
				inst->instruction_type = INSTTYPE_UNDEFINE;
				inst->instruction_code = INSTCODE_UNDEFINE;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// NOP
			case 0x0D:
				inst->instruction_code = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_NOP;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// movups
			// movss
			// movupd
			// movsd
			case 0x10:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				if(isPrefix(inst, 0xF3))	// MOVSS
					inst->instruction_code = INSTCODE_MOVSS;
				else if(isPrefix(inst, 0x66))	// MOVUPD
					inst->instruction_code = INSTCODE_MOVUPD;
				else if(isPrefix(inst, 0xF2))	// MOVSD
					inst->instruction_code = INSTCODE_MOVSD;
				else
					inst->instruction_code = INSTCODE_MOVUPS;
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// movups
			// movss
			// movupd
			// movsd
			case 0x11:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				if(isPrefix(inst, 0xF3))	// MOVSS
					inst->instruction_code = INSTCODE_MOVSS;
				else if(isPrefix(inst, 0x66))	// MOVUPD
					inst->instruction_code = INSTCODE_MOVUPD;
				else if(isPrefix(inst, 0xF2))	// MOVSD
					inst->instruction_code = INSTCODE_MOVSD;
				else
					inst->instruction_code = INSTCODE_MOVUPS;
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// movlps
			// movlpd
			// movhlps
			// movddup
			// movsldup
			case 0x12:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				if(isPrefix(inst, 0x66))	// MOVLPD
					inst->instruction_code = INSTCODE_MOVLPD;
				else if(isPrefix(inst, 0xF2))	// MOVDDUP
					inst->instruction_code = INSTCODE_MOVDDUP;
				else if(isPrefix(inst, 0xF3))	// MOVSLDUP
					inst->instruction_code = INSTCODE_MOVSLDUP;
				else
					inst->instruction_code = INSTCODE_MOVHLPS;
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// movlps
			// movlpd
			case 0x13:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				if(isPrefix(inst, 0x66))	// MOVLPD
					inst->instruction_code = INSTCODE_MOVLPD;
				else				// MOVLPS
					inst->instruction_code = INSTCODE_MOVLPS;
				break;

			// unpcklps
			// unpcklpd
			case 0x14:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				if(isPrefix(inst, 0x66))	// UNPCKLPD
					inst->instruction_code = INSTCODE_UNPCKLPD;
				else				// UNPCKLPS
					inst->instruction_code = INSTCODE_UNPCKLPS;
				break;

			// unpckhps
			// unpckhpd
			case 0x15:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				if(isPrefix(inst, 0x66))	// MOVLHPS
					inst->instruction_code = INSTCODE_MOVLHPS;
				else				// MOVHPS
					inst->instruction_code = INSTCODE_MOVHPS;
				break;

			// movhps
			// movhpd
			// movhlps
			// movshdup
			case 0x16:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				if(isPrefix(inst, 0x66))	// MOVHPD
					inst->instruction_code = INSTCODE_MOVHPD;
				else if(isPrefix(inst, 0xF3))	// MOVSHDUP
					inst->instruction_code = INSTCODE_MOVSHDUP;
				else						// MOVLHPS
					inst->instruction_code = INSTCODE_MOVLHPS;
				break;

			// movhps
			// movhpd
			case 0x17:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				if(isPrefix(inst, 0x66))	// MOVHPD
					inst->instruction_code = INSTCODE_MOVHPD;
				else						// MOVHPS
					inst->instruction_code = INSTCODE_MOVHPS;
				break;

			// Prefetch(Grp 16)
			case 0x18:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)		// overflow
					return NULL;

				switch(modrm_reg(**byte))
				{
					case 0:	// PREFETCHNTA
						inst->instruction_code = INSTCODE_PREFETCHNTA;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:	// PREFETCH0
						inst->instruction_code = INSTCODE_PREFETCH0;
						modModRMParams(byte, endbyte, inst, 1);
						break;

					case 2:	// PREFETCH1
						inst->instruction_code = INSTCODE_PREFETCH1;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:	// PREFETCH2
						inst->instruction_code = INSTCODE_PREFETCH2;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					default:
						return NULL;
						break;
				}
				break;

			// NOP
			case 0x1F:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_NOP;
				modOpcode(byte, endbyte, inst, 1);
				break;


			// MOV
			case 0x20:
			case 0x21:
			case 0x22:
			case 0x23:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_MOV;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// movaps
			// movapd
			case 0x28:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// MOVAPD
					inst->instruction_code = INSTCODE_MOVAPD;
				else						// MOVAPS
					inst->instruction_code = INSTCODE_MOVAPS;
				break;

			// movaps
			// movapd
			case 0x29:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// MOVAPD
					inst->instruction_code = INSTCODE_MOVAPD;
				else
					inst->instruction_code = INSTCODE_MOVAPS;
				break;

			// cvtpi2ps
			// cvtsi2ss
			// cvtpi2pd
			// cvtsi2sd
			case 0x2A:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// CVTSI2SS
					inst->instruction_code = INSTCODE_CVTSI2SS;
				else if(isPrefix(inst, 0x66))	// CVTPI2PD
					inst->instruction_code = INSTCODE_CVTPI2PD;
				else if(isPrefix(inst, 0xF2))	// CVTSI2SD
					inst->instruction_code = INSTCODE_CVTSI2SD;
				else						// CVTPI2PS
					inst->instruction_code = INSTCODE_CVTPI2PS;
				break;

			// movntps
			// movntpd
			case 0x2B:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// MOVNTPD
					inst->instruction_code = INSTCODE_MOVNTPD;
				else	// MOVNTPS
					inst->instruction_code = INSTCODE_MOVNTPS;
				break;

			// cvttps2pi
			// cvttss2si
			// cvttpd2pi
			// cvttsd2si
			case 0x2C:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// CVTTSS2SI
					inst->instruction_code = INSTCODE_CVTTSS2SI;
				else if(isPrefix(inst, 0x66))	// CVTTPD2PI
					inst->instruction_code = INSTCODE_CVTTPD2PI;
				else if(isPrefix(inst, 0xF2))	// CVTTSD2SI
					inst->instruction_code = INSTCODE_CVTTSD2SI;
				else
					inst->instruction_code = INSTCODE_CVTTPS2PI;
				break;

			// cvtps2pi
			// cvtss2si
			// cvtpd2pi
			// cvtsd2si
			case 0x2D:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// CVTPS2PI
					inst->instruction_code = INSTCODE_CVTPS2PI;
				else if(isPrefix(inst, 0x66))	// CVTPD2PI
					inst->instruction_code = INSTCODE_CVTPD2PI;
				else if(isPrefix(inst, 0xF2))	// CVTSD2SI
					inst->instruction_code = INSTCODE_CVTSD2SI;
				break;

			// ucomiss
			// ucomisd
			case 0x2E:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// UCOMISD
					inst->instruction_code = INSTCODE_UCOMISD;
				else	// UCOMISS
					inst->instruction_code = INSTCODE_UCOMISS;
				break;

			// comiss
			// comisd
			case 0x2F:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// COMISD
					inst->instruction_code = INSTCODE_COMISD;
				else	// COMISS
					inst->instruction_code = INSTCODE_COMISS;
				break;

			// WRMSR
			case 0x30:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_WRMSR;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// RDTSC
			case 0x31:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_RDTSC;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// RDMSR
			case 0x32:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_RDMSR;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// RDPMC
			case 0x33:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_RDPMC;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// SYSENTER
			case 0x34:
				inst->instruction_type = INSTTYPE_JMP;
				inst->instruction_code = INSTCODE_SYSENTER;
				modOpcode(byte, endbyte, inst, 1);
				break;


			// SYSEXIT
			case 0x35:
				inst->instruction_type = INSTTYPE_JMP;
				inst->instruction_code = INSTCODE_SYSEXIT;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// GETSEC	:: ?????????????????????
			// case 0x37:

			// 3-byte escape
			case 0x38:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)	// overflow
					return NULL;

				switch(**byte)
				{
					case 0x00:	// PSHUFB
						inst->instruction_code = INSTCODE_PSHUFB;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x01:	// PHADDW
						inst->instruction_code = INSTCODE_PHADDW;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x02:	// PHADDD
						inst->instruction_code = INSTCODE_PHADDD;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x03:	// PHADDSW
						inst->instruction_code = INSTCODE_PHADDSW;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x04:	// PMADDUBSW
						inst->instruction_code = INSTCODE_PMADDUBSW;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x05:	// PHSUBW
						inst->instruction_code = INSTCODE_PHSUBW;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x06:	// PHSUBD
						inst->instruction_code = INSTCODE_PHSUBD;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x07:	// PHSUBSW
						inst->instruction_code = INSTCODE_PHSUBSW;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x08:	// PSIGNB
						inst->instruction_code = INSTCODE_PSIGNB;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x09:	// PSIGNW
						inst->instruction_code = INSTCODE_PSIGNW;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x0A:	// PSIGND
						inst->instruction_code = INSTCODE_PSIGND;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x0B:	// PMULHRSW
						inst->instruction_code = INSTCODE_PMULHRSW;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x1C:	// PABSB
						inst->instruction_code = INSTCODE_PABSB;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x1D:	// PABSW
						inst->instruction_code = INSTCODE_PABSW;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					case 0x1E:	// PABSD
						inst->instruction_code = INSTCODE_PABSD;
						modOpcode(byte, endbyte, inst, 1);
						modModRMParams(byte, endbyte, inst, 9);
						break;

					default:
						return NULL;
						break;
				}
				break;

			// 3-byte escape
			case 0x3A:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)	// overflow
					return NULL;

				if(**byte != 0x0F)
					return NULL;

				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// CMOV - Conditional move
			case 0x40:	// CMOVO
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVO;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x41:	// CMOVNO
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVNO;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x42:	// CMOVB / CMOVNAE / CMOVC
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x43:	// CMOVNB / CMOVAE / CMOVNC
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVNB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x44:	// CMOVZ / CMOVE
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVZ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x45:	// CMOVNZ / CMOVNE
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVNZ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x46:	// CMOVBE / CMOVNA
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVBE;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x47:	// CMOVNBE / CMOVA
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVNBE;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x48:	// CMOVS
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVS;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x49:	// CMOVNS
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVNS;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x4A:	// CMOVP / CMOVPE
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVP;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x4B:	// CMOVNP / CMONPO
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVNP;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x4C:	// CMOVL / CMOVNGE
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x4D:	// CMOVNL / CMOVGE
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVNL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x4E:	// CMOVLE / CMOVNG
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVLE;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				
			case 0x4F:	// CMOVNLE / CMOVG
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMOVNLE;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;
				

			// movmskps
			// movmskpd
			case 0x50:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// MOVMSKPD
					inst->instruction_code = INSTCODE_MOVMSKPD;
				else	// MOVMSKPS
					inst->instruction_code = INSTCODE_MOVMSKPS;
				break;

			// sqrtps
			// sqrtss
			// sqrtpd
			// sqrtsd
			case 0x51:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// SQRTSS
					inst->instruction_code = INSTCODE_SQRTSS;
				else if(isPrefix(inst, 0x66))	// SQRTPD
					inst->instruction_code = INSTCODE_SQRTPD;
				else if(isPrefix(inst, 0xF2))	// SQRTSD
					inst->instruction_code = INSTCODE_SQRTSD;
				else	// SQRTPS
					inst->instruction_code = INSTCODE_SQRTPS;
				break;

			// rsqrtps
			// rsqrtss
			case 0x52:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// RSQRTSS
					inst->instruction_code = INSTCODE_RSQRTSS;
				else	// RSQRTPS
					inst->instruction_code = INSTCODE_RSQRTPS;
				break;

			// rcpps
			// rcpss
			case 0x53:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// RCPSS
					inst->instruction_code = INSTCODE_RCPSS;
				else	// RPCPS
					inst->instruction_code = INSTCODE_RPCPS;
				break;

			// andps
			// andpd
			case 0x54:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// ANDPD
					inst->instruction_code = INSTCODE_ANDPD;
				else	// ANDPS
					inst->instruction_code = INSTCODE_ANDPS;
				break;

			// andnps
			// andnpd
			case 0x55:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// ANDNPD
					inst->instruction_code = INSTCODE_ANDNPD;
				else	// ANDNPS
					inst->instruction_code = INSTCODE_ANDNPS;
				break;

			// orps
			// orpd
			case 0x56:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// ORPD
					inst->instruction_code = INSTCODE_ORPD;
				else	// ORPS
					inst->instruction_code = INSTCODE_ORPS;
				break;

			// xorps
			// xorpd
			case 0x57:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// XORPD
					inst->instruction_code = INSTCODE_XORPD;
				else	// XORPS
					inst->instruction_code = INSTCODE_XORPS;
				break;

			// addps
			// addss
			// addpd
			// addsd
			case 0x58:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// ADDSS
					inst->instruction_code = INSTCODE_ADDSS;
				else if(isPrefix(inst, 0x66))	// ADDPD
					inst->instruction_code = INSTCODE_ADDPD;
				else if(isPrefix(inst, 0xF2))	// ADDSD
					inst->instruction_code = INSTCODE_ADDSD;
				else	// ADDPS
					inst->instruction_code = INSTCODE_ADDPS;
				break;

			// mulps
			// mulss
			// mulpd
			// mulsd
			case 0x59:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// MULSS
					inst->instruction_code = INSTCODE_MULSS;
				else if(isPrefix(inst, 0x66))	// MULPD
					inst->instruction_code = INSTCODE_MULPD;
				else if(isPrefix(inst, 0xF2))	// MULSD
					inst->instruction_code = INSTCODE_MULSD;
				else	// MULPS
					inst->instruction_code = INSTCODE_MULPS;
				break;

			// cvtps2pd
			// cvtss2sd
			// cvtpd2ps
			// cvtsd2ss
			case 0x5A:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// CVTPD2PS
					inst->instruction_code = INSTCODE_CVTPD2PS;
				else if(isPrefix(inst, 0xF3))	// CVTSS2SD
					inst->instruction_code = INSTCODE_CVTSS2SD;
				else if(isPrefix(inst, 0xF2))	// CVTSD2SS
					inst->instruction_code = INSTCODE_CVTSD2SS;
				else	// CVTPD2PS
					inst->instruction_code = INSTCODE_CVTPD2PS;
				break;

			// cvtdq2ps
			// cvtps2dq
			// cvttps2dq
			case 0x5B:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// CVTPS2DQ
					inst->instruction_code = INSTCODE_CVTPS2DQ;
				else if(isPrefix(inst, 0xF3))	// CVTTPS2DQ
					inst->instruction_code = INSTCODE_CVTTPS2DQ;
				else	// CVTDQ2PS
					inst->instruction_code = INSTCODE_CVTDQ2PS;
				break;

			// subps
			// subss
			// subpd
			// subsd
			case 0x5C:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// SUBSS
					inst->instruction_code = INSTCODE_SUBSS;
				else if(isPrefix(inst, 0x66))	// SUBPD
					inst->instruction_code = INSTCODE_SUBPD;
				else if(isPrefix(inst, 0xF2))	// SUBSD
					inst->instruction_code = INSTCODE_SUBSD;
				else	// SUBPS
					inst->instruction_code = INSTCODE_SUBPS;
				break;

			// minps
			// minss
			// minpd
			// minsd
			case 0x5D:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// MINSS
					inst->instruction_code = INSTCODE_MINSS;
				else if(isPrefix(inst, 0x66))	// MINPD
					inst->instruction_code = INSTCODE_MINPD;
				else if(isPrefix(inst, 0xF2))	// MINSD
					inst->instruction_code = INSTCODE_MINSD;
				else	// MINPS
					inst->instruction_code = INSTCODE_MINPS;
				break;
				

			// divps
			// divss
			// divpd
			// divsd
			case 0x5E:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// DIVSS
					inst->instruction_code = INSTCODE_DIVSS;
				else if(isPrefix(inst, 0x66))	// DIVPD
					inst->instruction_code = INSTCODE_DIVPD;
				else if(isPrefix(inst, 0xF2))	// DIVSD
					inst->instruction_code = INSTCODE_DIVSD;
				else	// DIVPS
					inst->instruction_code = INSTCODE_DIVPS;
				break;

			// maxps
			// maxss
			// maxpd
			// maxsd
			case 0x5F:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0xF3))	// MAXSS
					inst->instruction_code = INSTCODE_MAXSS;
				else if(isPrefix(inst, 0x66))	// MAXPD
					inst->instruction_code = INSTCODE_MAXPD;
				else if(isPrefix(inst, 0xF2))	// MAXSD
					inst->instruction_code = INSTCODE_MAXSD;
				else	// MAXPS
					inst->instruction_code = INSTCODE_MAXPS;
				break;

			// punpcklbw
			case 0x60:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUNPCKLBW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// punpcklwd
			case 0x61:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUNPCKLWD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// punpckldq
			case 0x62:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUNPCKLDQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// packsswb
			case 0x63:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PACKSSWB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// pcmpgtb
			case 0x64:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PCMPGTB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// pcmpgtw
			case 0x65:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PCMPGTW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// pcmpgtd
			case 0x66:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PCMPGTD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// packuswb
			case 0x67:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PACKUSWB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			//punpckhbw
			case 0x68:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUNPCKHBW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// punpckhwd
			case 0x69:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUNPCKHWD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// punpckhdq
			case 0x6A:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUNPCKHDQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// packssdw
			case 0x6B:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PACKSSDW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// punpcklqdq
			case 0x6C:
				if(!isPrefix(inst, 0x66))	// require LOCK prefix
					return NULL;
					break;
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUNPCKLQDQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// punpckhqdq
			case 0x6D:
				if(!isPrefix(inst, 0x66))	// require LOCK prefix
					return NULL;
					break;

				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUNPCKHQDQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// movd/q
			case 0x6E:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_MOVD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;				

			// movq
			// movdqa
			// movdqu
			case 0x6F:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// MOVDQA
					inst->instruction_code = INSTCODE_MOVDQA;
				else if(isPrefix(inst, 0xF3))	// MOVDQU
					inst->instruction_code = INSTCODE_MOVDQU;
				else	// MOVQ
					inst->instruction_code = INSTCODE_MOVQ;
				break;

			// pshufw
			// pshufd
			// pshufhw
			// pshuflw
			case 0x70:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				modImmediate(byte, endbyte, inst, BYTE);

				if(isPrefix(inst, 0xF2))	// PSHUFLW
					inst->instruction_code = INSTCODE_PSHUFLW;
				else if(isPrefix(inst, 0xF3))	// PSHUFHW
					inst->instruction_code = INSTCODE_PSHUFHW;
				else if(isPrefix(inst, 0x66))	// PSHUFD
					inst->instruction_code = INSTCODE_PSHUFD;
				else	// PSHUFW
					inst->instruction_code = INSTCODE_PSHUFW;
				break;


			// Grp 12
			case 0x71:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)	// overflow
					return NULL;

				switch(modrm_reg(**byte))
				{
					case 2:	// PSRLW
						inst->instruction_code = INSTCODE_PSRLW;
						modModRMParams(byte, endbyte, inst, 2);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					case 4:	// PSRAW
						inst->instruction_code = INSTCODE_PSRAW;
						modModRMParams(byte, endbyte, inst, 4);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					case 6:	// PSLLW
						inst->instruction_code = INSTCODE_PSLLW;
						modModRMParams(byte, endbyte, inst, 6);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					default:
						return NULL;
						break;
				}
				break;

			// Grp 13
			case 0x72:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)	// overflow
					return NULL;

				switch(modrm_reg(**byte))
				{
					case 2:	// PSRLD
						inst->instruction_code = INSTCODE_PSRLD;
						modModRMParams(byte, endbyte, inst, 2);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					case 4:	// PSRAD
						inst->instruction_code = INSTCODE_PSRAD;
						modModRMParams(byte, endbyte, inst, 4);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					case 6:	// PSLLD
						inst->instruction_code = INSTCODE_PSLLD;
						modModRMParams(byte, endbyte, inst, 6);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					default:
						return NULL;
						break;
				}
				break;

			// Grp 14
			case 0x73:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)	// overflow
					return NULL;

				switch(modrm_reg(**byte))
				{
					case 2:	// PSRLQ
						inst->instruction_code = INSTCODE_PSRLQ;
						modModRMParams(byte, endbyte, inst, 2);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					case 3:	// PSRLDQ
						if(!isPrefix(inst, 0x66))	// require LOCK prefix
							return NULL;
						inst->instruction_code = INSTCODE_PSRLDQ;
						modModRMParams(byte, endbyte, inst, 3);
						modImmediate(byte, endbyte, inst, BYTE);
						break;
						
					case 6:	// PSLLQ
						inst->instruction_code = INSTCODE_PSLLQ;
						modModRMParams(byte, endbyte, inst, 6);
						modImmediate(byte, endbyte, inst, BYTE);
						break;
						
					case 7:	// PSLLDQ
						if(!isPrefix(inst, 0x66))	// require LOCK prefix
							return NULL;
						inst->instruction_code = INSTCODE_PSLLDQ;
						modModRMParams(byte, endbyte, inst, 7);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					default:
						return NULL;
						break;
				}
				break;

			// pcmpeqb
			case 0x74:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PCMPEQB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pcmpeqw
			case 0x75:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PCMPEQW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// pcmpeqd
			case 0x76:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PCMPEQD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// emms
			case 0x77:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_EMMS;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// VMREAD	: ??????????????????????????????????
			//case 0x78:
				

			// VMWRITE	: ??????????????????????????????????
			//case 0x79:

			// haddps
			// haddpd
			case 0x7C:
				inst->instruction_type = INSTTYPE_NORMAL;

				if(isPrefix(inst, 0x66))	// HADDPD
					inst->instruction_code = INSTCODE_HADDPD;
				else if(isPrefix(inst, 0xF2))	// HADDPS
					inst->instruction_code = INSTCODE_HADDPS;
				else
					return NULL;

				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// hsubps
			// hsubpd
			case 0x7D:
				inst->instruction_type = INSTTYPE_NORMAL;

				if(!isPrefix(inst, 0x66))	// require LOCK prefix
					return NULL;
				inst->instruction_code = INSTCODE_HSUBPD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// movd/q
			// movd/q
			// movq
			case 0x7E:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_MOVD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// movq
			// movdqa
			// movduq
			case 0x7F:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);

				if(isPrefix(inst, 0x66))	// MOVDQA
					inst->instruction_code = INSTCODE_MOVDQA;
				else if(isPrefix(inst, 0xF3))	// MOVDQU
					inst->instruction_code = INSTCODE_MOVDQU;
				else	// MOVQ
					inst->instruction_code = INSTCODE_MOVQ;
				break;

			// JCC(f64)
			case 0x80:	// JO
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JO;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x81:	// JNO
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JNO;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x82:	// JB / JNAE / JC
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JB;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x83:	// JNB / JAE / JNC
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JNB;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x84:	// JZ / JE
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JZ;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x85:	// JNZ / JNE
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JNZ;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x86:	// JBE / JNA
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JBE;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x87:	// JNBE / JA
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JNBE;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x88:	// JS
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JS;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x89:	// JNS
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JNS;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x8A:	// JP / JPE
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JP;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x8B:	// JNP / JPO
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JNP;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x8C:	// JL / JNGE
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JL;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x8D:	// JNL / JGE
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JNL;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x8E:	// JLE / JNG
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JLE;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			case 0x8F:	// JNLE / JG
				inst->instruction_type = INSTTYPE_JCC;
				inst->instruction_code = INSTCODE_JNLE;
				modOpcode(byte, endbyte, inst, 1);
				modDisplacement(byte, endbyte, inst, WORD);
				break;

			// SETcc
			case 0x90:	// SETO
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETO;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x91:	// SETNO
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETNO;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x92:	// SETB / SETNAE / SETC
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x93:	// SETNB / SETAE / SETNC
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETNB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x94:	// SETZ / SETE
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETZ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x95:	// SETNZ / SETNE
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETNZ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x96:	// SETBE / SETNA
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETBE;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x97:	// SETNBE / SETA
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETNBE;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x98:	// SETS
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETS;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x99:	// SETNS
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETNS;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x9A:	// SETP / SETPE
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETP;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x9B:	// SETNP / SETPO
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETNP;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x9C:	// SETL / SETNGE
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x9D:	// SETNL / SETGE
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETNL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x9E:	// SETLE / SETNG
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETLE;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			case 0x9F:	// SETNLE / SETG
				inst->instruction_type = INSTTYPE_SET;
				inst->instruction_code = INSTCODE_SETNLE;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 0);
				break;

			// PUSH(d64)
			case 0xA0:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUSH;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;

			// POP(d64)
			case 0xA1:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_POP;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;

			// CPUID
			case 0xA2:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CPUID;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// BT
			case 0xA3:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BT;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// SHLD
			case 0xA4:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_SHLD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				modImmediate(byte, endbyte, inst, BYTE);
				break;

			// SHLD
			case 0xA5:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_SHLD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// PUSH(d64)
			case 0xA6:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PUSH;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;

			// POP(d64)
			case 0xA7:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_POP;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;

			// RSM
			case 0xAA:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_RSM;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// BTS
			case 0xAB:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BTS;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// SHRD
			case 0xAC:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_SHRD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				modImmediate(byte, endbyte, inst, BYTE);
				break;

			case 0xAD:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_SHRD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// Grp 15
			case 0xAE:
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)	// overflow
					return NULL;

				switch(modrm_reg(**byte))
				{
					case 0:	// FXSAVE
						inst->instruction_type = INSTTYPE_FLOAT;
						inst->instruction_code = INSTCODE_FXSAVE;
						modModRMParams(byte, endbyte, inst, 0);
						break;

					case 1:	// FXRSTOR
						inst->instruction_type = INSTTYPE_FLOAT;
						inst->instruction_code = INSTCODE_FXRSTOR;
						modModRMParams(byte, endbyte, inst, 1);
						break;
						
					case 2:	// LDMXCSR
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_LDMXCSR;
						modModRMParams(byte, endbyte, inst, 2);
						break;

					case 3:	// STMXCSR
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_STMXCSR;
						modModRMParams(byte, endbyte, inst, 3);
						break;

					case 5:	// LFENCE
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_LFENCE;
						modModRMParams(byte, endbyte, inst, 5);
						break;

					case 6:	// MFENCE
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_MFENCE;
						modModRMParams(byte, endbyte, inst, 6);
						break;

					case 7:	// SFENCE
						inst->instruction_type = INSTTYPE_NORMAL;
						inst->instruction_code = INSTCODE_SFENCE;
						modModRMParams(byte, endbyte, inst, 7);
						break;

					default:
						return NULL;
						break;
				}
				break;

			// IMUL
			case 0xAF:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_IMUL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// CMPXCHG
			case 0xB0:
			case 0xB1:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMPXCHG;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// LSS
			case 0xB2:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_LSS;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// BTR
			case 0xB3:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BTR;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// LFS
			case 0xB4:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_LFS;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// LGS
			case 0xB5:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_LGS;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// MOVZX
			case 0xB6:
			case 0xB7:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_MOVZX;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// JMPE / POPCNT	: ??????????????????????????????????
			//case 0xB8:

			// Grp 10 / Invalid opcode
			case 0xB9:
				inst->instruction_type = INSTTYPE_UNDEFINE;
				inst->instruction_code = INSTCODE_UNDEFINE;
				modOpcode(byte, endbyte, inst, 1);
				break;

			// Grp 8
			case 0xBA:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)	// overflow
					return NULL;

				switch(modrm_reg(**byte))
				{
					case 4:	// BT
						inst->instruction_code = INSTCODE_BT;
						modModRMParams(byte, endbyte, inst, 4);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					case 5:	// BTS
						inst->instruction_code = INSTCODE_BTS;
						modModRMParams(byte, endbyte, inst, 5);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					case 6:	// BTR
						inst->instruction_code = INSTCODE_BTR;
						modModRMParams(byte, endbyte, inst, 6);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					case 7:	// BTC
						inst->instruction_code = INSTCODE_BTC;
						modModRMParams(byte, endbyte, inst, 7);
						modImmediate(byte, endbyte, inst, BYTE);
						break;

					default:
						return NULL;
						break;

				}
				break;

			// BTC
			case 0xBB:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BTC;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// BSF
			case 0xBC:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSF;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// BSR
			case 0xBD:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSR;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// MOVSX
			case 0xBE:
			case 0xBF:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_MOVSX;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// XADD
			case 0xC0:
			case 0xC1:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_XADD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// cmpps
			// cmpss
			// cmppd
			// cmpsd
			case 0xC2:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				modImmediate(byte, endbyte, inst, BYTE);

				if(isPrefix(inst, 0xF3))	// CMPSS
					inst->instruction_code = INSTCODE_CMPSS;
				else if(isPrefix(inst, 0x66))	// CMPPD
					inst->instruction_code = INSTCODE_CMPPD;
				else if(isPrefix(inst, 0xF2))	// CMPSD
					inst->instruction_code = INSTCODE_CMPSD;
				else	// CMPPS
					inst->instruction_code = INSTCODE_CMPPS;
				break;

			// movnti
			case 0xC3:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_MOVNTI;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pinsrw
			// pinsrw
			case 0xC4:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PINSRW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				modImmediate(byte, endbyte, inst, BYTE);
				break;

			// pextrw
			// pextrw
			case 0xC5:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PEXTRW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				modImmediate(byte, endbyte, inst, BYTE);
				break;

			// shufps
			// shufpd
			case 0xC6:
				inst->instruction_type = INSTTYPE_NORMAL;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				modImmediate(byte, endbyte, inst, BYTE);

				if(isPrefix(inst, 0x66))	// SHUFPD
					inst->instruction_code = INSTCODE_SHUFPD;
				else	// SHUFPS
					inst->instruction_code = INSTCODE_SHUFPS;
				break;

			// Grp 9
			case 0xC7:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_CMPXCHG;

				modOpcode(byte, endbyte, inst, 1);

				if(*byte > endbyte)	// overflow
					return NULL;

				switch(modrm_reg(**byte))
				{
					case 1:	// CMPXCHG8B
						modModRMParams(byte, endbyte, inst, 1);
						break;
					case 6:	// VMCLEAR
						modModRMParams(byte, endbyte, inst, 6);
						break;
					case 7:	// VMPTRST
						modModRMParams(byte, endbyte, inst, 7);
						break;
					default:
						return NULL;
				}
				break;

			// BSWAP
			case 0xC8:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSWAP;
				modOpcode(byte, endbyte, inst, 1);
				inst->opcode[inst->opcode_size - 1] = 0xC9;
				break;
			case 0xC9:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSWAP;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;
			case 0xCA:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSWAP;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;
			case 0xCB:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSWAP;
				modOpcode(byte, endbyte, inst, 1);
				inst->opcode[inst->opcode_size - 1] = 0xCA;
				break;
			case 0xCC:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSWAP;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;
			case 0xCD:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSWAP;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;
			case 0xCE:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSWAP;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;
			case 0xCF:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_BSWAP;
				modOpcode(byte, endbyte, inst, 1);
				inst->modrm_size = 2;
				break;

			// addsubps
			// addsubpd
			case 0xD0:
				inst->instruction_type = INSTTYPE_NORMAL;
				
				if(isPrefix(inst, 0x66))	// ADDSUBPD
					inst->instruction_code = INSTCODE_ADDSUBPD;
				else if(isPrefix(inst, 0xF2))	// ADDSUBPS
					inst->instruction_code = INSTCODE_ADDSUBPS;
				else
					return NULL;

				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psrlw
			case 0xD1:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSRLW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psrld
			case 0xD2:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSRLD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psrlq
			case 0xD3:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSRLQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// paddq
			case 0xD4:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PADDQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pmullw
			case 0xD5:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMULLW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// movq
			// mov2dq
			// movdq2q
			case 0xD6:
				inst->instruction_type = INSTTYPE_NORMAL;

				if(isPrefix(inst, 0xF3))	// MOVQ2DQ
					inst->instruction_code = INSTCODE_MOVQ2DQ;
				else if(isPrefix(inst, 0xF2))	// MOVDQ2Q
					inst->instruction_code = INSTCODE_MOVDQ2Q;
				else
					return NULL;

				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pmovmskb
			case 0xD7:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMOVMSKB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psubusb
			case 0xD8:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSUBUSB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psubusw
			case 0xD9:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSUBUSW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pminub
			case 0xDA:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMINUB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pand
			case 0xDB:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PAND;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// paddusb
			case 0xDC:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PADDUSB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// paddusw
			case 0xDD:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PADDUSW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pmaxub
			case 0xDE:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMAXUB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pandn
			case 0xDF:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PANDN;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pavgb
			case 0xE0:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PAVGB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psraw
			case 0xE1:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSRAW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psrad
			case 0xE2:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSRAD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pavgw
			case 0xE3:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PAVGW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pmulhuw
			case 0xE4:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMULHUW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pmulhw
			case 0xE5:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMULHW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// cvtpd2dq
			// cvttpd2dq
			// cvtdq2pd
			case 0xE6:
				inst->instruction_type = INSTTYPE_NORMAL;

				if(isPrefix(inst, 0xF2))	// CVTPD2DQ
					inst->instruction_code = INSTCODE_CVTPD2DQ;
				else if(isPrefix(inst, 0x66))	// CVTTPD2DQ
					inst->instruction_code = INSTCODE_CVTTPD2DQ;
				else if(isPrefix(inst, 0xF3))	// CVTDQ2PD
					inst->instruction_code = INSTCODE_CVTDQ2PD;
				else
					return NULL;

				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// movntq
			// movntdq
			case 0xE7:
				inst->instruction_type = INSTTYPE_NORMAL;

				if(isPrefix(inst, 0x66))	// MOVNTDQ
					inst->instruction_code = INSTCODE_MOVNTDQ;
				else	// MOVNTQ
					inst->instruction_code = INSTCODE_MOVNTQ;

				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psubsb
			case 0xE8:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSUBSB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psubsw
			case 0xE9:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSUBSW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pminsw
			case 0xEA:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMINSW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// por
			case 0xEB:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_POR;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// paddsb
			case 0xEC:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PADDSB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// paddsw
			case 0xED:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PADDSW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pmaxsw
			case 0xEE:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMAXSW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pxor
			case 0xEF:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PXOR;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// lddqu
			case 0xF0:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_LDDQU;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psllw
			case 0xF1:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSLLW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pslld
			case 0xF2:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSLLD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psllq
			case 0xF3:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSLLQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pmuludq
			case 0xF4:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMULUDQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// pmaddwd
			case 0xF5:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PMADDWD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psadbw
			case 0xF6:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSADBW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;


			// maskmovq
			// maskmovdqu
			case 0xF7:
				inst->instruction_type = INSTTYPE_NORMAL;

				if(isPrefix(inst, 0x66))	// MASKMOVDQU
					inst->instruction_code = INSTCODE_MASKMOVDQU;
				else	// MASKMOVQ
					inst->instruction_code = INSTCODE_MASKMOVQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psubb
			case 0xF8:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSUBB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psubw
			case 0xF9:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSUBW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psubd
			case 0xFA:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSUBD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// psubq
			case 0xFB:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PSUBQ;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// paddb
			case 0xFC:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PADDB;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// paddw
			case 0xFD:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PADDW;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// paddd
			case 0xFE:
				inst->instruction_type = INSTTYPE_NORMAL;
				inst->instruction_code = INSTCODE_PADDD;
				modOpcode(byte, endbyte, inst, 1);
				modModRMParams(byte, endbyte, inst, 9);
				break;

			// no case
			default:
				return NULL;
				break;

			}
		break;

	/*
		// three bytes opcodes
		// first byte is 0x0F 
		case 0x0F:
			(*byte)++;
			switch(**byte)
			{
			// second byte is 0x38
			case 0x38:
			(*byte)++;
				switch(**byte)
				{
				// pshufb
				// pshufb
				case 0x00:

				// phaddw
				// phaddw
				case 0x01:

				// phaddd
				// phaddd
				case 0x02:

				// phaddsw
				// phaddsw
				case 0x03:

				// pmaddubsw
				// pmaddusbw
				case 0x04:

				// phsubw
				// phsubw
				case 0x05:

				// phsubd
				// phsubd
				case 0x06:

				// phsubsw
				// phsubsw
				case 0x07:

				// psignb
				// psignb
				case 0x08:

				// psignw
				// psignw
				case 0x09:

				// psignd
				// psignd
				case 0x0A:

				// pmulhrsw
				// pmulhrsw
				case 0x0B:

				// pblendvb
				case 0x10:

				// blendvps
				case 0x14:

				// blendvpd
				case 0x15:

				// ptest
				case 0x17:

				// pabsb
				// pabsb
				case 0x1C:

				// pabsw
				// pabsw
				case 0x1D:

				// pabsd
				// pabsd
				case 0x1E:

				// pmovsxbw
				case 0x20:

				// pmovsxbd
				case 0x21:

				// pmovsxbq:
				case 0x22:

				// pmovsxwd
				case 0x23:

				// pmovxwq
				case 0x24:

				// pmovswdq
				case 0x25:

				// pmuldq
				case 0x28:

				// pcmpeqq
				case 0x29:

				// movntdqa
				case 0x2A:

				// packusdw
				case 0x2B:

				// pmovzxbw
				case 0x30:

				// pmovzxbd:
				case 0x31:

				// pmovzxbq
				case 0x32:

				// pmovzxwd
				case 0x33:

				// pmovzxwq
				case 0x34:

				// pmovzxdq
				case 0x35:

				// pcmpgtq
				case 0x36:

				// pminsb
				case 0x38:

				// pminsd
				case 0x39:

				// pminuw
				case 0x3A:

				// pminud
				case 0x3B:

				// pmaxsb
				case 0x3C:

				// pmaxsd
				case 0x3D:

				// pmaxuw
				case 0x3E:

				// pmaxud
				case 0x3F:

				// pmulld
				case 0x40:

				// phminposuw
				case 0x41:

				// NVEPT
				case 0x80:

				// NVVPID
				case 0x81:

				// MOVBE
				// CRC32
				case 0xF0:

				// MOVBE
				// CRC32
				case 0xF1:

				// no case
				default:
				}

			// second byte is 0x3A
			case 0x3A:
			(*byte)++;
                switch(**byte)
                {
                // roundps
                case 0x08:

                // roundpd
                case 0x09:

                // roundss
                case 0x0A:

                // roundsd
                case 0x0B: 
				
				// blendps
                case 0x0C:

                // blendpd
                case 0x0D:

                // pblendw
                case 0x0E:

                // palignr
                // palignr
                case 0x0F:

                // pextrb
                case 0x14:

                // pextrw
                case 0x15:

                // pextrd / pextrq
                case 0x16:

                // extractps
                case 0x17:

                // pinsrb
                case 0x20:

                // insertps
                case 0x21:

                // pinsrd / pinsrq
                case 0x22:

                // dpps
                case 0x40:

                // dppd
                case 0x41:

                // mpsadbw
                case 0x42:

                // pcmpestrm
                case 0x60:

                // pcmpestri
                case 0x61:

                // pcmpistrm
                case 0x62:

                // pcmpistri
                case 0x63:

                // no case
                default:
                }

			}
		*/

        // no case
        default:
#ifdef DEBUG
			printf("Wrong instruction at %x\n", *byte); 
#endif
			return NULL;
			break;
	}
	if(inst->modrm_size == 2)
	{
		if(inst != NULL)
		{
			free(inst);
			inst = NULL;
		}
	}
	return inst;
}

void modOpcode(unsigned char **byte, unsigned char *endbyte, Instruction *inst, int size)
{
	int i = 0;

	for(i=0; i<size; i++)
	{
		if(*byte > endbyte)
			parse_error(INSTRUCTION_OVERFLOW);
		inst->opcode[inst->opcode_size] = **byte;
		inst->opcode_size++;
		(*byte)++;
	}
}

void modModRMParams(unsigned char **byte, unsigned char *endbyte, Instruction *inst, char expected_reg)
{
	unsigned char _modrm_mod;
	unsigned char _modrm_reg;
	unsigned char _modrm_rm;
	unsigned char _sib_scale;
	unsigned char _sib_index;
	unsigned char _sib_base;
	// I assume this machine use 32 bit operand size, and do not use 16 bit addressing form.
	// So all of ModRM and SIB follows 32 bit addressing form given in Intel instruction set reference.
	
	unsigned char modrm = **byte;
	unsigned char sib = 0;

	if(*byte > endbyte)	// overflow
		parse_error(INSTRUCTION_OVERFLOW);

	inst->modrm = **byte;
	inst->modrm_size = 1;
	(*byte)++;

	if(modrm_rm(modrm) == 4 && modrm_mod(modrm) != 3)	// SIB follows MOdRM
	{
		inst->sib = **byte;
		inst->sib_size = 1;
		(*byte)++;
	}

	if(modrm_mod(modrm) == 0 && modrm_rm(modrm) == 5)	// disp32
		modDisplacement(byte, endbyte, inst, WORD);
	else if(modrm_mod(modrm) == 1)	// disp8
		modDisplacement(byte, endbyte ,inst, BYTE);
	else if(modrm_mod(modrm) == 2)	// disp32
		modDisplacement(byte, endbyte, inst, WORD);

	if(inst->sib_size)	// SIB present
	{
		sib = inst->sib;

		if(modrm_mod(modrm) == 0 && sib_base(sib) == 5)	// disp32
			modDisplacement(byte, endbyte, inst, WORD);
	}
	if(inst->modrm_size == 1)
	{
		_modrm_mod = modrm_mod(inst->modrm);
		_modrm_reg = modrm_reg(inst->modrm);
		_modrm_rm = modrm_rm(inst->modrm);


		if(_modrm_reg == 0 && _modrm_reg != expected_reg)
			_modrm_reg = 1;
		else if(_modrm_reg == 3 && _modrm_reg != expected_reg)
			_modrm_reg = 2;
		else if(_modrm_reg == 5 && _modrm_reg != expected_reg)
			_modrm_reg = 4;
//		else
//			inst->modrm_size = 2;

		if(_modrm_rm == 0)
			_modrm_rm = 1;
		else if(_modrm_rm == 3)
			_modrm_rm = 2;
//		else
//			inst->modrm_size = 2;

		if(_modrm_mod == 0 && _modrm_rm >= 4)
			inst->modrm_size = 2;
		else if( _modrm_mod == 1 || _modrm_mod == 2)
			inst->modrm_size = 2;
		else if( _modrm_mod == 3 && _modrm_rm >= 6)
			inst->modrm_size = 2;

		inst->modrm = (_modrm_mod << 6) + (_modrm_reg << 3) + (_modrm_rm);
	}
	if(inst->sib_size == 1)
	{
		_sib_scale = sib_scale(inst->modrm);
		_sib_index = sib_index(inst->modrm);
		_sib_base = sib_base(inst->modrm);

		if(_sib_index == 0)
			_sib_index= 1;
		else if(_sib_index == 3)
			_sib_index = 2;

		if(_sib_base == 0)
			_sib_index = 1;
		else if(_sib_base == 3)
			_sib_index = 2;

		inst->sib = (_sib_scale << 6) + (_sib_index << 3) + (_sib_base);
		inst->modrm_size = 2;
	}
}

void modImmediate(unsigned char **byte, unsigned char *endbyte, Instruction *inst, int size)
{
	int i = 0;

	if(isPrefix(inst, 0x66))
	{
		size = size / 2;
		if(size <= 1)	size = 1;
	}
	for(i = 0; i<size; i++)
	{
		if(*byte > endbyte)
			parse_error(INSTRUCTION_OVERFLOW);
		inst->immediate[inst->immediate_size] = **byte;
		inst->immediate_size++;
		(*byte)++;
	}
}

void modDisplacement(unsigned char **byte, unsigned char *endbyte, Instruction *inst, int size)
{
	int i = 0;
	for(i = 0; i<size; i++)
	{
		if(*byte > endbyte)
			parse_error(INSTRUCTION_OVERFLOW);
		inst->displacement[inst->displacement_size] = **byte;
		inst->displacement_size++;
		(*byte)++;
	}
}

void modPrefix(unsigned char **byte, unsigned char *endbyte, Instruction *inst)
{
	if(*byte > endbyte)
		parse_error(INSTRUCTION_OVERFLOW);
	inst->prefix[inst->prefix_size] = **byte;
	inst->prefix_size++;
	(*byte)++;
}
