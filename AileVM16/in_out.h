    ////////////////////////////////////////
   //       AileVM16 by SnegirSoft       //
  //                                    //
 //  File: error_codes.h               //
////////////////////////////////////////

#include <iostream>
#include <mutex>

bool is_work = true;

constexpr uint16_t in_count = 16;
constexpr uint16_t out_count = 16;

void* in = new uint8_t[in_count] {};
void* out = new uint8_t[out_count] {};

uint8_t* IN = (uint8_t*)in;
uint8_t* OUT = (uint8_t*)out;

std::mutex in_mtx;
std::mutex out_mtx;

constexpr uint16_t OUT_PRINT_SYNC = 0;
constexpr uint16_t OUT_PRINT_DATA = 1;

constexpr uint16_t IN_PRINT_SYNC = 0;

void print()
{
	bool sync = false;
	while (is_work)
	{
		out_mtx.lock();
		if (sync != OUT[OUT_PRINT_SYNC])
		{
			if (sync = OUT[OUT_PRINT_SYNC])
			{
				std::cout << (char)OUT[OUT_PRINT_DATA];
			}
			IN[IN_PRINT_SYNC] = sync;
		}
		out_mtx.unlock();
	}
}
