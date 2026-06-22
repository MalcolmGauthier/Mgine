#pragma once

#include "MG_include.h"
#include "MG_asset.h"

#define MG_TEXT(x) TEXT(x)

typedef struct MG_String
{
	wchar_t* data;
	uint32_t length;
	MG_ID id;
	// determines if string should be freed when the once-per-frame cleanup happens.
	// true = string last a single frame, false = string freed when done so manually.
	// this means that strings with free_flag set do not need to be kept track of.
	bool free_flag;
}
MG_String;

MG_API MG_String* MG_stringA(const char* text);
MG_API MG_String* MG_string(const wchar_t* text);
