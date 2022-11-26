#pragma once
#include <stdio.h>

template <typename T>

class TESTBASE
{
public:
	operator&() {
		return T&;
	}

private:
};
