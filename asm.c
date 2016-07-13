#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct {
	uint32_t Flg, Imm;
	uint8_t Pre, Opc, Mod, Reg, Rm, Sca, Ind, Bas;
} Insn;

extern uint8_t DiAsm(const uint8_t *Code, Insn *Result);

int main(){
	/* 775DE18D | 01 1C C5 34 12 00 00     | add dword ptr ds:[eax*8+1234],ebx       | */
	uint8_t Raw[] = {0x01, 0x1C, 0xC5, 0x34, 0x12, 0x00, 0x00};
	Insn Instruction;
	memset(&Instruction, 0x00, sizeof(Insn));
	uint8_t Size = DiAsm(Raw, &Instruction);
	printf("Size: %x\n" // 0x07
		   "Flg : %x\n" // 0x00
		   "Imm : %x\n" // 0x1234
		   "Pre : %x\n" // 0x00
		   "Opc : %x\n" // 0x01
		   "Mod : %x\n" // 0x00
		   "Reg : %x\n" // 0x03
		   "Rm  : %x\n" // 0x04
		   "Sca : %x\n" // 0x03
		   "Ind : %x\n" // 0x00
		   "Bas : %x\n" // 0x05
		   , Size, Instruction.Flg, Instruction.Imm, Instruction.Pre, Instruction.Opc, Instruction.Mod, Instruction.Reg, Instruction.Rm, Instruction.Sca, Instruction.Ind, Instruction.Bas);
	return 0;
}