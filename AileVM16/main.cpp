    ////////////////////////////////////////
   //       AileVM16 by SnegirSoft       //
  //                                    //
 //  File: main.cpp                    //
////////////////////////////////////////

#include <iostream>
#include <fstream>

#include "memory.h"
#include "error_codes.h"
#include "util.h"

constexpr uint8_t sign8 = 1 << 7;
constexpr uint16_t sign16 = 1 << 15;
constexpr uint8_t max8 = (uint8_t)0 - 1;
constexpr uint16_t max16 = (uint16_t)0 - 1;

// Temporary; there will be interruption here
#define INVALID_INSTRUCTION \
	std::wcout << L"Invalid instruction" << std::endl; \
	std::exit(EError::ERROR)

#define ONE one(first, f_size, next)
#define TWO two(first, second, f_size, s_size, next)
#define TWO_SAME two_same(first, second, f_size, next)

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
		std::wcout << L"Wrong number of arguments" << std::endl;
		std::exit(EError::WRONG_NUMBER_OF_ARGUMENTS);
	}

	memory_size = argc >= 3 ? std::stoi(argv[2]) : 65'536; // Memory size is stored in 2-nd argument
	if (memory_size > 65'536)
	{
		std::wcout << L"The maximum memory size is 65536 bytes" << std::endl;
		std::exit(EError::WRONG_MEMORY_SIZE);
	}
	std::basic_ifstream<uint8_t> file(argv[1], std::ios::in | std::ios::binary); // The name of the file with binary program is stored in 1-st argument
#else
	std::basic_ofstream<uint8_t> out("test.lvm", std::ios::out | std::ios::binary);
	out << (uint8_t)0x05 << (uint8_t)0x06 << (uint8_t)0x09 << (uint8_t)0xA6 << (uint8_t)0x00 << (uint8_t)0x9F;
	out.close();

	memory_size = 65'536;
	std::basic_ifstream<uint8_t> file("test.lvm", std::ios::in | std::ios::binary);
#endif

	if (!file.is_open())
	{
		std::wcout << L"The file does not exist or cannot be accessed" << std::endl;
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
				std::wcout << L"The program is out of memory" << std::endl;
				std::exit(EError::OUT_OF_MEMORY);
			}
		}

		// The last three bytes are always jump to the beginning
		MEM[memory_size - 3] = 0x10; // JMP byte 0
		MEM[memory_size - 2] = 0x20;
		MEM[memory_size - 1] = 0x00;
	}
	file.close();

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
				((next & 0b1111'0000) != 0b1111'0000))		// First and second cannot both be memory
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

						F->PF = *first & 0b0001;
						F->ZF = *first == 0;
						F->SF = *first & sign8;
					}
					else
					{
						*(uint16_t*)first = (uint16_t)0 - *(uint16_t*)first;

						F->PF = *first & 0b0001;
						F->ZF = first[0] == 0 && first[1] == 0;
						F->SF = first[1] & sign8;
					}
					break;
				}
				case 1010'0000: // INC
				{
					if (f_size == 1)
					{
						(*first)++;

						F->CF = *first == 0;
						F->PF = *first & 0b0001;
						F->AF = (*first & 0b0001'1111) == 0b0001'0000;
						F->ZF = F->CF;
						F->SF = *first & sign8;
						F->OF = (*first & sign8) == sign8;
					}
					else
					{
						(*(uint16_t*)first)++;

						F->CF = first[0] == 0 && first[1] == 0;
						F->PF = *first & 0b0001;
						F->AF = (*first & 0b0001'1111) == 0b0001'0000;
						F->ZF = F->CF;
						F->SF = first[1] & sign8;
						F->OF = (*(uint16_t*)first & sign16) == sign16;
					}
					break;
				}
				case 1110'0000: // DEC
				{
					if (f_size == 1)
					{
						(*first)--;

						F->CF = *first == 0b1111'1111;
						F->PF = *first & 0b0001;
						F->AF = (*first & 0b0001'1111) == 0b0000'1111;
						F->ZF = *first == 0;
						F->SF = *first & sign8;
						F->OF = (*first & sign8) == sign8;
					}
					else
					{
						(*(uint16_t*)first)++;

						F->CF = first[0] == 0 && first[1] == 0;
						F->PF = *first & 0b0001;
						F->AF = (*first & 0b0001'1111) == 0b0001'0000;
						F->ZF = F->CF;
						F->SF = first[1] & sign8;
						F->OF = (*(uint16_t*)first & sign16) == sign16;
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
		}
		}
	}
}
