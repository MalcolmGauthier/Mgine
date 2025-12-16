#include "MG_file.h"

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

byte* MG_asset_load(FILE* file, MG_Asset* asset)
{
	bool close_file = false;

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
	uint32_t asset_count = 0;
	size_t file_size = 0;

	rewind(file);
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

	asset->asset_file_loaded = false;
}


static void MG_load_shader_code(FILE* file, char** shader_codes, uint32_t shader_code_count)
{
	for (uint32_t i = 0; i < shader_code_count; i++)
	{
		uint32_t code_size = 0;
		if (fread(&code_size, sizeof(uint32_t), 1, file) != 1)
		{
			printf("Error: Failed to read shader code size for index %u\n", i);
			return;
		}
		if (code_size > MG_SHADER_CODE_SIZE_MAX)
		{
			printf("Error: Shader code size exceeds maximum limit: %u (max %u)\n", code_size, MG_SHADER_CODE_SIZE_MAX);
			return;
		}
		shader_codes[i] = malloc(code_size + 1);
		if (!shader_codes[i])
		{
			printf("Error: Failed to allocate memory for shader code\n");
			return;
		}
		if (fread(shader_codes[i], sizeof(char), code_size, file) != code_size)
		{
			printf("Error: Failed to read shader code for index %u\n", i);
			free(shader_codes[i]);
			shader_codes[i] = NULL;
			return;
		}
		shader_codes[i][code_size] = '\0';
	}
}

static void MG_load_shaders(FILE* file, MG_Instance* instance, char** shader_codes, uint32_t shader_code_count, uint32_t shader_count)
{
	instance->shader_count = shader_count;
	instance->shader_list = calloc(shader_count, sizeof(MG_Shader));
	if (!instance->shader_list)
	{
		printf("Error: Failed to allocate memory for shaders\n");
		instance->active = false;
		return;
	}
	
	for (uint32_t i = 0; i < shader_count; i++)
	{
		MG_Shader* shader = &instance->shader_list[i];
		uint32_t vertex_code_index = 0;
		uint32_t fragment_code_index = 0;
		fread(&vertex_code_index, sizeof(uint32_t), 1, file);
		fread(&fragment_code_index, sizeof(uint32_t), 1, file);
		if (vertex_code_index >= shader_code_count || fragment_code_index >= shader_code_count)
		{
			printf("Error: Invalid shader code index in shader %u\n", i);
			vertex_code_index = 0;
			fragment_code_index = 0;
		}
		*shader = MG_shader_create(shader_codes[vertex_code_index], shader_codes[fragment_code_index]);

		uint16_t define_count = 0;
		fread(&define_count, sizeof(uint16_t), 1, file);
		if (define_count > MG_SHADER_DEFINE_MAX)
		{
			printf("Error: Shader define count exceeds maximum limit: %u (max %u) in shader %u\n", define_count, MG_SHADER_DEFINE_MAX, i);
			instance->active = false;
			return;
		}
		for (uint16_t j = 0; j < define_count; j++)
		{
			uint8_t define_type = 255;
			fread(&define_type, sizeof(uint8_t), 1, file);
			uint16_t define_length = 0;
			fread(&define_length, sizeof(uint16_t), 1, file);
			if (define_length == 0 || define_length > 255)
			{
				printf("Error: Invalid shader define length in shader %u. FILE CORRUPTION MAY HAVE OCCURED AND CAUSED FOLLOWING ERRORS.\n", i);
				fseek(file, define_length, SEEK_CUR);
				continue;
			}
			char* define_name = malloc(define_length + 1);
			if (!define_name)
			{
				printf("Error: Failed to allocate memory for shader define name in shader %u\n", i);
				fseek(file, define_length, SEEK_CUR);
				instance->active = false;
				return;
			}
			fread(define_name, sizeof(char), define_length, file);
			define_name[define_length] = '\0';
			
			if (define_type == 0)
				MG_shader_define(&shader->vertex_shader_code, 1, define_name);
			else if (define_type == 1)
				MG_shader_define(&shader->fragment_shader_code, 1, define_name);
			else
				printf("Error: Invalid shader define type of %u in shader %u\n", define_type, i);

			free(define_name);
		}

		if (MG_shader_compile(shader))
			shader->ID = 0;
	}
}

