// Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Opcodes.h"
#include "memory.h"
#include <stdio.h>
#include "Host.h"
#include <fstream>
#include <time.h>
#include <Windows.h>

#define TIMEOUT 1000
#define PC_ENTRY 0xFF00
#define RESET_PROCESS_MAX (MEM::MAX_MEM - 0xFFFC)-0x46
#define HOST_CONTROL_MEMORY_SIZE 0x46

#define INSDEF(ins, code) case OPCODE(ins): code break;
#define FUNCTIONDEF(name) Byte name(s32& Cycles, const MEM& memory)
#define FUNCTIONDEFW(name) Word name(s32& Cycles, const MEM& memory)
#define FUNCTIONDEFV(name) void name(s32& Cycles, MEM& memory)
#define LOADREG(addrname, reg) {reg = addrname(Cycles, memory);SetZeroAndNegativeFlags(reg); }
#define SETSTATUS(name, function) {name = function(Cycles, memory);}
#define EMPTY	{}
#define STOREREG(addrname, reg) {Word ST##reg##_Addr = addrname( Cycles, memory );WriteByte(reg, Cycles, ST##reg##_Addr, memory); }
#define DEFAULT default:{} break;
#define LOADINGTEXT(txt, times, _t) std::cout << txt;for (int _ = 0; _ < 8; _++) {Sleep(_t /1000);putchar('.');}std::cout << std::endl;
#define range(start, stop) for (int i=start; i<stop; i++)
#define COMPARE(addrname) {Word Address = addrname( Cycles, memory );Byte Operand=ReadByte(Cycles, Address, memory);RegisterCompare(Operand, A); }
#define SFLAG(name) {Flag.##name = true;}
#define CFLAG(name) {Flag.##name = false;}


using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;
using s32 = signed int;

struct StatusFlags
{
	Byte C : 1;	//0: Carry Flag	
	Byte Z : 1;	//1: Zero Flag
	Byte I : 1; //2: Interrupt disable
	Byte D : 1; //3: Decimal mode
	Byte B : 1; //4: Break
	Byte Unused : 1; //5: Unused
	Byte V : 1; //6: Overflow
	Byte N : 1; //7: Negative

};


struct CPU {

    Word PC;

    Byte SP;

    Byte A, X, Y;

	union // processor status
	{
		Byte PS;
		StatusFlags Flag;
	};

	MEM Memory;

	static constexpr Byte
		NegativeFlagBit = 0b10000000,
		OverflowFlagBit = 0b01000000,
		BreakFlagBit = 0b000010000,
		UnusedFlagBit = 0b000100000,
		InterruptDisableFlagBit = 0b000000100,
		ZeroBit = 0b00000001;

	void PrintStatusInfo(MEM& memory) {
		printf("[GENERAL]\n");
		printf("PC=0x%x(%d)\n", PC, PC);
		printf("SP=0x%x(%d)\n", SP, SP);
		printf("[REGISTERS]\n");
		printf("A=0x%x(%d)\n", A, A);
		printf("X=0x%x(%d)\n", X, X);
		printf("Y=0x%x(%d)\n", Y, Y);
		printf("[FLAGS]\n");
		printf("Carry: %x\n", Flag.C);
		printf("Zero: %x\n", Flag.Z);
		printf("Interrupt disable: %x\n", Flag.I);
		printf("Decimal Mode: %x\n", Flag.D);
		printf("Break: %x\n", Flag.B);
		printf("Overflow: %x\n", Flag.V);
		printf("Negative: %x\n", Flag.N);
		printf("[MEMORY]\n");
		printf("MAX: 0x%x\n", MEM::MAX_MEM);
	}

	void SetZeroAndNegativeFlags(Byte Register)
	{
		Flag.Z = (Register == 0);
		Flag.N = (Register & NegativeFlagBit) > 0;
	}


	void Reset(MEM& memory)
	{
		PC = PC_ENTRY;
		SP = 0xFF;
		Flag.C = Flag.Z = Flag.I = Flag.D = Flag.B = Flag.V = Flag.N = 0;
		A = X = Y = 0;
		memory.Initialise();

		LOADINGTEXT("Reseting CPU", 8, 999);
	}

