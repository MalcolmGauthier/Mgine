#include "MG_asset.h"
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

typedef struct
{
	enum
	{
		MG_YAJL_INIT,
		MG_YAJL_SHADER_SRC,
		MG_YAJL_SHADER,
		MG_YAJL_SHADER_DEF,
		MG_YAJL_TEXTURE,
		MG_YAJL_MATERIAL,
		MG_YAJL_MATERIAL_SVT,
		MG_YAJL_MODEL,
		MG_YAJL_PREFAB,
		MG_YAJL_PREFAB_COMP,
		MG_YAJL_PREFAB_CHILDREN,
		MG_YAJL_SOUND,
		MG_YAJL_SCENE,
		MG_YAJL_SCENE_OBJ,
		MG_YAJL_SCENE_TEXTURES,
		MG_YAJL_SCENE_MODELS,
		MG_YAJL_EXIT
	} curr_step;

	void* curr_asset;
	uint32_t index;

	char* label;
}
MG_YAJL_Context;

#define MG_SHADER_CODE_MAX 1024
#define MG_SHADER_CODE_SIZE_MAX 65536
#define MG_SHADER_MAX 1024
#define MG_SHADER_DEFINE_MAX 256

#define MG_MATERIAL_MAX 4096
#define MG_MATERIAL_SIZE_MAX 65536
#define MG_MATERIAL_SVT_MAX 1024
#define MG_MATERIAL_VAR_NAME_MAX 256

#define MG_PREFAB_MAX 4096
#define MG_PREFAB_NAME_MAX 1024
#define MG_PREFAB_COMPONENTS_MAX 1024
#define MG_PREFAB_COMPONENT_NAME_MAX 1024
#define MG_PREFAB_OBJECTS_MAX 2048

#define MG_TEXTURE_MAX 32768
#define MG_MODEL_MAX 4096
#define MG_SOUND_MAX 4096
#define MG_SCENE_MAX 1024
#define MG_SCENE_NAME_MAX 1024

#define MG_PATH_MAX 1024

MG_ID MG_asset_add(struct MG_AssetList* asset_list, void* asset)
{
	if (!asset_list || !asset)
	{
		printf("Error: Null parameter(s) in MG_asset_add\n");
		return 0;
	}

	if (asset_list->next_id == 0)
	{
		asset_list->next_id++;
	}

	if (MG_hashmap_add(asset_list->assets, asset_list->next_id, asset))
	{
		return 0;
	}

	asset_list->next_id++;
	return asset_list->next_id - 1;
}

