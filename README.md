# usde
Ultra Small Disassembler Engine

Note. 
- Only common opcodes are supported (+75% of one byte opcodes).
- Can get implicit registers. For example :
- 0x90 NOP <=> XCHG eAX, eAX <=> Result->Reg = 1000b where 000 mean eax in Mod.Reg and
the 1 is a special flag to tell you that the instruction will use eax as destination.

Source used :
http://www.sandpile.org/
https://github.com/notnanocat/ssde
http://wiki.osdev.org/X86-64_Instruction_Encoding
https://github.com/AmrThabet/x86Emulator/tree/master/hde28c
