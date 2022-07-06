    ////////////////////////////////////////
   //       AileVM16 by SnegirSoft       //
  //                                    //
 //  File: memory.h                    //
////////////////////////////////////////

#include <cstdint>

void* gen_reg = new uint8_t[32] {}; // General-purpose registers
void* ins_reg = new uint8_t[2]{}; // Instruction pointer register
void* flags = new uint8_t[2] { 0b01000000, 0b00000000 }; // Flags register
void* sys_reg = new uint8_t[2] {}; // System registers
void* memory;
uint32_t memory_size;

uint16_t* AX = (uint16_t*)gen_reg;
uint16_t* BX = AX + 1;
uint16_t* CX = AX + 2;
uint16_t* DX = AX + 3;
uint16_t* SP = AX + 4;
uint16_t* BP = AX + 5;
uint16_t* SI = AX + 6;
uint16_t* DI = AX + 7;
uint16_t* R8W = AX + 8;
uint16_t* R9W = AX + 9;
uint16_t* R10W = AX + 10;
uint16_t* R11W = AX + 11;
uint16_t* R12W = AX + 12;
uint16_t* R13W = AX + 13;
uint16_t* R14W = AX + 14;
uint16_t* R15W = AX + 15;
uint16_t* IP = (uint16_t*)ins_reg;
uint16_t* FLAGS = (uint16_t*)flags;
uint16_t* SYS = (uint16_t*)sys_reg;

uint8_t* AL = (uint8_t*)AX;
uint8_t* BL = (uint8_t*)BX;
uint8_t* CL = (uint8_t*)CX;
uint8_t* DL = (uint8_t*)DX;
uint8_t* SPL = (uint8_t*)SP;
uint8_t* BPL = (uint8_t*)BP;
uint8_t* SIL = (uint8_t*)SI;
uint8_t* DIL = (uint8_t*)DI;
uint8_t* R8L = (uint8_t*)R8W;
uint8_t* R9L = (uint8_t*)R9W;
uint8_t* R10L = (uint8_t*)R10W;
uint8_t* R11L = (uint8_t*)R11W;
uint8_t* R12L = (uint8_t*)R12W;
uint8_t* R13L = (uint8_t*)R13W;
uint8_t* R14L = (uint8_t*)R14W;
uint8_t* R15L = (uint8_t*)R15W;
uint8_t* FLAGSL = (uint8_t*)FLAGS;
uint8_t* SYSL = (uint8_t*)SYS;

uint8_t* AH = AL + 1;
uint8_t* BH = BL + 1;
uint8_t* CH = CL + 1;
uint8_t* DH = DL + 1;
uint8_t* SPH = SPL + 1;
uint8_t* BPH = BPL + 1;
uint8_t* SIH = SIL + 1;
uint8_t* DIH = DIL + 1;
uint8_t* R8H = R8L + 1;
uint8_t* R9H = R9L + 1;
uint8_t* R10H = R10L + 1;
uint8_t* R11H = R11L + 1;
uint8_t* R12H = R12L + 1;
uint8_t* R13H = R13L + 1;
uint8_t* R14H = R14L + 1;
uint8_t* R15H = R15L + 1;
uint8_t* FLAGSH = FLAGSL + 1;
uint8_t* SYSH = SYSL + 1;

uint8_t* MEM;

struct FlagsType
{
	bool CF : 1; // Carry
	bool F1 : 1;
	bool PF : 1; // Parity
	bool F3 : 1;
	bool AF : 1; // Auxiliary Carry
	bool F5 : 1;
	bool ZF : 1; // Zero
	bool SF : 1; // Sign

	bool F8 : 1;
	bool F9 : 1;
	bool FA : 1;
	bool OF : 1; // Overflow
	bool FC : 1;
	bool FD : 1;
	bool FE : 1;
	bool FF : 1;
};
FlagsType* F = (FlagsType*)flags;

enum EFlags
{
	CF = 0b0000'0000'0000'0001,
	PF = 0b0000'0000'0000'0100,
	AF = 0b0000'0000'0001'0000,
	ZF = 0b0000'0000'0100'0000,
	SF = 0b0000'0000'1000'0000,
	OF = 0b0000'1000'0000'0000
};