static void* MG_load_asset_list(FILE* file, uint32_t asset_count, size_t asset_size, size_t path_offset, size_t index_offset)
{
	if (asset_count == 0)
		return NULL;

	void* asset_list = calloc(asset_count, asset_size);
	if (!asset_list)
	{
		printf("Error: Failed to allocate memory for asset list. Asset of size %u.\n", (uint32_t)asset_size);
		return NULL;
	}

	byte* current_asset = (byte*)asset_list;
	for (uint32_t i = 0; i < asset_count; i++)
	{
		uint16_t path_length = 0;
		if (fread(&path_length, sizeof(uint16_t), 1, file) != 1)
		{
			printf("Error: Failed to read asset path length in asset %u.\n", i);
			// cleanup partial allocations not handled here
			return asset_list;
		}
		if (path_length == 0 || path_length > MG_PATH_MAX)
		{
			printf("Error: Invalid asset path length in asset %u. FILE CORRUPTION MAY HAVE OCCURED AND CAUSED FOLLOWING ERRORS.\n", i);
			// skip the path and index if possible
			fseek(file, path_length + sizeof(uint32_t), SEEK_CUR);
			current_asset += asset_size;
			continue;
		}
		char* asset_path = malloc(path_length + 1);
		if (!asset_path)
		{
			printf("Error: Failed to allocate memory for asset path in asset %u\n", i);
			fseek(file, path_length + sizeof(uint32_t), SEEK_CUR);
			current_asset += asset_size;
			continue;
		}
		if (fread(asset_path, sizeof(char), path_length, file) != path_length)
		{
			printf("Error: Failed to read asset path in asset %u\n", i);
			free(asset_path);
			current_asset += asset_size;
			continue;
		}
		asset_path[path_length] = '\0';

		char** path_ptr = (char**)(current_asset + path_offset);
		*path_ptr = asset_path;

		uint32_t* index_ptr = (uint32_t*)(current_asset + index_offset);
		if (fread(index_ptr, sizeof(uint32_t), 1, file) != 1)
		{
			printf("Error: Failed to read asset index for asset %u\n", i);
			// leave index as zero
		}

		current_asset += asset_size;
	}

	return asset_list;
}

