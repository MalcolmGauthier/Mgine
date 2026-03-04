#pragma once
#include "MG_include.h"

typedef struct MG_Asset
{
	byte* asset_file_data;	// copy of the data from the asset file
	size_t asset_file_size; // size of the asset file data
	bool asset_file_loaded; // whether the asset file data is loaded
	bool loaded;			// whether the data has been extracted from the file and into the parent struct

	char* path;				// file path
	int32_t index_in_file;  // index in the MG Asset file, -1 if unused
}
MG_Asset;