byte* MG_asset_load(FILE* file, MG_Asset* asset)
{
	bool close_file = false;

	if (!asset)
		return NULL;

	if (!file)
	{
		errno_t err = fopen_s(&file, asset->path, "rb");
		if (err)
		{
			printf("Error: Failed to open asset file: %s with error %i\n", asset->path, (int)err);
			return NULL;
		}
		close_file = true;
	}

	char header[6];
	byte* asset_data = NULL;
	int32_t asset_count = 0;
	size_t file_size = 0;

	rewind(file);

	// index less than 0 means the filepath is to an asset and not an MGA file
	if (asset->index_in_file < 0)
	{
		fseek(file, 0, SEEK_END);
		asset->asset_file_size = ftell(file);
		rewind(file);

		asset->asset_file_data = malloc(asset->asset_file_size);
		if (!asset->asset_file_data)
		{
			printf("Error: Failed to allocate memory for asset data\n");
			goto fail;
		}
		size_t read_bytes = fread(asset->asset_file_data, 1, asset->asset_file_size, file);
		if (read_bytes != asset->asset_file_size)
		{
			printf("Error: Failed to read asset data from file\n");
			free(asset->asset_file_data);
			asset->asset_file_data = NULL;
			goto fail;
		}
		asset->asset_file_loaded = true;
		return asset->asset_file_data;
	}

	fread(header, sizeof(char), sizeof(header), file);
	if (memcmp(header, "MGINEA", 6) != 0)
	{
		printf("Error: Invalid asset file format: %s\n", asset->path);
		goto fail;
	}

	fread(&asset_count, sizeof(uint32_t), 1, file);
	if (asset->index_in_file >= asset_count)
	{
		printf("Error: Asset index out of bounds: %u (max %u)\n", asset->index_in_file, asset_count - 1);
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

	if(fseek(file, asset->index_in_file * sizeof(uint64_t) + sizeof(uint32_t) + sizeof(header), SEEK_SET))
	{
		printf("Error: Failed to seek to asset index %u in file %s\n", asset->index_in_file, asset->path);
		goto fail;
	}

	uint64_t asset_offset = 0;
	uint64_t asset_length = 0;
	fread(&asset_offset, sizeof(uint64_t), 1, file);
	// asset length temporarily stores the offset of the next asset
	fread(&asset_length, sizeof(uint64_t), 1, file);
	if (asset_offset >= file_size)
	{
		printf("Error: Invalid asset offset for index %u: %llu\n", asset->index_in_file, asset_offset);
		goto fail;
	}

	if (asset->index_in_file == asset_count - 1)
	{
		asset_length = file_size - asset_offset;
	}
	else
	{
		asset_length = asset_length - asset_offset;
	}

	fseek(file, (long)asset_offset, SEEK_SET);
	asset_data = malloc((size_t)asset_length);
	if (!asset_data)
	{
		printf("Error: Failed to allocate memory for asset data\n");
		goto fail;
	}

	size_t read_bytes = fread(asset_data, 1, (size_t)asset_length, file);
	if (read_bytes != (size_t)asset_length)
	{
		printf("Error: Failed to read asset data from file\n");
		free(asset_data);
		asset_data = NULL;
		goto fail;
	}

	asset->asset_file_data = asset_data;
	asset->asset_file_size = (size_t)asset_length;
	asset->asset_file_loaded = true;

fail:
	if (close_file)
		fclose(file);
	return asset_data;
}

void MG_asset_free(MG_Asset* asset)
{
	if (asset->asset_file_data)
	{
		free(asset->asset_file_data);
		asset->asset_file_data = NULL;
	}

	if (asset->path)
	{
		free(asset->path);
		asset->path = NULL;
	}

	asset->asset_file_loaded = false;
}



int MG_load_yajl_parse_bool(void* ctx, int bool_val)
{
	return 1;
}

int MG_load_yajl_parse_int(void* ctx, long long int_val)
{
	return 1;
}

int MG_load_yajl_parse_float(void* ctx, double float_val)
{
	return 1;
}

int MG_load_yajl_parse_number(void* ctx, const char* number_val, size_t number_len)
{
	return 1;
}

int MG_load_yajl_parse_string(void* ctx, const unsigned char* string_val, size_t string_len)
{
	return 1;
}

int MG_load_yajl_parse_start_map(void* ctx)
{
	return 1;
}

int MG_load_yajl_parse_map_key(void* ctx, const unsigned char* key, size_t key_len)
{
	return 1;
}

int MG_load_yajl_parse_end_map(void* ctx)
{
	return 1;
}

int MG_load_yajl_parse_start_array(void* ctx)
{
	return 1;
}

int MG_load_yajl_parse_end_array(void* ctx)
{
	return 1;
}

int MG_load_game()
{
	if (!MG_INSTANCE)
	{
		printf("Error: MG_load_game called with NULL instance\n");
		return -1;
	}

	wchar_t mg_file_path[MAX_PATH + 4];
	DWORD len = GetModuleFileNameW(NULL, mg_file_path, MAX_PATH);
	if (len == 0 || len == MAX_PATH)
	{
		printf("Error: Failed to get module file name\n");
		return -2;
	}

	wchar_t* extension = wcsrchr(mg_file_path, L'.');
	if (extension)
	{
		size_t remaining = MAX_PATH - (size_t)(extension - mg_file_path);
		wcscpy_s(extension, remaining, L".mg");
	}
	else
	{
		wcscat_s(mg_file_path, MAX_PATH, L".mg");
	}

	FILE* mg_file = _wfopen(mg_file_path, L"rb");
	if (!mg_file)
	{
		printf("Error: Failed to open MG file: %ls\n", mg_file_path);
		return -3;
	}

	byte* file_buffer = malloc(0x10000); // 64kb
	if (!file_buffer)
	{
		printf("Error: Failed to allocate memory for file buffer\n");
		fclose(mg_file);
		return -4;
	}

	yajl_handle handle;
	MG_YAJL_Context yajl_context;
	yajl_callbacks json_callbacks =
	{
		.yajl_null = NULL,
		.yajl_boolean = MG_load_yajl_parse_bool,
		.yajl_integer = MG_load_yajl_parse_int,
		.yajl_double = MG_load_yajl_parse_float,
		.yajl_number = MG_load_yajl_parse_number,
		.yajl_string = MG_load_yajl_parse_string,

		.yajl_start_map = MG_load_yajl_parse_start_map,
		.yajl_map_key = MG_load_yajl_parse_map_key,
		.yajl_end_map = MG_load_yajl_parse_end_map,

		.yajl_start_array = MG_load_yajl_parse_start_array,
		.yajl_end_array = MG_load_yajl_parse_end_array
	};
	int error_code = 0;
	handle = yajl_alloc(&json_callbacks, NULL, &yajl_context);
	if (!yajl_config(handle, yajl_allow_comments, 1))
	{
		printf("Error: Failed to configure YAJL parser\n");
		error_code = -5;
		// special cleanup, no freeing handle
		goto cleanup;
	}

	while (true)
	{
		size_t read_bytes = fread(file_buffer, 1, sizeof(file_buffer), mg_file);
		if (read_bytes == 0)
		{
			if (feof(mg_file))
			{
				yajl_complete_parse(handle);
				break;
			}

			printf("Error: Failed to read MG file: %ls\n", mg_file_path);
			error_code = -6;
			break;
		}

		if (yajl_parse(handle, file_buffer, read_bytes))
		{
			unsigned char* error_message = yajl_get_error(handle, 1, file_buffer, read_bytes);
			printf("Error: Failed to parse MG file: %ls\n%s\n", mg_file_path, error_message);
			yajl_free_error(handle, error_message);
			error_code = -7;
			break;
		}
	}

	yajl_free(handle);
cleanup:
	free(file_buffer);
	fclose(mg_file);
	return error_code;
}