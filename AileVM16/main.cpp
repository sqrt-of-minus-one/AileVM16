    ////////////////////////////////////////
   //       AileVM16 by SnegirSoft       //
  //                                    //
 //  File: main.cpp                    //
////////////////////////////////////////

#include <fstream>
#include <thread>

#include "memory.h"
#include "error_codes.h"
#include "util.h"
#include "in_out.h"

constexpr uint8_t sign8 = 1 << 7;
constexpr uint16_t sign16 = 1 << 15;
constexpr uint8_t max8 = (uint8_t)0 - 1;
constexpr uint16_t max16 = (uint16_t)0 - 1;

// Temporary; there will be interruption here
#define INVALID_INSTRUCTION \
	std::cout << "Invalid instruction" << std::endl; \
	std::exit(EError::ERROR)

#define ONE one(first, f_size, next)
#define TWO two(first, second, f_size, s_size, next)
#define TWO_SAME two_same(first, second, f_size, next)
#define SET_FLAGS_8(pointer) \
	F->PF = *(pointer) & 0b0001; \
	F->ZF = *(pointer) == 0; \
	F->SF = *(pointer) & sign8
#define SET_FLAGS_16(pointer) \
	F->PF = *(pointer) & 0b0001; \
	F->ZF = (pointer)[0] == 0 && (pointer)[1] == 0; \
	F->SF = (pointer)[1] & sign16

void free()
{
	_getch();
	delete[] gen_reg;
	delete[] ins_reg;
	delete[] flags;
	delete[] memory;
}

