#pragma once
#include "Opcodes.h"
#include "memory.h"
#include "types.h"
#include "Parser.h"
#include <stdio.h>



struct CPU {
	Word PC;

	Byte SP;

	Byte A, X, Y;

	Byte C : 1;	//0: Carry Flag	
	Byte Z : 1;	//1: Zero Flag
	Byte I : 1; //2: Interrupt disable
	Byte D : 1; //3: Decimal mode
	Byte B : 1; //4: Break
	Byte Unused : 1; //5: Unused
	Byte V : 1; //6: Overflow
	Byte N : 1; //7: Negative

	void PrintStatusInfo(MEM& memory) {
		printf("[GENERAL]\n");
		printf("PC=0x%x(%d)\n", PC, PC);
		printf("SP=0x%x(%d)\n", SP, SP);
		printf("[REGISTERS]\n");
		printf("A=0x%x(%d)\n", A, A);
		printf("X=0x%x(%d)\n", X, X);
		printf("Y=0x%x(%d)\n", Y, Y);
		printf("[FLAGS]\n");
		printf("Carry: %x\n", C);
		printf("Zero: %x\n", Z);
		printf("Interrupt disable: %x\n", I);
		printf("Decimal Mode: %x\n", D);
		printf("Break: %x\n", B);
		printf("Overflow: %x\n", V);
		printf("Negative: %x\n", N);
		printf("[MEMORY]\n");
		printf("MAX: 0x%x", MEM::MAX_MEM);
	}

	void SetZeroAndNegativeFlags(Byte Register)
	{
		Z = (Register == 0);
		N = (Register & 0b10000000) > 0;
	}


	void Reset(MEM& memory)
	{
		PC = 0xFFFC;
		SP = 0xFF;
		C = Z = I = D = B = V = N = 0;
		A = X = Y = 0;
		memory.Initialise();
	}

	Byte FetchByte(s32& Cycles, const MEM& memory)
	{
		Byte Data = memory[PC];
		PC++;
		Cycles--;
		return Data;
	}

	Word FetchWord(s32& Cycles, const MEM& memory)
	{
		// 6502 is little endian
		Word Data = memory[PC];
		PC++;

		Data |= (memory[PC] << 8);
		PC++;

		Cycles -= 2;
		return Data;
	}

	s32 Execute(s32 Cycles, MEM& memory) {
		while (Cycles > 0)
		{
			Byte Ins = FetchByte(Cycles, memory);

			switch (Ins)
			{

			default:
				break;
			}
		}
		return Cycles;
	}

};


extern CPU cpu;