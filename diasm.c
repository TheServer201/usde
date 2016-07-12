#include <stdint.h>

typedef struct {
	uint32_t Flg, Imm;
	uint8_t Pre, Opc, Mod, Reg, Rm, Sca, Ind, Bas;
} Insn;

uint8_t DiAsm(const uint8_t *Code, Insn *Result){
	uint8_t Size, Byte, Low0, Low1, High, Mod;
	for(Size = 0x10; Size; Size -= 0x01){
		Byte = Code[Size];
		Low0 = Byte & 0x07;
		if(Result->Mod){
			Result->Rm  = Low0;
			Result->Mod = Byte >> 0x06;
			Result->Reg = (Byte >> 0x03) & 0x07;
			if(Result->Mod != 0x03){
				if(Result->Pre & 0x08){
					if(Result->Rm == 0x06)
						Result->Imm = 0x02;
				} else {
					if(Result->Rm == 0x05)
						Result->Imm = 0x04;
					if(Result->Rm == 0x04)
						Result->Sca = 0x01;
				}
				if(Result->Mod == 0x01)
					Result->Imm = 0x01;
			}
		} else if(Result->Sca){
			Result->Bas = Low0;
			Result->Sca = Byte >> 0x06;
			Result->Ind = (Byte >> 0x03) & 0x07;
		} else if(Result->Imm)
			Result->Imm = ((uint32_t*)Code)[Size] & (-0x01 >> 0x08 * (0x04 - Result->Imm));
		else {
			Low1 = Byte & 0x0F;
			High = Byte & 0xF0;
			if(Result->Flg & 0x01){
				if((High == 0x40) || (Byte == 0xAF) ||
				   (Byte == 0xB6) || (Byte == 0xB7)){
					Result->Mod = 0x01;
				}
			} else
				switch(Byte){
					case 0xF2:
						Result->Pre |= 0x01;
					case 0xF3:
						Result->Pre |= 0x02;
					case 0x66:
						Result->Pre |= 0x04;
					case 0x67:
						Result->Pre |= 0x08;
					case 0x0F:
						Result->Flg = 0x01;
					case 0xC3: case 0xC9:
						Result->Flg = 0x02;
					case 0xFE: case 0xFF:
						Result->Mod = 0x01;
					case 0x6A: case 0xEB:
						Result->Imm = 0x01;
					case 0xC2:
						Result->Imm = 0x02;
					case 0x68: case 0xE8:
					case 0xE9:
						Result->Imm = 0x04;
					case 0x6B: case 0xC0:
					case 0xC1: case 0xC6:
						Result->Mod = 0x01;
						Result->Imm = 0x01;
					case 0x69: case 0xC7:
						Result->Mod = 0x01;
						Result->Imm = 0x04;
					default:
						switch(High){
							case 0x70:
								Result->Imm = 0x01;
							case 0x80:
								Result->Mod = 0x01;
								if(Low1 < 0x04){
									if(Low1 != 0x02)
										Result->Imm = 0x01;
									else
										Result->Imm = 0x04;
								}
							case 0x90:
								if(Low1 < 0x08)
									Result->Reg = Low0 | 0x08;
							case 0xA0:
								if(Low1 < 0x04){
									if(Result->Pre & 0x08)
										Result->Imm = 0x02;
									else
										Result->Imm = 0x04;
								} else if(Low1 < 0x08)
									Result->Reg = 0x20;
								else {
									Result->Reg = 0x08;
									if((Low1 != 0x08) || 
									   (Low1 != 0x09))
										Result->Reg |= 0x10;
									else
										if(Low1 & 0x01)
											Result->Imm = 0x04;
										else
											Result->Imm = 0x01;
								}
							case 0xB0:
								Result->Reg = Low0;
								if(Low1 < 0x08)
									Result->Imm = 0x01;
								else
									Result->Imm = 0x04;
							case 0xD0:
								if(Low1 < 0x04)
									Result->Mod = 0x01;
							default:
								if(High < 0x40){
									if(Low1 < 0x06)
										Result->Mod = 0x01;
									else if(Low1 < 0x08){
										Result->Reg = 0x08;
										if(Low1 & 0x01)
											Result->Imm = 0x04;
										else
											Result->Imm = 0x01;
									}
								} else if(High < 0x60)
									Result->Reg = Low0;
						}
				}
			Result->Opc = Byte;
		}
	}
	return Size;
}