#define TEST
int main(int argc, char** argv)
{
	std::atexit(&free);

#ifndef TEST
	if (argc < 2)
	{
		std::cout << "Wrong number of arguments" << std::endl;
		std::exit(EError::WRONG_NUMBER_OF_ARGUMENTS);
	}

	memory_size = argc >= 3 ? std::stoi(argv[2]) : 65'536; // Memory size is stored in 2-nd argument
	if (memory_size > 65'536)
	{
		std::cout << "The maximum memory size is 65536 bytes" << std::endl;
		std::exit(EError::WRONG_MEMORY_SIZE);
	}
	std::basic_ifstream<uint8_t> file(argv[1], std::ios::in | std::ios::binary); // The name of the file with binary program is stored in 1-st argument
#else
	std::basic_ofstream<uint8_t> out("test.lvm", std::ios::out | std::ios::binary);
	out << (uint8_t)0x00 << (uint8_t)0xE0 <<													// Stack address
/*00*/	(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x03 << (uint8_t)0x00 << (uint8_t)0x00 <<	// MOV DX, 0

/*05*/	(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'H'  <<	// MOV AX, 0x4801
/*0A*/	(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*0C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*0E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*12*/	(uint8_t)0x15 << (uint8_t)0x10 << (uint8_t)0x0C <<										// JE 0x0C
/*15*/	(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x00 << (uint8_t)0x00 <<	// MOV AX, 0
/*1A*/	(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*1C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*1E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*22*/	(uint8_t)0x15 << (uint8_t)0x90 << (uint8_t)0x1C <<										// JNE 0x1C

/*25*/	(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'e'  <<	// MOV AX, 0x
/*2A*/	(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*2C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*2E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*32*/	(uint8_t)0x15 << (uint8_t)0x10 << (uint8_t)0x2C <<										// JE 0x2C
/*35*/	(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x00 << (uint8_t)0x00 <<	// MOV AX, 0
/*3A*/	(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*3C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*3E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*42*/	(uint8_t)0x15 << (uint8_t)0x90 << (uint8_t)0x3C <<										// JNE 0x3C

		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'l'  <<	// MOV AX, 0x
		(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*4C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*4E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*52*/	(uint8_t)0x15 << (uint8_t)0x10 << (uint8_t)0x4C <<										// JE 0x4C
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x00 << (uint8_t)0x00 <<	// MOV AX, 0
		(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*5C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*5E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*62*/	(uint8_t)0x15 << (uint8_t)0x90 << (uint8_t)0x5C <<										// JNE 0x5C

		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'l'  <<	// MOV AX, 0x4801
		(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*6C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*6E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*72*/	(uint8_t)0x15 << (uint8_t)0x10 << (uint8_t)0x6C <<										// JE 0x6C
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x00 << (uint8_t)0x00 <<	// MOV AX, 0
		(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*7C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*7E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*82*/	(uint8_t)0x15 << (uint8_t)0x90 << (uint8_t)0x7C <<										// JNE 0x7C

		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'o'  <<	// MOV AX, 0x4801
		(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*8C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*8E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*92*/	(uint8_t)0x15 << (uint8_t)0x10 << (uint8_t)0x8C <<										// JE 0x8C
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x00 << (uint8_t)0x00 <<	// MOV AX, 0
		(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*9C*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*9E*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*A2*/	(uint8_t)0x15 << (uint8_t)0x90 << (uint8_t)0x9C <<										// JNE 0x9C

		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'!'  <<	// MOV AX, 0x4801
		(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*AC*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*AE*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*B2*/	(uint8_t)0x15 << (uint8_t)0x10 << (uint8_t)0xAC <<										// JE 0xAC
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x00 << (uint8_t)0x00 <<	// MOV AX, 0
		(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*BC*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
/*BE*/	(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
/*C2*/	(uint8_t)0x15 << (uint8_t)0x90 << (uint8_t)0xBC <<										// JNE 0xBC

		(uint8_t)0x20;																			// EXIT

	out.close();

	memory_size = 65'536;
	std::basic_ifstream<uint8_t> file("test.lvm", std::ios::in | std::ios::binary);
#endif

	if (!file.is_open())
	{
		std::cout << "The file does not exist or cannot be accessed" << std::endl;
		std::exit(EError::NO_FILE);
	}

	// Initialize memory
	memory = new uint8_t[memory_size];
	MEM = (uint8_t*)memory;

	// Load the program from the file to memory
	{
		// The first two bytes are the address of the stack
		*BPL = file.get();
		*BPH = file.get();
		*SP = *BP - 1;

		uint16_t addr = 0; // Current address
		uint8_t byte; // Current byte
		while (byte = file.get(), !file.eof())
		{
			MEM[addr++] = byte;
			if (addr >= memory_size - 3)
			{
				std::cout << "The program is out of memory" << std::endl;
				std::exit(EError::OUT_OF_MEMORY);
			}
		}

		// The last three bytes are always jump to the beginning
		MEM[addr] = 0x14; // JMP byte 0
		MEM[addr + 1] = 0x10;
		MEM[addr + 2] = 0x00;
	}
	file.close();

	std::thread print_th(&print);
	print_th.detach();

	uint8_t* first;
	uint8_t* second;
	uint8_t f_size, s_size;
	while (true)
	{
		uint8_t next = MEM[*IP + 1];
		switch (MEM[*IP])
		{
		case 0x00: // NOP
		{
			(*IP)++;
			break;
		}
		case 0x01: // MOV
		{
			*SYS = TWO_SAME;
			if (first && second &&
				((next & 0b0011'0000) != 0b0001'0000) &&		// First cannot be value
				((next & 0b1111'0000) != 0b1111'0000))			// First and second cannot both be memory
			{
				if (f_size == 1)
				{
					*first = *second;
				}
				else
				{
					first[0] = second[0];
					first[1] = second[1];
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x02: // CHG
		{
			*SYS = TWO_SAME;
			if (first && second &&
				((next & 0b0011'0000) != 0b0001'0000) &&		// First cannot be value
				((next & 0b1100'0000) != 0b0100'0000) &&		// Second cannot be value
				((next & 0b1111'0000) != 0b1111'0000))		// First and second cannot both be memory
			{
				if (f_size == 1)
				{
					std::swap(*first, *second);
				}
				else
				{
					std::swap(first[0], second[0]);
					std::swap(first[1], second[1]);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x03: // PUSH / POP
		{
			*SYS = ONE;
			if ((next & 0b0011'0011) == 0b0000'0001) // PUSHF / POPF
			{
				first = FLAGSL;
				f_size = 2;
			}

			switch (next & 0b1100'1100)
			{
			case 0b0000'0000: // PUSH
			{
				if ((next & 0b0011'0011) == 0b0000'0000) // PUSHA
				{
					for (int i = 0; i < 32; i++)
					{
						MEM[*SP + 1 + i] = AL[i];
					}
					*SP += 32;
				}
				else if (first) // PUSH
				{
					if (f_size == 1)
					{
						MEM[++(*SP)] = *first;
					}
					else
					{
						MEM[++(*SP)] = first[0];
						MEM[++(*SP)] = first[1];
					}
				}
				else
				{
					INVALID_INSTRUCTION;
				}
				break;
			}
			case 0b1000'0000: // POP
			{
				if ((next & 0b0011'0011) == 0b0000'0000) // POPA
				{
					uint16_t SP_t = *SP;
					for (int i = 0; i < 32; i++)
					{
						AL[i] = MEM[SP_t - 31 + i];
					}
					*SP = SP_t - 32;
				}
				else if (first && // POP
					((next & 0b0011'0000) != 0b0001'0000))		// First cannot be value
				{
					if (f_size == 1)
					{
						*first = MEM[(*SP)--];
					}
					else
					{
						first[0] = MEM[(*SP)--];
						first[1] = MEM[(*SP)--];
					}
				}
				else
				{
					INVALID_INSTRUCTION;
				}
				break;
			}
			default:
			{
				INVALID_INSTRUCTION;
			}
			}
			*IP = *SYS;
			break;
		}
		case 0x04: // NEG / INC / DEC
		{
			*SYS = ONE;
			if (first)
			{
				switch (next & 0b1110'1110)
				{
				case 0010'0000: // NEG
				{
					if (f_size == 1)
					{
						*first = (uint8_t)0 - *first;

						SET_FLAGS_8(first);
					}
					else
					{
						*(uint16_t*)first = (uint16_t)0 - *(uint16_t*)first;

						SET_FLAGS_16(first);
					}
					break;
				}
				case 1010'0000: // INC
				{
					if (f_size == 1)
					{
						(*first)++;

						SET_FLAGS_8(first);
						F->CF = *first == 0;
						F->AF = (*first & 0b0001'1111) == 0b0001'0000;
						F->OF = (*first & sign8) == sign8;
					}
					else
					{
						(*(uint16_t*)first)++;

						SET_FLAGS_16(first);
						F->CF = first[0] == 0 && first[1] == 0;
						F->AF = (*first & 0b0001'1111) == 0b0001'0000;
						F->OF = (*(uint16_t*)first & sign16) == sign16;
					}
					break;
				}
				case 1110'0000: // DEC
				{
					if (f_size == 1)
					{
						(*first)--;

						SET_FLAGS_8(first);
						F->CF = *first == max8;
						F->AF = (*first & 0b0001'1111) == 0b0000'1111;
						F->OF = *first == (max8 ^ sign8);
					}
					else
					{
						(*(uint16_t*)first)++;

						SET_FLAGS_16(first);
						F->CF = first[0] == max8 && first[1] == max8;
						F->AF = (*first & 0b0001'1111) == 0b0001'0000;
						F->OF = (*(uint16_t*)first) == (max16 ^ sign16);
					}
					break;
				}
				default:
				{
					INVALID_INSTRUCTION;
				}
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x05: // ADD / ADC
		{
			bool carry = false;
			if (next & 0b0010'0000) // ADC
			{
				carry = F->CF;
				next &= 0b1101'1111;
			}
			*SYS = TWO;

			if (first && second &&
				f_size >= s_size &&								// First size cannot be less than second size
				((next & 0b0011'0000) != 0b0001'0000) &&		// First cannot be value
				((next & 0b1111'0000) != 0b1111'0000))			// First and second cannot both be memory
			{
				if (f_size == 1)
				{
					uint8_t tmp1 = *first;
					uint8_t tmp2 = *second + carry;
					*first += tmp2;

					SET_FLAGS_8(first);
					F->CF = (*second == max8 && carry) || (max8 - tmp2 < tmp1);
					F->AF = ((tmp1 ^ tmp2) & 0b0001'0000) != (*first & 0b0001'0000);
					F->OF = ((tmp1 ^ tmp2) & sign8) != (*first & sign8);
				}
				else
				{
					uint16_t* first16 = (uint16_t*)first;
					uint16_t* second16 = (uint16_t*)second;

					uint16_t tmp1 = *first16;
					uint16_t tmp2 = *second16 + carry;
					*first16 += tmp2;

					SET_FLAGS_16(first);
					F->CF = (*second16 == max16 && carry) || (max16 - tmp2 < tmp1);
					F->AF = ((tmp1 ^ tmp2) & 0b0001'0000) != (*first16 & 0b0001'0000);
					F->OF = ((tmp1 ^ tmp2) & sign16) != (*first16 & sign16);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x06: // SUB / SBB
		{
			bool carry = false;
			if (next & 0b0010'0000) // SBB
			{
				carry = F->CF;
				next &= 0b1101'1111;
			}
			*SYS = TWO;

			if (first && second &&
				f_size >= s_size &&								// First size cannot be less than second size
				((next & 0b0011'0000) != 0b0001'0000) &&		// First cannot be value
				((next & 0b1111'0000) != 0b1111'0000))			// First and second cannot both be memory
			{
				if (f_size == 1)
				{
					uint8_t tmp1 = *first;
					uint8_t tmp2 = *second + carry;
					*first -= tmp2;

					SET_FLAGS_8(first);
					F->CF = (*second == max8 && carry) || (tmp1 < tmp2);
					F->AF = ((tmp1 ^ tmp2) & 0b0001'0000) != (*first & 0b0001'0000);
					F->OF = ((tmp1 ^ tmp2) & sign8) != (*first & sign8);
				}
				else
				{
					uint16_t* first16 = (uint16_t*)first;
					uint16_t* second16 = (uint16_t*)second;

					uint16_t tmp1 = *first16;
					uint16_t tmp2 = *second16 + carry;
					*first16 -= tmp2;

					SET_FLAGS_16(first);
					F->CF = (*second16 == max16 && carry) || (tmp1 < tmp2);
					F->AF = ((tmp1 ^ tmp2) & 0b0001'0000) != (*first16 & 0b0001'0000);
					F->OF = ((tmp1 ^ tmp2) & sign16) != (*first16 & sign16);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x07: // CMP
		{
			*SYS = TWO;

			if (first && second &&
				f_size >= s_size &&								// First size cannot be less than second size
				((next & 0b0011'0000) != 0b0001'0000) &&		// First cannot be value
				((next & 0b1111'0000) != 0b1111'0000))			// First and second cannot both be memory
			{
				if (f_size == 1)
				{
					uint8_t tmp = *first - *second;

					SET_FLAGS_8(&tmp);
					F->CF = *first < *second;
					F->AF = ((*first ^ *second) & 0b0001'0000) != (tmp & 0b0001'0000);
					F->OF = ((*first ^ *second) & sign8) != (tmp & sign8);
				}
				else
				{
					uint16_t* first16 = (uint16_t*)first;
					uint16_t* second16 = (uint16_t*)second;

					uint16_t tmp = *first16 - *second16;

					SET_FLAGS_16((int8_t*)(&tmp));
					F->CF = *first16 < *second16;
					F->AF = ((*first16 ^ *second16) & 0b0001'0000) != (tmp & 0b0001'0000);
					F->OF = ((*first16 ^ *second16) & sign16) != (tmp & sign16);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x08: // MUL / IMUL
		{
			*SYS = ONE;
			if (first)
			{
				switch (next & 0b1100'1100)
				{
				case 0b0000'0000: // MUL
				{
					if (f_size == 1)
					{
						*AX = (uint16_t)*AL * *first;
						SET_FLAGS_16(AL);
						F->CF = *AH;
						F->AF = false;
						F->OF = *AH;
					}
					else
					{
						uint32_t tmp = (uint32_t)*AX * *(uint16_t*)first;
						uint16_t* tmp_p = (uint16_t*)&tmp;
						*AX = tmp_p[0];
						*DX = tmp_p[1];
						
						F->PF = tmp & 0b0001;
						F->ZF = tmp == 0;
						F->SF = *DH & sign8;
						F->CF = *DX;
						F->AF = false;
						F->OF = *DX;
					}
					break;
				}
				case 0b1000'0000: // IMUL
				{
					if (f_size == 1)
					{
						*(int16_t*)AX = (int16_t)*AL * *(int8_t*)first;
						SET_FLAGS_16(AL);
						F->CF = *AH;
						F->AF = false;
						F->OF = *AH;
					}
					else
					{
						int32_t tmp = (int32_t)*AX * *(int16_t*)first;
						int16_t* tmp_p = (int16_t*)&tmp;
						*(int16_t*)AX = tmp_p[0];
						*(int16_t*)DX = tmp_p[1];

						F->PF = tmp & 0b0001;
						F->ZF = tmp == 0;
						F->SF = *DH & sign8;
						F->CF = *DX;
						F->AF = false;
						F->OF = *DX;
					}
					break;
				}
				default:
				{
					INVALID_INSTRUCTION;
				}
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x09: // DIV / IDIV
		{
			*SYS = ONE;
			if (first)
			{
				switch (next & 0b1100'1100)
				{
				case 0b0000'0000: // DIV
				{
					if (f_size == 1)
					{
						uint16_t tmp1 = *AX / *first;
						uint8_t tmp2 = *AX % *first;

						*AL = tmp1;
						*AH = tmp2;

						SET_FLAGS_8(AL);
						F->CF = tmp1 > max8;
						F->AF = false;
						F->OF = tmp1 > max8;
					}
					else
					{
						uint32_t tmp0 = ((uint32_t)*DX << 16) + *AX;
						uint32_t tmp1 = tmp0 / *(uint16_t*)first;
						uint16_t tmp2 = tmp0 % *(uint16_t*)first;

						*AX = tmp1;
						*DX = tmp2;

						F->PF = *AX & 0b0001;
						F->ZF = *AX == 0;
						F->SF = *AH & sign8;
						F->CF = tmp1 > max16;
						F->AF = false;
						F->OF = tmp1 > max16;
					}
					break;
				}
				case 0b1000'0000: // IDIV
				{
					if (f_size == 1)
					{
						int16_t tmp1 = (int16_t)*AX / *(int8_t*)first;
						int8_t tmp2 = (int16_t)*AX % *(int8_t*)first;

						*(int8_t*)AL = tmp1;
						*(int8_t*)AH = tmp2;

						SET_FLAGS_8(AL);
						F->CF = ((int8_t*)tmp1)[1] != max8;
						F->AF = false;
						F->OF = ((int8_t*)tmp1)[1] != max8;
					}
					else
					{
						int32_t tmp0 = ((uint32_t)*DX << 16) + *AX;
						int32_t tmp1 = tmp0 / *(int16_t*)first;
						int16_t tmp2 = tmp0 % *(int16_t*)first;

						*(int16_t*)AX = tmp1;
						*(int16_t*)DX = tmp2;

						F->PF = *AX & 0b0001;
						F->ZF = *AX == 0;
						F->SF = *AH & sign8;
						F->CF = ((int16_t*)tmp1)[1] != max16;
						F->AF = false;
						F->OF = ((int16_t*)tmp1)[1] != max16;
					}
					break;
				}
				default:
				{
					INVALID_INSTRUCTION;
				}
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x0A: // NOT
		{
			*SYS = ONE;
			if (first &&
				(next & 0b1110'1100) == 0b0010'0000)
			{
				if (f_size == 1)
				{
					*first = ~*first;

					SET_FLAGS_8(first);
				}
				else
				{
					*(uint16_t*)first = ~*(uint16_t*)first;

					SET_FLAGS_16(first);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x0B: // AND / TEST
		{
			bool test = (next & 0b0010'0000) == 0b0000'0000; // TEST or AND
			next |= 0b0010'0000;
			*SYS = TWO_SAME;
			if (first && second)
			{
				if (f_size = 1)
				{
					uint8_t tmp = *first & *second;
					if (!test)
					{
						*first = tmp;
					}

					SET_FLAGS_8(&tmp);
				}
				else
				{
					uint16_t tmp = *(uint16_t*)first & *(uint16_t*)second;
					if (!test)
					{
						*(uint16_t*)first = tmp;
					}

					SET_FLAGS_16((uint8_t*)(&tmp));
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x0C: // OR / XOR
		{
			bool is_xor = (next & 0b0010'0000) == 0b0000'0000; // XOR or OR
			next |= 0b0010'0000;
			*SYS = TWO_SAME;
			if (first && second)
			{
				if (f_size = 1)
				{
					if (is_xor)
					{
						*first ^= *second;
					}
					else
					{
						*first |= *second;
					}

					SET_FLAGS_8(first);
				}
				else
				{
					if (is_xor)
					{
						*(uint16_t*)first ^= *(uint16_t*)second;
					}
					else
					{
						*(uint16_t*)first |= *(uint16_t*)second;
					}

					SET_FLAGS_16(first);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x0D: // BT / BTN
		{
			bool btn = (next & 0b0010'0000) == 0b0000'0000; // BTN or BT
			next |= 0b0010'0000;
			*SYS = TWO;
			if (first && second &&
				s_size != 1 &&									// Second size must be one byte
				(next & 0b1100'0000) != 0b1100'0000 &&			// Second cannot be memory
				*second < 8 * f_size)							// Second must be a number of first's bit
			{
				uint16_t mask = 1 << *second;
				if (f_size = 1)
				{
					F->CF = *first & mask;
					if (btn)
					{
						if (F->CF)
						{
							*first |= mask;
						}
						else
						{
							*first &= ~mask;
						}
					}
				}
				else
				{
					F->CF = *(uint16_t*)first & mask;
					if (btn)
					{
						if (F->CF)
						{
							*(uint16_t*)first |= mask;
						}
						else
						{
							*(uint16_t*)first &= ~mask;
						}
					}
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x0E: // BTR / BTS
		{
			bool bts = (next & 0b0010'0000) == 0b0000'0000; // BTS or BTR
			next |= 0b0010'0000;
			*SYS = TWO;
			if (first && second &&
				s_size != 1 &&									// Second size must be one byte
				(next & 0b1100'0000) != 0b1100'0000 &&			// Second cannot be memory
				*second < 8 * f_size)							// Second must be a number of first's bit
			{
				uint16_t mask = 1 << *second;
				if (f_size = 1)
				{
					F->CF = *first & mask;
					if (bts)
					{
						*first |= mask;
					}
					else
					{
						*first &= ~mask;
					}
				}
				else
				{
					F->CF = *(uint16_t*)first & mask;
					if (bts)
					{
						*(uint16_t*)first |= mask;
					}
					else
					{
						*(uint16_t*)first &= ~mask;
					}
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x0F: // NC / RC / SC / MOVAF / MOVFA / INx / OUTx
		{
			switch (next)
			{
			case 0x00: // NC
			{
				F->CF = !F->CF;
				break;
			}
			case 0x01: // RC
			{
				F->CF = false;
				break;
			}
			case 0x02: // SC
			{
				F->CF = true;
				break;
			}
			case 0x03: // MOVAF
			{
				*AH = *FLAGSL;
				break;
			}
			case 0x04: // MOVFA
			{
				*FLAGSL = *AH & 0b1101'0111 | 0b0000'0010;
				break;
			}
			case 0x10: // INB
			{
				if (*DX < in_count)
				{
					in_mtx.lock();
					*AL = IN[*DX];
					in_mtx.unlock();
				}
				else
				{
					INVALID_INSTRUCTION;
				}
				break;
			}
			case 0x11: // INW
			{
				if (*DX < in_count - 1)
				{
					in_mtx.lock();
					*AL = IN[*DX];
					*AH = IN[*DX + 1];
					in_mtx.unlock();
				}
				else
				{
					INVALID_INSTRUCTION;
				}
				break;
			}
			case 0x14: // OUTB
			{
				if (*DX < in_count)
				{
					out_mtx.lock();
					OUT[*DX] = *AL;
					out_mtx.unlock();
				}
				else
				{
					INVALID_INSTRUCTION;
				}
				break;
			}
			case 0x15: // OUTW
			{
				if (*DX < in_count - 1)
				{
					out_mtx.lock();
					OUT[*DX] = *AL;
					OUT[*DX + 1] = *AH;
					out_mtx.unlock();
				}
				else
				{
					INVALID_INSTRUCTION;
				}
				break;
			}
			default:
			{
				INVALID_INSTRUCTION;
			}
			}
			(*IP) += 2;
			break;
		}
		case 0x10: // SHL / SHR
		{
			bool right = (next & 0b0010'0000) == 0b0000'0000; // SHR or SHL
			next |= 0b0010'0000;
			*SYS = TWO;
			if (first && second &&
				(next & 0b1100'0000) != 0b1100'0000)			// Second cannot be memory
			{
				if (f_size = 1)
				{
					if (*second != 0)
					{
						if (right)
						{
							*first >>= (s_size == 1 ? *second : *(uint16_t*)second) - 1;
							F->CF = *first & 0b0001;
							*first >>= 1;
						}
						else
						{
							*first <<= (s_size == 1 ? *second : *(uint16_t*)second) - 1;
							F->CF = *first & sign8;
							*first <<= 1;
						}
					}
					SET_FLAGS_8(first);
				}
				else
				{
					if (*(uint16_t*)second != 0)
					{
						if (right)
						{
							*(uint16_t*)first >>= (s_size == 1 ? *second : *(uint16_t*)second) - 1;
							F->CF = *(uint16_t*)first & 0b0001;
							*(uint16_t*)first >>= 1;
						}
						else
						{
							*(uint16_t*)first <<= (s_size == 1 ? *second : *(uint16_t*)second) - 1;
							F->CF = *(uint16_t*)first & sign16;
							*(uint16_t*)first <<= 1;
						}
					}
					SET_FLAGS_16(first);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x11: // SAL / SAR
		{
			bool right = (next & 0b0010'0000) == 0b0000'0000; // SAR or SAL
			next |= 0b0010'0000;
			*SYS = TWO;
			if (first && second &&
				(next & 0b1100'0000) != 0b1100'0000)			// Second cannot be memory
			{
				if (f_size = 1)
				{
					if (*second != 0)
					{
						if (right)
						{
							// Implementation-dependent, works with Microsoft compiler
							*(int8_t*)first >>= (s_size == 1 ? *second : *(uint16_t*)second) - 1;
							F->CF = *first & 0b0001;
							*(int8_t*)first >>= 1;
						}
						else
						{
							*first <<= (s_size == 1 ? *second : *(uint16_t*)second) - 1;
							F->CF = *first & sign8;
							*first <<= 1;
						}
					}
					SET_FLAGS_8(first);
				}
				else
				{
					if (*(uint16_t*)second != 0)
					{
						if (right)
						{
							// Implementation-dependent, works with Microsoft compiler
							*(int16_t*)first >>= (s_size == 1 ? *second : *(uint16_t*)second) - 1;
							F->CF = *(uint16_t*)first & 0b0001;
							*(int16_t*)first >>= 1;
						}
						else
						{
							*(uint16_t*)first <<= (s_size == 1 ? *second : *(uint16_t*)second) - 1;
							F->CF = *(uint16_t*)first & sign16;
							*(uint16_t*)first <<= 1;
						}
					}
					SET_FLAGS_16(first);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x12: // ROL / ROR
		{
			bool right = (next & 0b0010'0000) == 0b0000'0000; // ROR or ROL
			next |= 0b0010'0000;
			*SYS = TWO;
			if (first && second &&
				(next & 0b1100'0000) != 0b1100'0000)			// Second cannot be memory
			{
				uint16_t s = (s_size == 1 ? *second : *(uint16_t*)second);
				if (f_size = 1)
				{
					if (*second != 0)
					{
						if (right)
						{
							for (int i = 0; i < s; i++)
							{
								F->CF = *first & 0b0001;
								*first >>= 1;
								if (F->CF)
								{
									*first |= sign8;
								}
							}
						}
						else
						{
							for (int i = 0; i < s; i++)
							{
								F->CF = *first & sign8;
								*first <<= 1;
								if (F->CF)
								{
									*first |= 0b0001;
								}
							}
						}
					}
					SET_FLAGS_8(first);
				}
				else
				{
					if (*second != 0)
					{
						if (right)
						{
							for (int i = 0; i < s; i++)
							{
								F->CF = *(uint16_t*)first & 0b0001;
								*(uint16_t*)first >>= 1;
								if (F->CF)
								{
									*(uint16_t*)first |= sign16;
								}
							}
						}
						else
						{
							for (int i = 0; i < s; i++)
							{
								F->CF = *(uint16_t*)first & sign16;
								*(uint16_t*)first <<= 1;
								if (F->CF)
								{
									*(uint16_t*)first |= 0b0001;
								}
							}
						}
					}
					SET_FLAGS_16(first);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x13: // RCL / RCR
		{
			bool right = (next & 0b0010'0000) == 0b0000'0000; // RCR or RCL
			next |= 0b0010'0000;
			*SYS = TWO;
			if (first && second &&
				(next & 0b1100'0000) != 0b1100'0000)			// Second cannot be memory
			{
				uint16_t s = (s_size == 1 ? *second : *(uint16_t*)second);
				if (f_size = 1)
				{
					if (*second != 0)
					{
						if (right)
						{
							for (int i = 0; i < s; i++)
							{
								bool tmp = F->CF;
								F->CF = *first & 0b0001;
								*first >>= 1;
								if (tmp)
								{
									*first |= sign8;
								}
							}
						}
						else
						{
							for (int i = 0; i < s; i++)
							{
								bool tmp = F->CF;
								F->CF = *first & sign8;
								*first <<= 1;
								if (tmp)
								{
									*first |= 0b0001;
								}
							}
						}
					}
					SET_FLAGS_8(first);
				}
				else
				{
					if (*second != 0)
					{
						if (right)
						{
							for (int i = 0; i < s; i++)
							{
								bool tmp = F->CF;
								F->CF = *(uint16_t*)first & 0b0001;
								*(uint16_t*)first >>= 1;
								if (tmp)
								{
									*(uint16_t*)first |= sign16;
								}
							}
						}
						else
						{
							for (int i = 0; i < s; i++)
							{
								bool tmp = F->CF;
								F->CF = *(uint16_t*)first & sign16;
								*(uint16_t*)first <<= 1;
								if (tmp)
								{
									*(uint16_t*)first |= 0b0001;
								}
							}
						}
					}
					SET_FLAGS_16(first);
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			*IP = *SYS;
			break;
		}
		case 0x14: // JMP / CALL / RET
		{
			uint8_t r_next = next;
			next &= 0b0011'0011;
			*SYS = ONE;
			switch (next & 0b1100'1100)
			{
			case 0b1100'0000: // RET
			{
				uint16_t addr = *(uint16_t*)(MEM + *SP - 1);
				*SP -= 2;
				break;
			}
			case 0b1000'0000: // CALL
			{
				MEM[++(*SP)] = SYS[0];
				MEM[++(*SP)] = SYS[1];
				[[fallthrough]];
			}
			case 0b0000'0000: // JMP
			{
				if (first)
				{
					*IP = (f_size == 1 ? *first : *(uint16_t*)first);
				}
				else
				{
					INVALID_INSTRUCTION;
				}
				break;
			}
			default:
			{
				INVALID_INSTRUCTION;
			}
			}
			break;
		}
		
#define JUMP_IF(cond)											\
	bool f = (next & 0b1000'0000) == 0b1000'0000;				\
	next &= 0b0111'1111;										\
	*SYS = ONE;													\
	if (first &&												\
		(next & 0b0100'1100) == 0b0000'0000)					\
	{															\
		if ((cond) ^ f)											\
		{														\
			*IP = (f_size == 1 ? *first : *(uint16_t*)first);	\
		}														\
		else													\
		{														\
			*IP = *SYS;											\
		}														\
	}															\
	else														\
	{															\
		INVALID_INSTRUCTION;									\
	}

		case 0x15: // JZ=JE / JNZ=JNE
		{
			JUMP_IF(F->ZF);
			break;
		}
		case 0x16: // JC=JNAE=JB / JNC=JAE=JNB
		{
			JUMP_IF(F->CF);
			break;
		}
		case 0x17: // JP / JNP
		{
			JUMP_IF(F->PF);
			break;
		}
		case 0x18: // JS / JNS
		{
			JUMP_IF(F->SF);
			break;
		}
		case 0x19: // JO / JNO
		{
			JUMP_IF(F->OF);
			break;
		}
		case 0x1A: // JA=JNBE / JNA=JBE
		{
			JUMP_IF(!F->CF && !F->ZF);
			break;
		}
		case 0x1B: // JG=JNLE / JLE=JNG
		{
			JUMP_IF(!F->ZF && F->SF == F->OF);
			break;
		}
		case 0x1C: // JGE=JNL / JL=JNGE
		{
			JUMP_IF(F->SF == F->OF);
			break;
		}
		case 0x1D: // JCZ / LOOP
		{
			bool f = (next & 0b1000'0000) == 0b0000'0000;
			next &= 0b1000'0000;
			*SYS = ONE;
			if (first &&
				(next & 0b0100'1100) == 0b0000'0000)
			{
				if (*CX == 0 ^ f)
				{
					*IP = (f_size == 1 ? *first : *(uint16_t*)first);
				}
				else
				{
					*IP = *SYS;
				}

				if (*CX != 0 && f)
				{
					(*CX)--;
				}
			}
			else
			{
				INVALID_INSTRUCTION;
			}
			break;
		}
		case 0x20: // EXIT
		{
			std::exit(EError::OK);
			break;
		}
		default:
		{
			INVALID_INSTRUCTION;
		}
		}
	}
	std::exit(EError::OK);
}