	FUNCTIONDEFW(FetchByte)
	{
		Byte Data = memory[PC];
		PC++;
		Cycles--;
		return Data;
	}

	FUNCTIONDEFW(FetchWord)
	{
		// 6502 is little endian
		Word Data = memory[PC];
		PC++;

		Data |= (memory[PC] << 8);
		PC++;

		Cycles -= 2;
		return Data;
	}

	Byte ReadByte(
		s32& Cycles,
		Word Address,
		const MEM& memory)
	{
		Byte Data = memory[Address];
		Cycles--;
		return Data;
	}

	Word ReadWord(
		s32& Cycles,
		Word Address,
		const MEM& memory)
	{
		Byte LoByte = ReadByte(Cycles, Address, memory);
		Byte HiByte = ReadByte(Cycles, Address + 1, memory);
		return LoByte | (HiByte << 8);
	}

	/** write 1 byte to memory */
	void WriteByte(Byte Value, s32& Cycles, Word Address, MEM& memory)
	{
		memory[Address] = Value;
		Cycles--;
	}

	/** write 2 bytes to memory */
	void WriteWord(Word Value, s32& Cycles, Word Address, MEM& memory)
	{
		memory[Address] = Value & 0xFF;
		memory[Address + 1] = (Value >> 8);
		Cycles -= 2;
	}

#pragma region Stack Functions


	Word SPToAddress() const
	{
		return 0x100 | SP;
	}

	void PushWordToStack( s32& Cycles, MEM& memory, Word Value )
	{
		WriteByte( Value >> 8, Cycles, SPToAddress(), memory );
		SP--;
		WriteByte( Value & 0xFF, Cycles, SPToAddress(), memory );
		SP--;
	}

	/** Push the PC-1 onto the stack */
	FUNCTIONDEFV(PushPCMinusOneToStack)
	{
		PushWordToStack( Cycles, memory, PC - 1 );
	}

	/** Push the PC+1 onto the stack */
	FUNCTIONDEFV(PushPCPlusOneToStack)
	{
		PushWordToStack( Cycles, memory, PC + 1 );
	}

	/** Push the PC onto the stack */
	FUNCTIONDEFV(PushPCToStack)
	{
		PushWordToStack( Cycles, memory, PC );
	}

	void PushByteOntoStack( s32& Cycles, Byte Value, MEM& memory )
	{
		const Word SPWord = SPToAddress();
		memory[SPWord] = Value;
		Cycles--;
		SP--;
		Cycles--;
	}

	FUNCTIONDEF(PopByteFromStack)
	{
		SP++;
		Cycles--;
		const Word SPWord = SPToAddress();
		Byte Value = memory[SPWord];
		Cycles--;
		return Value;
	}

	/** Pop a 16-bit value from the stack */
	FUNCTIONDEFW(PopWordFromStack)
	{
		Word ValueFromStack = ReadWord( Cycles, SPToAddress()+1, memory );
		SP += 2;
		Cycles--;
		return ValueFromStack;
	}

	FUNCTIONDEFV(PushPSToStack) {
		Byte PSStack = PS | BreakFlagBit | UnusedFlagBit;
		PushByteOntoStack(Cycles, PSStack, memory);
	}

	FUNCTIONDEFV(PopPSFromStack) {
		PS = PopByteFromStack(Cycles, memory);
		Flag.B = false;
		Flag.Unused = false;
	}

#pragma endregion

#pragma region Function for Instructions!

	FUNCTIONDEFW(AddrZeroPage)
	{
		Byte ZeroPageAddr = FetchByte(Cycles, memory);
		return ZeroPageAddr;
	}

	FUNCTIONDEFW(AddrZeroPageX) {
		Byte ZeroPageAddr = FetchByte(Cycles, memory);
		ZeroPageAddr += X;
		Cycles--;
		return ZeroPageAddr;
	}

