#pragma once
#include "MG_include.h"

typedef struct MG_Asset
{
	byte* asset_file_data;
	size_t asset_file_size;
	bool asset_file_loaded;
	bool loaded;

	char* path;
	int32_t index_in_file;
}
MG_Asset;