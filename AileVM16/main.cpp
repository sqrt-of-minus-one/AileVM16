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

// Temporary; there will be interruption here
#define INVALID_INSTRUCTION \
	std::wcout << L"Invalid instruction" << std::endl; \
	std::exit(EError::ERROR)

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
			
		}
		}
	}
}