	FUNCTIONDEFW(AddrZeroPageY) {
		Byte ZeroPageAddr = FetchByte(Cycles, memory);
		ZeroPageAddr += Y;
		Cycles--;
		return ZeroPageAddr;
	}

	FUNCTIONDEFW(AddrAbsolute) {
		Word AbsAddress = FetchWord(Cycles, memory);
		return AbsAddress;
	}

	FUNCTIONDEFW(AddrAbsoluteX) {
		Word AbsAddress = FetchWord(Cycles, memory);
		Word AbsAddressX = AbsAddress + X;
		const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressX) >> 8;
		if (CrossedPageBoundary)
		{
			Cycles--;
		}

		return AbsAddressX;
	}

	FUNCTIONDEFW(AddrAbsoluteX_5) {
		Word AbsAddress = FetchWord(Cycles, memory);
		Word AbsAddressX = AbsAddress + X;
		Cycles--;
		return AbsAddressX;
	}

	FUNCTIONDEFW(AddrAbsoluteY) {
		Word AbsAddress = FetchWord(Cycles, memory);
		Word AbsAddressY = AbsAddress + Y;
		const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressY) >> 8;
		if (CrossedPageBoundary)
		{
			Cycles--;
		}

		return AbsAddressY;
	}

	FUNCTIONDEFW(AddrAbsoluteY_5) {
		Word AbsAddress = FetchWord(Cycles, memory);
		Word AbsAddressY = AbsAddress + Y;
		Cycles--;
		return AbsAddressY;
	}

	FUNCTIONDEFW(AddrIndirectX) {
		Byte ZPAddress = FetchByte(Cycles, memory);
		ZPAddress += X;
		Cycles--;
		Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
		return EffectiveAddr;
	}

	FUNCTIONDEFW(AddrIndirectY) {
		Byte ZPAddress = FetchByte(Cycles, memory);
		Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
		Word EffectiveAddrY = EffectiveAddr + Y;
		const bool CrossedPageBoundary = (EffectiveAddr ^ EffectiveAddrY) >> 8;
		if (CrossedPageBoundary)
		{
			Cycles--;
		}
		return EffectiveAddrY;
	}

	FUNCTIONDEFW(AddrIndirectY_6) {
		Byte ZPAddress = FetchByte(Cycles, memory);
		Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
		Word EffectiveAddrY = EffectiveAddr + Y;
		Cycles--;
		return EffectiveAddrY;
	}

	void RegisterCompare(Byte Operand, Byte RegisterValue) {
		Byte Temp = RegisterValue - Operand;
		Flag.N = (Temp & NegativeFlagBit) > 0;
		Flag.Z = RegisterValue == Operand;
		Flag.C = RegisterValue >= Operand;
	}

