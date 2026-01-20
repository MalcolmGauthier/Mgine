#include "MG_textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi.h"
#undef STB_IMAGE_IMPLEMENTATION

uint8_t _MG_default_texture[_MG_default_tex_width * _MG_default_tex_height * 3] = { 0 };
static void MG_texture_init_default(void)
{
	memset(_MG_default_texture, 0xFF, sizeof(_MG_default_texture));
	for (int i = 2; i < sizeof(_MG_default_texture) - 1; i += 3)
		_MG_default_texture[i] = 0;
	for (int i = 0; i < _MG_default_tex_height / 2; i++)
		for (int j = _MG_default_tex_width / 2; j < _MG_default_tex_width; j++)
		{
			_MG_default_texture[i * _MG_default_tex_height * 3 + j * 3 + 0] = 0;
			_MG_default_texture[i * _MG_default_tex_height * 3 + j * 3 + 1] = 0;
			_MG_default_texture[i * _MG_default_tex_height * 3 + j * 3 + 2] = 0;
		}
	for (int i = _MG_default_tex_height / 2; i < _MG_default_tex_height; i++)
		for (int j = 0; j < _MG_default_tex_width / 2; j++)
		{
			_MG_default_texture[i * _MG_default_tex_height * 3 + j * 3 + 0] = 0;
			_MG_default_texture[i * _MG_default_tex_height * 3 + j * 3 + 1] = 0;
			_MG_default_texture[i * _MG_default_tex_height * 3 + j * 3 + 2] = 0;
		}
}

MG_Texture* MG_texture_init(MG_Instance* instance, const char* path)
{
	return MG_texture_init_MGA(instance, path, -1);
}

MG_Texture* MG_texture_init_MGA(MG_Instance* instance, const char* path, int32_t index_in_file)
{
	MG_Texture* texture;

	MG_Texture* new_list = realloc(instance->texture_list, sizeof(MG_Texture*) * (instance->texture_count + 1));
	if (!new_list)
	{
		printf("Failed to allocate memory for new texture metadata\n");
		return NULL;
	}
	instance->texture_list = new_list;
	texture = &instance->texture_list[instance->texture_count];
	instance->texture_count++;

	texture->base.path = (char*)path;
	texture->base.index_in_file = index_in_file;
	texture->id = 0;

	return texture;
}

int MG_texture_load(MG_Texture* texture)
{
	if (!texture)
	{
		printf("Error: Failed to load texture: texture is NULL\n");
		return -1;
	}

	if (texture->id != 0)
	{
		printf("Warning: Texture already loaded.");
		return 0;
	}

	if (!texture->base.asset_file_loaded)
	{
		printf("Warning: Texture is not loaded. Loading texture.\n");
		MG_asset_load(NULL, &texture->base);
	}
	
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);

	int width, height, channel_cnt;
	stbi_uc* pixels = stbi_load_from_memory(texture->base.asset_file_data, (int)texture->base.asset_file_size, &width, &height, &channel_cnt, 0);
	if (!pixels)
	{
		printf("Warning: Failed to load texture from path: %s, %u\n", texture->base.path, texture->base.index_in_file);
		if (_MG_default_texture[0] == 0)
			MG_texture_init_default();
		pixels = _MG_default_texture;
		width = _MG_default_tex_width;
		height = _MG_default_tex_height;
		channel_cnt = 3;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	GLenum color_type;
	switch (channel_cnt)
	{
	case 1:
		color_type = GL_RED;
		break;
	case 2:
		color_type = GL_RG;
		break;
	case 3:
		color_type = GL_RGB;
		break;
	case 4:
	default:
		color_type = GL_RGBA;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, color_type, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (pixels != _MG_default_texture)
		stbi_image_free(pixels);

	free(texture->base.asset_file_data);
	texture->base.loaded = true;
	texture->base.asset_file_data = NULL;
	texture->base.asset_file_loaded = false;
	texture->width = width;
	texture->height = height;
	texture->channels = (byte)channel_cnt;

	return 0;
}