static void MG_load_materials(FILE* file, MG_Instance* instance, uint32_t material_count)
{
	instance->material_list = calloc(material_count, sizeof(MG_Material*));
	if (!instance->material_list)
	{
		printf("Error: Failed to allocate memory for materials\n");
		instance->active = false;
		return;
	}

	for (uint32_t i = 0; i < material_count; i++)
	{
		MG_Material* material;
		uint32_t material_extra_size = 0;
		uint32_t backup_size = 0;
		fread(&material_extra_size, sizeof(uint32_t), 1, file);
		if (material_extra_size > MG_MATERIAL_SIZE_MAX)
		{
			printf("Warning: Material size exceeds maximum limit: %u (max %u). CORRUPTION MAY HAVE OCCURED.\n", material_extra_size, MG_MATERIAL_SIZE_MAX);
			backup_size = material_extra_size;
			material_extra_size = MG_MATERIAL_SIZE_MAX;
		}
		material = calloc(1, material_extra_size + sizeof(MG_Material));
		if (!material)
		{
			printf("Error: Failed to allocate memory for material %u\n", i);
			instance->active = false;
			return;
		}
		instance->material_list[i] = material;

		fread(material->diffuse_rgba, sizeof(float), 4, file);
		glm_vec4_clamp(material->diffuse_rgba, 0, 1);
		uint32_t diffuse_texture_index = 0;
		fread(&diffuse_texture_index, sizeof(uint32_t), 1, file);
		if (diffuse_texture_index < instance->texture_count && instance->texture_list)
			material->diffuse_texture = &instance->texture_list[diffuse_texture_index];
		fread(&material->contains_transparency, sizeof(uint8_t), 1, file);
		if (material->contains_transparency > 1)
			material->contains_transparency = 1;
		uint32_t shader_index = 0;
		fread(&shader_index, sizeof(uint32_t), 1, file);
		if (shader_index < instance->shader_count && instance->shader_list)
			material->shader = &instance->shader_list[shader_index];

		byte* material_end = (byte*)material + sizeof(MG_Material);
		fread(material_end, 1, material_extra_size, file);
		if (backup_size > material_extra_size)
			fseek(file, backup_size - material_extra_size, SEEK_CUR);

		uint16_t svt_count = 0;
		fread(&svt_count, sizeof(uint16_t), 1, file);
		if (svt_count > MG_MATERIAL_SVT_MAX)
		{
			printf("Warning: Material shader variable type count exceeds maximum limit: %u (max %u).\n", svt_count, MG_MATERIAL_SVT_MAX);
			instance->active = false;
			return;
		}
		material->shader_variable_count = svt_count;
		material->shader_variables = calloc(svt_count, sizeof(MG_MaterialShaderVariable));
		if (!material->shader_variables)
		{
			printf("Error: Failed to allocate memory for material shader variables in material %u\n", i);
			instance->active = false;
			return;
		}
		for (uint16_t j = 0; j < svt_count; j++)
		{
			MG_MaterialShaderVariable* svt = &material->shader_variables[j];
			uint16_t var_name_length = 0;
			fread(&var_name_length, sizeof(uint16_t), 1, file);
			if (var_name_length == 0 || var_name_length > MG_MATERIAL_VAR_NAME_MAX)
			{
				printf("Warning: Material shader variable name length exceeds maximum limit: %u (max %u) in material %u.\n", var_name_length, MG_MATERIAL_VAR_NAME_MAX, i);
				fseek(file, var_name_length + sizeof(uint32_t) * 2, SEEK_CUR);
				continue;
			}
			svt->name = malloc(var_name_length + 1);
			if (!svt->name)
			{
				printf("Error: Failed to allocate memory for material shader variable name in material %u\n", i);
				instance->active = false;
				return;
			}
			fread((char*)svt->name, sizeof(char), var_name_length, file);
			((char*)svt->name)[var_name_length] = '\0';
			fread(&svt->type, sizeof(uint32_t), 1, file);
			fread(&svt->size, sizeof(uint32_t), 1, file);
			fread(&svt->offset_in_material, sizeof(uint32_t), 1, file);
			MG_material_register_variable(material, svt->name, svt->type, svt->offset_in_material);
		}
	}
}