#pragma endregion


	s32 Execute(s32 Cycles, MEM& memory) {

		while (Cycles > 0)
		{
			Memory = memory;
			Byte Ins = FetchByte(Cycles, memory);

			switch (Ins)
			{
				INSDEF(HOSTCALL, {
					//printf("Hostcall at $%x\n", PC);
					u32 id = (u32)FetchByte(Cycles, memory);
					void* parameters[3];
					parameters[0] = (void*)A;
					parameters[1] =(void*) X;
					parameters[2] =(void*) Y;
					
					int _ = HandleHostcall(id, parameters);
					
					switch (_)
					{
					case EXTERNAL_RUNTINE_CODE(75):
						Reset(memory);

					case EXTERNAL_RUNTINE_CODE(30):
						PrintStatusInfo(memory);

					case EXTERNAL_RUNTINE_CODE(44): {
						X = Cycles;
					}
					case EXTERNAL_RUNTINE_CODE(83): {
						PC = PC - A;
					}
					case EXTERNAL_RUNTINE_CODE(34): {
						if (Flag.Z == true) {
							Flag.Z = false;
							break;
						}
						else PC += 2;
					}
					case EXTERNAL_RUNTINE_CODE(54): {
						printf("%d", X);
					}
			
					}

					memory = Memory;
				})

				// LDA

				INSDEF(LDA_IM, {
					A = FetchByte(Cycles, memory);
					SetZeroAndNegativeFlags( A );
				})

				INSDEF(LDA_ZP, {

					A = AddrZeroPage(Cycles, memory);
					SetZeroAndNegativeFlags( A );
				})

				INSDEF(LDA_ZPX, {

					A = AddrZeroPageX(Cycles, memory);
					SetZeroAndNegativeFlags(A);
				})

				INSDEF(LDA_ABS, {

					A = AddrAbsolute(Cycles, memory);
					SetZeroAndNegativeFlags( A );
				})

				INSDEF(LDA_ABSX, {
					A = AddrAbsoluteX(Cycles, memory);
					SetZeroAndNegativeFlags( A );
				})

				INSDEF(LDA_ABSY, {
					A = AddrAbsoluteY(Cycles, memory);
					SetZeroAndNegativeFlags(A);
				})

				INSDEF(LDA_INDX, {
					A = AddrIndirectX(Cycles, memory);
					SetZeroAndNegativeFlags( A );
				})

				INSDEF(LDA_INDY, {
					A = AddrIndirectY(Cycles, memory);
					SetZeroAndNegativeFlags( A );

				})

				// LDX

				INSDEF(LDX_IM, LOADREG(FetchByte, X))

				INSDEF(LDX_ZP, LOADREG(AddrZeroPage, X))

				INSDEF(LDX_ZPY, LOADREG(AddrZeroPageY, X))

				INSDEF(LDX_ABS, LOADREG(AddrAbsolute, X))

				INSDEF(LDX_ABSY, LOADREG(AddrAbsoluteY, X))

				// LDY

				INSDEF(LDY_IM, LOADREG(FetchByte, Y))

				INSDEF(LDY_ZP, LOADREG(AddrZeroPage, Y))

				INSDEF(LDY_ZPX, LOADREG(AddrZeroPageX, X))

				INSDEF(LDY_ABS, LOADREG(AddrAbsolute, Y))

				INSDEF(LDY_ABSX, LOADREG(AddrAbsoluteX, Y))
				
				// Jumps
				INSDEF(JSR, {
					Word SubAddr = FetchWord(Cycles, memory);
					PushPCMinusOneToStack(Cycles, memory);
					PC = SubAddr;
					Cycles--;
				})

				INSDEF(RTS, {
					Word ReturnAddress = PopWordFromStack(Cycles, memory);
					PC = ReturnAddress + 1;
					Cycles -= 2;
				})

				INSDEF(JMP_ABS, {
					Word Address = AddrAbsolute(Cycles, memory);
					PC = Address;
				})

				INSDEF(JMP_IND, {
					Word Address = AddrAbsolute(Cycles, memory);
					Address = ReadWord(Cycles, Address, memory);
					PC = Address;
				})

				// Etc.
				INSDEF(NOP,	EMPTY)

				// STA

				INSDEF(STA_ZP, STOREREG(AddrZeroPage, A))

				INSDEF(STA_ZPX, STOREREG(AddrZeroPageX, A))

				INSDEF(STA_ABS, STOREREG(AddrAbsolute, A))

				INSDEF(STA_ABSX, STOREREG(AddrAbsoluteX, A))

				INSDEF(STA_ABSY, STOREREG(AddrAbsoluteY, A))

				INSDEF(STA_INDX, STOREREG(AddrIndirectX, A))

				INSDEF(STA_INDY, STOREREG(AddrIndirectY, A))

				// STX

				INSDEF(STX_ZP, STOREREG(AddrZeroPage, X))

				INSDEF(STX_ZPY, STOREREG(AddrZeroPageY, X))

				INSDEF(STX_ABS, STOREREG(AddrAbsolute, X))

				// STY

				INSDEF(STY_ZP, STOREREG(AddrZeroPage, Y))

				INSDEF(STY_ZPX, STOREREG(AddrZeroPageY, Y))

				INSDEF(STY_ABS, STOREREG(AddrAbsolute, Y))

				// Stack
				INSDEF(TSX, {
					X = SP;
					Cycles--;
					SetZeroAndNegativeFlags(X);
				})

				INSDEF(TXS, {
					SP = X;
					Cycles--;
				})

				INSDEF(PHA, PushByteOntoStack(Cycles, A, memory);)

				INSDEF(PLA, {
					A = PopByteFromStack(Cycles, memory);
					SetZeroAndNegativeFlags(A);
					Cycles--;
				})

				INSDEF(PHP, PushPSToStack(Cycles, memory);)

				INSDEF(PLP, {
					PopPSFromStack(Cycles, memory);
					Cycles--;
				})

				// Compare

				INSDEF(CMP, {
					Byte Operand = FetchByte(Cycles, memory);
					RegisterCompare(Operand, A);
				})

				INSDEF(CMP_ZP, COMPARE(AddrZeroPage))

				INSDEF(CMP_ZPX, COMPARE(AddrZeroPageX))

				INSDEF(CMP_ABS, COMPARE(AddrAbsolute))

				INSDEF(CMP_ABSX, COMPARE(AddrAbsoluteX))

				INSDEF(CMP_ABSY, COMPARE(AddrAbsoluteY))

				INSDEF(CMP_INDX, COMPARE(AddrIndirectX))

				INSDEF(CMP_INDY, COMPARE(AddrIndirectY))

				INSDEF(CLC, CFLAG(C))

				INSDEF(SEC, SFLAG(C))

				INSDEF(CLD, CFLAG(D))

				INSDEF(SED, SFLAG(D))

				INSDEF(CLI, CFLAG(I))

				INSDEF(SEI, SFLAG(I))

				INSDEF(CLV, CFLAG(V))

				INSDEF(INX, {
					X=X+1;
					Cycles--;
					SetZeroAndNegativeFlags(X);
				})

				INSDEF(INY, {
					Y++;
					Cycles--;
					SetZeroAndNegativeFlags(Y);
				})

				INSDEF(DEX, {
					X++;
					Cycles--;
					SetZeroAndNegativeFlags(X);
				})

				INSDEF(DEY, {
					Y++;
					Cycles--;
					SetZeroAndNegativeFlags(Y);
				})

				DEFAULT
			}
		}
		return Cycles;
	}

};

