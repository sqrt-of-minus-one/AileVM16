    ////////////////////////////////////////
   //       AileVM16 by SnegirSoft       //
  //                                    //
 //  File: test.h                      //
////////////////////////////////////////

void test(std::basic_ofstream<uint8_t>& file)
{
	file << (uint8_t)0x00 << (uint8_t)0xE0 <<													// Stack address

		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'H'  <<	// MOV AX, 0x
		(uint8_t)0x14 << (uint8_t)0x90 << (uint8_t)0x31 <<										// CALL ?
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'e'  <<	// MOV AX, 0x
		(uint8_t)0x14 << (uint8_t)0x90 << (uint8_t)0x31 <<										// CALL ?
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'l'  <<	// MOV AX, 0x
		(uint8_t)0x14 << (uint8_t)0x90 << (uint8_t)0x31 <<										// CALL ?
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'l'  <<	// MOV AX, 0x
		(uint8_t)0x14 << (uint8_t)0x90 << (uint8_t)0x31 <<										// CALL ?
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'o'  <<	// MOV AX, 0x
		(uint8_t)0x14 << (uint8_t)0x90 << (uint8_t)0x31 <<										// CALL ?
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)'!'  <<	// MOV AX, 0x
		(uint8_t)0x14 << (uint8_t)0x90 << (uint8_t)0x31 <<										// CALL ?

		(uint8_t)0x20 <<																		// EXIT

/*31*/	(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x03 << (uint8_t)0x00 << (uint8_t)0x00 <<	// MOV DX, 0
		(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*38*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
		(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
		(uint8_t)0x15 << (uint8_t)0x10 << (uint8_t)0x38 <<										// JE 0x38
		(uint8_t)0x01 << (uint8_t)0x65 << (uint8_t)0x00 << (uint8_t)0x00 << (uint8_t)0x00 <<	// MOV AX, 0
/*46*/	(uint8_t)0x0F << (uint8_t)0x15 <<														// OUTW
/*48*/	(uint8_t)0x0F << (uint8_t)0x10 <<														// INB
		(uint8_t)0x07 << (uint8_t)0x60 << (uint8_t)0x00 << (uint8_t)0x00 <<						// CMP AL, 0
		(uint8_t)0x15 << (uint8_t)0x90 << (uint8_t)0x48 <<										// JNE 0x48
		(uint8_t)0x14 << (uint8_t)0xC0;															// RET
}