static void MG_load_objects(FILE* file, MG_Instance* instance, MG_Object* parent, MG_Object_LL** output)
{
	uint32_t obj_count = 0;
	fread(&obj_count, sizeof(uint32_t), 1, file);
	if (obj_count > MG_PREFAB_OBJECTS_MAX)
	{
		printf("Error: Prefab count exceeds maximum limit: %u (max %u)\n", obj_count, MG_PREFAB_MAX);
		instance->active = false;
		return;
	}
	if (!parent && output == NULL)
	{
		instance->prefab_list = calloc(obj_count, sizeof(MG_Object));
		if (!instance->prefab_list)
		{
			printf("Error: Failed to allocate memory for prefabs\n");
			instance->active = false;
			return;
		}
		instance->prefab_count = obj_count;
	}

	for (uint32_t i = 0; i < obj_count; i++)
	{
		uint32_t prefab_name_len = 0;
		fread(&prefab_name_len, sizeof(uint32_t), 1, file);

		if (prefab_name_len == 0)
		{
			uint32_t index = 0;
			fread(&index, sizeof(uint32_t), 1, file);
			if (index >= instance->prefab_count)
			{
				printf("Error: Invalid prefab reference index %u\n", index);
				continue;
			}

			// If loading into a parent or output list, create an untracked copy and add to the list
			if (parent || output)
			{
				MG_Object* copy = MG_object_create_untracked_copy(&instance->prefab_list[index]);
				if (!copy)
				{
					printf("Error: Failed to create untracked copy of prefab %u\n", index);
					instance->active = false;
					return;
				}
				if (parent)
					MG_LL_add(&parent->children, copy);
				else if (output)
					MG_LL_add(output, copy);
			}
			else
			{
				MG_Object* copy = MG_object_create_untracked_copy(&instance->prefab_list[index]);
				if (!copy)
				{
					printf("Error: Failed to create untracked copy of prefab %u\n", index);
					instance->active = false;
					return;
				}
				instance->prefab_list[i] = *copy;
				// Free the temporary allocation but do not deep free children pointers
				free(copy);
			}

			continue;
		}

		// Creating a new prefab or object
		MG_Object* prefab;
		if (parent || output)
		{
			prefab = calloc(1, sizeof(MG_Object));
			if (!prefab)
			{
				printf("Error: Failed to allocate memory for prefab object\n");
				instance->active = false;
				return;
			}
			prefab->parent = parent;
			if (parent)
				MG_LL_add(&parent->children, prefab);
			else if (output)
				MG_LL_add(output, prefab);
		}
		else
		{
			prefab = &instance->prefab_list[i];
		}

		if (prefab_name_len > MG_PREFAB_NAME_MAX)
		{
			fseek(file, prefab_name_len, SEEK_CUR);
			prefab_name_len = 0;
		}
		char* name = malloc(prefab_name_len + 1);
		if (!name)
		{
			printf("Error: Failed to allocate memory for prefab names\n");
			instance->active = false;
			return;
		}
		fread(name, sizeof(char), prefab_name_len, file);
		name[prefab_name_len] = '\0';
		prefab->id = MG_ID_get_id(name);

		fread(&prefab->flags, sizeof(uint32_t), 1, file);

		uint16_t component_count = 0;
		fread(&component_count, sizeof(uint16_t), 1, file);
		if (component_count > MG_PREFAB_COMPONENTS_MAX)
		{
			printf("Error: Too many components in prefab %u.", i);
			instance->active = false;
			return;
		}
		for (uint16_t j = 0; j < component_count; j++)
		{
			uint32_t component_name_len = 0;
			fread(&component_name_len, sizeof(uint32_t), 1, file);
			if (component_name_len > MG_PREFAB_COMPONENT_NAME_MAX)
			{
				fseek(file, component_name_len, SEEK_CUR);
				component_name_len = 0;
			}
			char* component_name = malloc(component_name_len + 1);
			if (!component_name)
			{
				printf("Error: Failed to allocate memory for component name in prefab %u\n", i);
				instance->active = false;
				return;
			}
			fread(component_name, sizeof(char), component_name_len, file);
			component_name[component_name_len] = '\0';

			uint32_t component_data_size = 0;
			fread(&component_data_size, sizeof(uint32_t), 1, file);
			byte* component_data = NULL;
			if (component_data_size > 0)
			{
				component_data = malloc(component_data_size);
				if (!component_data)
				{
					printf("Error: Failed to allocate memory for component data in prefab %u\n", i);
					instance->active = false;
					return;
				}
				// Skip pointer-sized data as per spec
				fseek(file, 2 * sizeof(void*), SEEK_CUR);
				if (component_data_size > 2 * sizeof(void*))
					fread(component_data, 1, component_data_size - 2 * sizeof(void*), file);
			}

			MG_component_create(prefab, MG_component_get_template_by_name(instance, component_name), component_data);
			free(component_name);
			if (component_data)
				free(component_data);
		}

		MG_load_objects(file, instance, prefab, NULL);
	}
}

