#pragma once
#include "types.h"

struct MEM
{
	static constexpr u32 MAX_MEM = 1024 * 64;
	Byte Data[MAX_MEM];

	void Initialise()
	{
		for (u32 i = 0; i < MAX_MEM; i++)
		{
			Data[i] = 0;
		}
	}

	/** read 1 byte */
	Byte operator[](u32 Address) const
	{
		// assert here Address is < MAX_MEM
		return Data[Address];
	}

	/** write 1 byte */
	Byte& operator[](u32 Address)
	{
		// assert here Address is < MAX_MEM
		return Data[Address];
	}
};