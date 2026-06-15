#pragma once

#include "MG_include.h"

#define MG_TEXT(x) TEXT(x)

typedef struct MG_String
{
	wchar_t* data;
	uint32_t length;
	bool free_flag;
}
MG_String;