static void MG_load_scenes(FILE* file, MG_Instance* instance, uint16_t scene_count)
{
	instance->scene_list = calloc(scene_count, sizeof(MG_Scene));
	if (!instance->scene_list)
	{
		printf("Error: Failed to allocate memory for scenes\n");
		instance->active = false;
		return;
	}

	for (uint16_t i = 0; i < scene_count; i++)
	{
		MG_Scene* scene = &instance->scene_list[i];
		uint32_t scene_name_len = 0;
		fread(&scene_name_len, sizeof(uint32_t), 1, file);
		if (scene_name_len > MG_SCENE_NAME_MAX)
		{
			fseek(file, scene_name_len, SEEK_CUR);
			scene_name_len = 0;
		}
		scene->name = malloc(scene_name_len + 1);
		if (!scene->name)
		{
			printf("Error: Failed to allocate memory for scene name\n");
			instance->active = false;
			return;
		}
		fread(scene->name, sizeof(char), scene_name_len, file);
		scene->name[scene_name_len] = '\0';
		
		MG_load_objects(file, instance, NULL, &scene->objects);

		uint32_t texture_count = 0;
		fread(&texture_count, sizeof(uint32_t), 1, file);
		if (texture_count > MG_TEXTURE_MAX)
		{
			printf("Error: Scene texture count exceeds maximum limit: %u (max %u)\n", texture_count, MG_TEXTURE_MAX);
			instance->active = false;
			return;
		}
		scene->texture_count = texture_count;
		scene->textures = calloc(texture_count, sizeof(MG_Texture*));
		for (uint32_t j = 0; j < texture_count; j++)
		{
			uint32_t texture_index = 0;
			fread(&texture_index, sizeof(uint32_t), 1, file);
			if (texture_index < instance->texture_count)
			{
				scene->textures[j] = &instance->texture_list[texture_index];
			}
		}

		uint32_t model_count = 0;
		fread(&model_count, sizeof(uint32_t), 1, file);
		if (model_count > MG_MODEL_MAX)
		{
			printf("Error: Scene model count exceeds maximum limit: %u (max %u)\n", model_count, MG_MODEL_MAX);
			instance->active = false;
			return;
		}
		scene->model_count = model_count;
		scene->models = calloc(model_count, sizeof(MG_Model*));
		for (uint32_t j = 0; j < model_count; j++)
		{
			uint32_t model_index = 0;
			fread(&model_index, sizeof(uint32_t), 1, file);
			if (model_index < instance->model_count)
			{
				scene->models[j] = &instance->model_list[model_index];
			}
		}
	}
}

