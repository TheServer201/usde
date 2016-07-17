#include <stdint.h>
#include <stdio.h>

typedef struct {
	uint32_t Imm;
	uint8_t Flg, Pre, Opc, Mod, Reg, Rm, Sca, Ind, Bas;
} Insn;

uint8_t DiAsm(const uint8_t *Code, Insn *Result){
	uint8_t Size, Byte, Low0, Low1, High, Imm, Tmp;
	for(Size = 0x00; Size < 0x10; Size += 0x01){
		Byte = Code[Size];
		Low0 = Byte & 0x07;
		if(Result->Flg & 0x08){
			Result->Flg -= 0x10;
			Imm = Result->Flg >> 0x04;
			if(Imm)
				Result->Imm |= Byte << (0x08 * (0x04 - Imm));
			else
				goto Done;
			continue;
		}
		if(Result->Flg & 0x04){
			Result->Bas = Low0;
			Result->Sca = Byte >> 0x06;
			Result->Ind = (Byte >> 0x03) & 0x07;
			if(Result->Bas == 0x05)
				Result->Flg |= 0x48;
			else
				goto Done;
			continue;
		} 
		if(Result->Flg & 0x02){
			Result->Rm  = Low0;
			Result->Mod = Byte >> 0x06;
			Result->Reg = (Byte >> 0x03) & 0x07;
			if(Result->Mod != 0x03){
				if(Result->Pre & 0x08){
					if((Result->Mod == 0x02) || 
					   (Result->Rm  == 0x06))
						Result->Flg |= 0x28;
				} else {
					if((Result->Mod == 0x02) ||
					   (Result->Rm  == 0x05))
						Result->Flg |= 0x48;
					if(Result->Rm == 0x04)
						Result->Flg |= 0x04;
				}
				if(Result->Mod == 0x01)
					Result->Flg |= 0x18;
			}
			continue;
		}
		if(Size < 2){
			Result->Opc = Byte;
			High = Byte & 0xF0;
			if(Result->Flg & 0x01){
				if((High == 0x40) ||
				   (Byte == 0xAF) || 
				   (Byte == 0xB6) ||
				   (Byte == 0xB7))
					Result->Flg |= 0x02;
				else
					goto Done;
			} else {
				Low1 = Byte & 0x0F;
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
						goto Done;
					case 0x6A: case 0xEB:
						Result->Flg |= 0x18;
					case 0xC2:
						Result->Flg |= 0x28;
					case 0x68: case 0xE8:
					case 0xE9:
						Result->Flg |= 0x48;
					case 0x6B: case 0xC0:
					case 0xC1: case 0xC6:
						Result->Flg |= 0x1A;
					case 0x69: case 0xC7:
						Result->Flg |= 0x4A;
					default:
						switch(High){
							case 0x70:
								Result->Flg |= 0x18;
							case 0x80:
								Result->Flg |= 0x02;
								if(Low1 < 0x04){
									if(Low1 != 0x02)
										Result->Flg |= 0x18;
									else
										Result->Flg |= 0x48;
								}
							case 0x90:
								if(Low1 < 0x08)
									Result->Reg = Low0 | 0x08;
							case 0xA0:
								if(Low1 < 0x04){
									if(Result->Pre & 0x08)
										Result->Flg |= 0x28;
									else
										Result->Flg |= 0x48;
								} else if(Low1 < 0x08){
									Result->Reg = 0x20;
								} else {
									Result->Reg = 0x08;
									if((Low1 != 0x08) || 
									   (Low1 != 0x09)){
										Result->Reg |= 0x10;
									} else {
										if(Low1 & 0x01)
											Result->Flg |= 0x48;
										else
											Result->Flg |= 0x18;
									}
								}
							case 0xB0:
								Result->Reg = Low0;
								if(Low1 < 0x08)
									Result->Flg |= 0x18;
								else
									Result->Flg |= 0x48;
							case 0xD0:
								if(Low1 < 0x04)
									Result->Flg |= 0x02;
							default:
								if(High < 0x40){
									if(Low1 < 0x06)
										Result->Flg |= 0x02;
									else if(Low1 < 0x08){
										Result->Reg = 0x08;
										if(Low1 & 0x01)
											Result->Flg |= 0x48;
										else
											Result->Flg |= 0x18;
									}
								} else if(High < 0x60)
									Result->Reg = Low0;
								Tmp = Byte & 0xF7;
								if(Tmp > 0xF5)
									Result->Flg |= 0x02;
						}
				}
			}
		}
	}
Done:
	return Size + 0x01;
}
