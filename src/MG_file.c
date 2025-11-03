#include "MG_file.h"

byte* MG_asset_load(FILE* file, const char* path, uint32_t index)
{
	if (!path || !file)
	{
		printf("Error: Failed to load asset: path is NULL\n");
		return NULL;
	}

	char header[6];
	byte* asset_data = NULL;
	uint32_t asset_count = 0;
	size_t file_size = 0;

	rewind(file);
	fread(header, sizeof(char), sizeof(header), file);
	if (memcmp(header, "MGINEA", 6) != 0)
	{
		printf("Error: Invalid asset file format: %s\n", path);
		goto fail;
	}

	fread(&asset_count, sizeof(uint32_t), 1, file);
	if (index >= asset_count)
	{
		printf("Error: Asset index out of bounds: %u (max %u)\n", index, asset_count - 1);
		goto fail;
	}
	if (asset_count > 0xFFFF)
	{
		printf("Error: Asset count exceeds maximum limit: %u (max 65535)\n", asset_count);
		goto fail;
	}

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	rewind(file);

	if(fseek(file, index * sizeof(uint64_t) + sizeof(uint32_t) + sizeof(header), SEEK_SET))
	{
		printf("Error: Failed to seek to asset index %u in file: %s\n", index, path);
		goto fail;
	}

	uint64_t asset_offset = 0;
	uint64_t asset_length = 0;
	fread(&asset_offset, sizeof(uint64_t), 1, file);
	// asset length temporarily stores the offset of the next asset
	fread(&asset_length, sizeof(uint64_t), 1, file);
	if (asset_offset >= file_size)
	{
		printf("Error: Invalid asset offset for index %u: %llu\n", index, asset_offset);
		goto fail;
	}

	if (index == asset_count - 1)
	{
		asset_length = file_size - asset_offset;
	}
	else
	{
		asset_length = asset_length - asset_offset;
	}

	fseek(file, asset_offset, SEEK_SET);
	asset_data = malloc((size_t)asset_length);
	if (!asset_data)
	{
		printf("Error: Failed to allocate memory for asset data\n");
		goto fail;
	}

	size_t read_bytes = fread(asset_data, 1, (size_t)asset_length, file);
	if (read_bytes != (size_t)asset_length)
	{
		printf("Error: Failed to read asset data from file: %s\n", path);
		free(asset_data);
		goto fail;
	}

	return asset_data;

fail:
	return NULL;
}

int MG_load_game(void)
{

}