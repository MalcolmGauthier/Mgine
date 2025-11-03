#include "MG_textures.h"

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

MG_Texture* MG_texture_init(const char* path, uint32_t index_in_file)
{
	MG_Texture* texture = calloc(1, sizeof(MG_Texture));
	if (!texture)
	{
		printf("Failed to allocate memory for texture\n");
		return NULL;
	}

	texture->path = path;
	texture->index_in_file = index_in_file;
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

	if (!texture->path)
	{
		printf("Error: Failed to load texture: path is NULL\n");
		return -2;
	}
	
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);

	int width, height, channel_cnt;
	stbi_uc* texture = stbi_load(texture->path, &width, &height, &channel_cnt, 0);
	if (!texture)
	{
		printf("Warning: Failed to load texture from path: %s\n", texture->path);
		if (_MG_default_texture[0] == 0)
			MG_texture_init_default();
		texture = _MG_default_texture;
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, color_type, GL_UNSIGNED_BYTE, texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (texture != _MG_default_texture)
		stbi_image_free(texture);

	return 0;
}