int MG_load_game(MG_Instance* instance)
{
	if (!instance)
	{
		printf("Error: MG_load_game called with NULL instance\n");
		return -1;
	}

	char mg_file_path[MAX_PATH];
	DWORD len = GetModuleFileNameA(NULL, mg_file_path, MAX_PATH);
	if (len == 0 || len == MAX_PATH)
	{
		printf("Error: Failed to get module file name\n");
		return -2;
	}

	char* extension = strrchr(mg_file_path, '.');
	if (extension)
	{
		size_t remaining = MAX_PATH - (size_t)(extension - mg_file_path);
		strcpy_s(extension, remaining, ".mg");
	}
	else
		strcat_s(mg_file_path, MAX_PATH, ".mg");

	FILE* mg_file = fopen(mg_file_path, "rb");
	if (!mg_file)
	{
		printf("Error: Failed to open MG file: %s\n", mg_file_path);
		return -3;
	}

	char header[5];
	fread(header, sizeof(char), sizeof(header), mg_file);
	if (memcmp(header, "MGINE", 5) != 0)
	{
		printf("Error: Invalid MG file format: %s\n", mg_file_path);
		fclose(mg_file);
		return -4;
	}
	
	uint32_t version = 0;
	fread(&version, sizeof(uint32_t), 1, mg_file);
	if (version != 1)
	{
		printf("Error: Unsupported MG file version: %u\n", version);
		fclose(mg_file);
		return -5;
	}

	uint32_t shader_code_count = 0;
	fread(&shader_code_count, sizeof(uint32_t), 1, mg_file);
	if (shader_code_count > MG_SHADER_CODE_MAX)
	{
		printf("Error: Shader code count exceeds maximum limit: %u (max %u)\n", shader_code_count, MG_SHADER_CODE_MAX);
		fclose(mg_file);
		return -6;
	}
	char** shader_codes = calloc(shader_code_count, sizeof(char*));
	if (!shader_codes)
	{
		printf("Error: Failed to allocate memory for shader codes\n");
		fclose(mg_file);
		return -7;
	}
	MG_load_shader_code(mg_file, shader_codes, shader_code_count);
	instance->shader_code_count = shader_code_count;
	instance->shader_code_list = shader_codes;

	uint32_t shader_count = 0;
	fread(&shader_count, sizeof(uint32_t), 1, mg_file);
	if (shader_count > MG_SHADER_MAX)
	{
		printf("Error: Shader count exceeds maximum limit: %u (max %u)\n", shader_count, MG_SHADER_MAX);
		fclose(mg_file);
		return -8;
	}
	MG_load_shaders(mg_file, instance, shader_codes, shader_code_count, shader_count);

	uint32_t texture_count = 0;
	fread(&texture_count, sizeof(uint32_t), 1, mg_file);
	if (texture_count > MG_TEXTURE_MAX)
	{
		printf("Error: Texture count exceeds maximum limit: %u (max %u)\n", texture_count, MG_TEXTURE_MAX);
		fclose(mg_file);
		return -9;
	}
	instance->texture_count = texture_count;
	instance->texture_list = (MG_Texture*)MG_load_asset_list(mg_file, texture_count, sizeof(MG_Texture), offsetof(MG_Texture, base.path), offsetof(MG_Texture, base.index_in_file));

	uint32_t material_count = 0;
	fread(&material_count, sizeof(uint32_t), 1, mg_file);
	if (material_count > MG_MATERIAL_MAX)
	{
		printf("Error: Material count exceeds maximum limit: %u (max %u)\n", material_count, MG_MATERIAL_MAX);
		fclose(mg_file);
		return -10;
	}
	instance->material_count = material_count;
	MG_load_materials(mg_file, instance, material_count);

	uint32_t model_count = 0;
	fread(&model_count, sizeof(uint32_t), 1, mg_file);
	if (model_count > MG_MODEL_MAX)
	{
		printf("Error: Model count exceeds maximum limit: %u (max %u)\n", model_count, MG_MODEL_MAX);
		fclose(mg_file);
		return -11;
	}
	instance->model_count = model_count;
	instance->model_list = (MG_Model*)MG_load_asset_list(mg_file, model_count, sizeof(MG_Model), offsetof(MG_Model, base.path), offsetof(MG_Model, base.index_in_file));

	MG_load_objects(mg_file, instance, NULL, NULL);

	uint32_t sound_count = 0;
	fread(&sound_count, sizeof(uint32_t), 1, mg_file);
	if (sound_count > MG_SOUND_MAX)
	{
		printf("Error: Sound count exceeds maximum limit: %u (max %u)\n", sound_count, MG_SOUND_MAX);
		fclose(mg_file);
		return -12;
	}
	instance->sound_count = sound_count;
	instance->sound_list = (MG_Sound*)MG_load_asset_list(mg_file, sound_count, sizeof(MG_Sound), offsetof(MG_Sound, base.path), offsetof(MG_Sound, base.index_in_file));

	uint16_t scene_count = 0;
	fread(&scene_count, sizeof(uint16_t), 1, mg_file);
	if (scene_count > MG_SCENE_MAX)
	{
		printf("Error: Scene count exceeds maximum limit: %u (max %u)\n", scene_count, MG_SCENE_MAX);
		fclose(mg_file);
		return -13;
	}
	instance->scene_count = scene_count;
	MG_load_scenes(mg_file, instance, scene_count);

	fclose(mg_file);
	return 0;
}