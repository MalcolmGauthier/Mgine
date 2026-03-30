#pragma once
#include "MG_include.h"

typedef struct MG_Asset
{
	// copy of the data from the asset file
	byte* asset_file_data;
	// size of the asset file data
	size_t asset_file_size;
	// whether the asset file data is loaded
	bool asset_file_loaded;
	// whether the data has been extracted from the file and into the parent struct
	bool loaded;

	// file path. must be dynamically alocated, freed when unloaded
	char* path;
	// index in the MG Asset file, -1 if unused
	int32_t index_in_file;
}
MG_Asset;