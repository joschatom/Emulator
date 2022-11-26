#pragma once
#include "types.h"
#include <iostream>
#include <fstream>
#include "memory.h"

struct PROGRAMM
{
	Word EntryAdress;
	u32	ID;
	Byte P_SP;
};

PROGRAMM LoadFileIntoMemory(char* filename, MEM& memory);
PROGRAMM LoadProgramIntoMemory(Byte* Program, u32 NumBytes, MEM& memory);