void clear() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

int main()
{
	CPU cpu;
	MEM mem;

	SetConsoleTitle(L"CONHOST.EXE - 6402-EMULATOR\\WINDOW");


	Byte ResetProcess[16];

	ResetProcess[0] = OPCODE(LDA_IM);
	ResetProcess[1] = 0x55;
	ResetProcess[2] = OPCODE(HOSTCALL);

	LOADINGTEXT("Loading Files", 8, 888);

	int no = {};
	std::ifstream fin("C:\\Users\\Joscha\\source\\repos\\Emulator\\BIN\\RESET_PROCESS.BIN", std::ios::in | std::ios::binary);

	if (!fin)
	{
		std::cout << "The Importentd File 'RESET_PROCESS.BIN' is missing!\n";
		Sleep(TIMEOUT * 2);
		return 1;
	}
	std::cout << sizeof(fin.gcount());
	fin.read((char*)ResetProcess, 16);

	std::cout << "Reset Process Max: " << RESET_PROCESS_MAX << std::endl;

	cpu.Reset(mem);

	LOADINGTEXT("Loading Files into Memory", 4, 1000)

	memcpy((&mem[PC_ENTRY]), ResetProcess, 16 * sizeof(Byte));
	

	LOADINGTEXT("Preparing", 5, 1000)

	char* input = {};

	while (true)
	{
		//system("cls -A -T (?)");
		/*COORD p = {0, 0};
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);*/
		

		//FillRect(GetDC(GetConsoleWindow()), &rect, (HBRUSH)COLOR_WINDOWTEXT);
		printf("%d\n", cpu.X);

		cpu.Execute(16, mem);

		cpu.X += 1;

		Sleep(260.8);
	}

	for (;;);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
