#include "Progamm.h"

PROGRAMM LoadFileIntoMemory(char* filename, MEM& memory) {
	PROGRAMM prog = {};

	prog.ID = 0;

	std::ofstream file;

	file.open(filename, 'rb');

	if (file.bad()) std::cout << "Bad File: " << filename << std::endl;

	return prog;
}

PROGRAMM LoadProgramIntoMemory(Byte* Program, u32 NumBytes, MEM& memory) {

	PROGRAMM prog = {};

	prog.ID = 0;

	Word LoadAddress = 0;
	if (Program && NumBytes > 2)
	{
		u32 At = 0;
		const Word Lo = Program[At++];
		const Word Hi = Program[At++] << 8;
		LoadAddress = Lo | Hi;
		for (Word i = LoadAddress; i < LoadAddress + NumBytes - 2; i++)
		{
			//TODO: mem copy?
			memory[i] = Program[At++];
		}
	}

	prog.EntryAdress = LoadAddress;

	return prog;
};
