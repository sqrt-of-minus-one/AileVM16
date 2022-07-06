    ////////////////////////////////////////
   //       AileVM16 by SnegirSoft       //
  //                                    //
 //  File: util.h                      //
////////////////////////////////////////

// Bytes in the first operand
uint8_t get_first_size(uint8_t byte)
{
	return (byte & 0b0000'0010) ? 0 : 1 << (byte & 0b0000'0001);
}

// Bytes in the second operand
uint8_t get_second_size(uint8_t byte)
{
	return (byte & 0b0000'1000) ? 0 : 1 << ((byte & 0b0000'0100) >> 2);
}

// Bytes in both operands
uint8_t get_size(uint8_t byte)
{
	switch (byte & 0b0000'1111)
	{
	case 0b0000:
		return 1;
	case 0b0101:
		return 2;
//	case 0b1010:	// For 64-bit
//		return 4;
//	case 0b1111:
//		return 8;
	default:
		return 0;
	}
}

uint8_t* get_reg_addr_(uint8_t addr)
{
	switch (addr & 0b1111'0000)
	{
	case 0b0000'0000:
	{
		return (uint8_t*)(AX + addr);
		break;
	}
	case 0b0001'0000: // AH, BH etc.
	{
		return (uint8_t*)(AX + addr) + 1;
		break;
	}
	default:
	{
		return nullptr;
		break;
	}
	}
}

// Returns the address of the next instruction
uint16_t two_(uint8_t*& out_first, uint8_t*& out_second, uint8_t& out_f_size, uint8_t& out_s_size, uint8_t byte)
{
	uint8_t first_instr_size;

	// First
	switch (byte & 0b0011'0000)
	{
	case 0b0001'0000: // Value
	{
		out_first = MEM + *IP + 2;
		first_instr_size = out_f_size;
		break;
	}
	case 0b0010'0000: // Register
	{
		out_first = get_reg_addr_(MEM[*IP + 2]);
		first_instr_size = 1;
		break;
	}
	case 0b0011'0000: // Memory
	{
		out_first = MEM + *(uint16_t*)(MEM + *IP + 2);
		first_instr_size = 2;
		break;
	}
	default:
	{
		out_first = nullptr;
		out_second = nullptr;
		return *IP + 2;
	}
	}

	// Second
	switch (byte & 0b1100'0000)
	{
	case 0b0100'0000: // Value
	{
		out_second = MEM + *IP + 2 + first_instr_size;
		return *IP + 2 + first_instr_size + out_s_size;
		break;
	}
	case 0b1000'0000: // Register
	{
		out_second = get_reg_addr_(MEM[*IP + 2 + first_instr_size]);
		return *IP + first_instr_size + 3;
		break;
	}
	case 0b1100'0000: // Memory
	{
		out_second = MEM + *(uint16_t*)(MEM + *IP + 2 + first_instr_size);
		return *IP + first_instr_size + 4;
		break;
	}
	default:
	{
		out_first = nullptr;
		out_second = nullptr;
		return *IP + 2;
	}
	}
}

// Returns the address of the next instruction
uint16_t one(uint8_t*& out_first, uint8_t& out_f_size, uint8_t byte)
{
	out_f_size = get_first_size(byte);
	if (out_f_size == 0)
	{
		out_first = nullptr;
		return *IP + 2;
	}

	switch (byte & 0b0011'0000)
	{
	case 0b0001'0000: // Value
	{
		out_first = MEM + *IP + 2;
		return *IP + 2 + out_f_size;
		break;
	}
	case 0b0010'0000: // Register
	{
		out_first = get_reg_addr_(MEM[*IP + 2]);
		return *IP + 3;
		break;
	}
	case 0b0011'0000: // Memory
	{
		out_first = MEM + *(uint16_t*)(MEM + *IP + 2);
		return *IP + 4;
		break;
	}
	default:
	{
		out_first = nullptr;
		return *IP + 2;
	}
	}
}

// Returns the address of the next instruction
uint16_t two(uint8_t*& out_first, uint8_t*& out_second, uint8_t& out_f_size, uint8_t& out_s_size, uint8_t byte)
{
	out_f_size = get_first_size(byte);
	out_s_size = get_second_size(byte);
	if (out_f_size == 0 || out_s_size == 0)
	{
		out_first = nullptr;
		out_second = nullptr;
		return *IP + 2;
	}
	return two_(out_first, out_second, out_f_size, out_s_size, byte);
}

// Returns the address of the next instruction
uint16_t two_same(uint8_t*& out_first, uint8_t*& out_second, uint8_t& out_size, uint8_t byte)
{
	out_size = get_size(byte);
	if (out_size == 0)
	{
		out_first = nullptr;
		out_second = nullptr;
		return *IP + 2;
	}
	return two_(out_first, out_second, out_size, out_size, byte);
}
