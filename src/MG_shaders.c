#include "MG_shaders.h"

MG_Shader* MG_shader_create(MG_Instance* instance, const char* vertex_shader, const char* fragment_shader)
{
	MG_Shader* shader = calloc(1, sizeof(MG_Shader));
	// returns error code if shader is null
	if (MG_asset_add(&instance->shader_list, &instance->shader_count, shader))
	{
		printf("Failed to add shader to instance shader list.\n");
		free(shader);
		return NULL;
	}
	
	size_t vert_len = strlen(vertex_shader);
	size_t frag_len = strlen(fragment_shader);

	// asset being null is checked in MG_asset_add, yet vs still complains
#pragma warning(suppress : 6011)
	shader->vertex_shader_code = malloc(vert_len);
	shader->fragment_shader_code = malloc(frag_len);
	if (!shader->vertex_shader_code || !shader->fragment_shader_code)
	{
		printf("Failed to allocate memory for shader code.\n");
		free(shader->vertex_shader_code);
		free(shader);
		return NULL;
	}

	memcpy(shader->vertex_shader_code, vertex_shader, vert_len);
	memcpy(shader->fragment_shader_code, fragment_shader, frag_len);

	shader->status = MG_SHADER_STATUS_NOT_IMPLEMENTED;
	return shader;
}

MG_Shader* MG_shader_create_from_filepaths(MG_Instance* instance, const char* vertex_shader_path, const char* fragment_shader_path)
{
	char* vertex_shader_code = NULL;
	char* fragment_shader_code = NULL;
	size_t vert_len = 0;
	size_t frag_len = 0;

	FILE* file_vert = fopen(vertex_shader_path, "rb");
	FILE* file_frag = fopen(fragment_shader_path, "rb");
	if (!file_vert || !file_frag)
	{
		printf("Failed to open shader file: %s\n", vertex_shader_path);
		if (file_vert) fclose(file_vert);
		if (file_frag) fclose(file_frag);
		return NULL;
	}
	fseek(file_vert, 0, SEEK_END);
	fseek(file_frag, 0, SEEK_END);
	vert_len = ftell(file_vert);
	frag_len = ftell(file_frag);
	fseek(file_vert, 0, SEEK_SET);
	fseek(file_frag, 0, SEEK_SET);
	vertex_shader_code = malloc(vert_len + 1);
	fragment_shader_code = malloc(frag_len + 1);
	if (!vertex_shader_code || !fragment_shader_code)
	{
		printf("Failed to allocate memory for shader code.\n");
		free(vertex_shader_code);
		free(fragment_shader_code);
		fclose(file_vert);
		return NULL;
	}
	fread(vertex_shader_code, 1, vert_len, file_vert);
	fread(fragment_shader_code, 1, frag_len, file_frag);
	vertex_shader_code[vert_len] = '\0';
	fragment_shader_code[frag_len] = '\0';
	fclose(file_vert);
	fclose(file_frag);

	MG_Shader* shader = MG_shader_create(instance, vertex_shader_code, fragment_shader_code);
	free(vertex_shader_code);
	free(fragment_shader_code);
	return shader;
}

int MG_shader_define(char** shader_file_text_ref, int define_count, ...)
{
	// defines is the etire block of text we want to add
	char* defines = calloc(1, sizeof(char));
	if (!defines)
	{
		printf("Failed to allocate memory for shader defines.\n");
		return -1;
	}

	va_list args;
	va_start(args, define_count);

	size_t defines_len = 0;

	for (int i = 0; i < define_count; i++)
	{
		const char* define = va_arg(args, const char*);

		if (!define) continue;

		if (isdigit(define[0]))
		{
			printf("Shader define cannot start with a digit: %c\n", define[0]);
			continue;
		}

		size_t len = strlen(define);
		if (len == 0 || len > 255)
		{
			printf("Shader define must be between 1 and 255 chars long\n");
			continue;
		}

		bool good = true;
		for (size_t j = 0; j < len; j++)
		{
			if (!(isalnum(define[j]) || define[j] == '_'))
			{
				printf("Invalid character in shader define: %c\n", define[j]);
				good = false;
				break;
			}
		}
		if (!good) continue;

		// we then go line by line adding on the defines to the defines variable
		size_t new_size = defines_len + len + 10 + 1;
		void* temp = realloc(defines, new_size);
		if (!temp)
		{
			printf("Failed to allocate memory for shader defines (-2)\n");
			free(defines);
			va_end(args);
			return -2;
		}
		defines = temp;

		strcat_s(defines, new_size, "#define ");
		strcat_s(defines, new_size, define);
		strcat_s(defines, new_size, "\n");

		defines_len = strlen(defines);
	}

	va_end(args);

	size_t shader_file_size = strlen(*shader_file_text_ref) + 1;
	char* final_shader = calloc(defines_len + shader_file_size, 1);
	if (!final_shader)
	{
		printf("Failed to allocate memory for shader defines (-3)\n");
		free(defines);
		return -3;
	}

	strcpy_s(final_shader, defines_len + shader_file_size, defines);
	strcat_s(final_shader, defines_len + shader_file_size, *shader_file_text_ref);

	free(*shader_file_text_ref);
	*shader_file_text_ref = final_shader;

	free(defines);
	return 0;
}

int MG_shader_compile(MG_Shader* shader)
{
	if (!shader)
	{
		printf("Failed to compile shader: shader is NULL\n");
		return -1;
	}

	GLuint vertex_shader = 0;
	GLuint fragment_shader = 0;

	if (shader->ID)
	{
		printf("WARNING: shader already compiled. recompiling.\n");
		glDeleteProgram(shader->ID);
		shader->ID = 0;
	}

	shader->ID = glCreateProgram();
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	int success;

	glShaderSource(vertex_shader, 1, &shader->vertex_shader_code, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
		printf("ERROR compiling vertex shader: %s\n", infoLog);
		shader->status = MG_SHADER_STATUS_ERROR;
		return -2;
	}

	glShaderSource(fragment_shader, 1, &shader->fragment_shader_code, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
		glDeleteShader(vertex_shader);
		printf("ERROR compiling fragment shader: %s\n", infoLog);
		shader->status = MG_SHADER_STATUS_ERROR;
		return -3;
	}

	glAttachShader(shader->ID, vertex_shader);
	glAttachShader(shader->ID, fragment_shader);
	glLinkProgram(shader->ID);
	glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shader->ID, 512, NULL, infoLog);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		printf("ERROR linking shader: %s\n", infoLog);
		shader->status = MG_SHADER_STATUS_ERROR;
		return -4;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	free(shader->vertex_shader_code);
	free(shader->fragment_shader_code);
	shader->status = MG_SHADER_STATUS_OK;
	return 0;
}


int MG_material_register_variable(MG_Material* material, const char* name, GLenum type, uint32_t offset_in_material)
{
	if (!material || !material->shader || !name)
	{
		printf("Failed to register material shader variable: material, name or material shader is NULL\n");
		return -1;
	}

	if (material->shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Failed to register material shader variable: material shader is not compiled\n");
		return -2;
	}

	if (offset_in_material > _msize(material))
	{
		printf("Failed to register material shader variable: offset_in_material is out of bounds\n");
		return -3;
	}

	size_t len = strlen(name);
	// check against empty string, starting with digit, or containing invalid chars
	if (len == 0 || isdigit(name[0]) || len == strspn(name, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"))
	{
		printf("Failed to register material shader variable: name is invalid variable name\n");
		return -4;
	}

	int size;
	switch (type)
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
		size = 1;
		break;

	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
		size = 2;
		break;

	case GL_FLOAT:
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_BOOL:
		size = 4;
		break;

	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
	case GL_BOOL_VEC2:
		size = 8;
		break;

	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
	case GL_BOOL_VEC3:
		size = 12;
		break;

	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
	case GL_BOOL_VEC4:
		size = 16;
		break;

	case GL_FLOAT_MAT2:
		size = 16;
		break;
	case GL_FLOAT_MAT3:
		size = 36;
		break;
	case GL_FLOAT_MAT4:
		size = 64;
		break;

	case GL_DOUBLE:
		size = 8;
		break;
	case GL_DOUBLE_VEC2:
		size = 16;
		break;
	case GL_DOUBLE_VEC3:
		size = 24;
		break;
	case GL_DOUBLE_VEC4:
		size = 32;
		break;

	case GL_DOUBLE_MAT2:
		size = 32;
		break;
	case GL_DOUBLE_MAT3:
		size = 72;
		break;
	case GL_DOUBLE_MAT4:
		size = 128;
		break;

	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_CUBE_MAP_ARRAY:
	case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
	case GL_SAMPLER_CUBE:
		size = 4;
		break;

	default:
		printf("Failed to register shader variable: unsupported type %u\n", type);
		return -6;
	}

	if (size + offset_in_material > _msize(material))
	{
		printf("Failed to register material shader variable: variable size is out of bounds\n");
		return -5;
	}

	void* bak = material->shader_variables;
	material->shader_variables = realloc(material->shader_variables, material->shader_variable_count * sizeof(MG_MaterialShaderVariable));
	if (!material->shader_variables)
	{
		printf("Failed to reallocate memory for material shader variable translation unit\n");
		material->shader_variables = bak;
		return -5;
	}
	material->shader_variable_count++;

	MG_MaterialShaderVariable* var = &material->shader_variables[material->shader_variable_count - 1];
	var->size = size;
	var->name = name;
	var->type = type;
	var->offset_in_material = offset_in_material;

	return 0;
}

void MG_material_free(MG_Material* material)
{
	if (!material)
		return;

	free(material->shader_variables);
	material->shader_variables = NULL;

	if (material->free_func)
		material->free_func(material);
}


void MG_shader_use(MG_Shader* shader)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to use shader: shader is NULL or not compiled\n");
		return;
	}

	glUseProgram(shader->ID);
}

void MG_shader_free(MG_Shader* shader)
{
	if (!shader)
		return;

	if (shader->ID)
	{
		glDeleteProgram(shader->ID);
		shader->ID = 0;
	}

	free(shader);
	shader = NULL;
}


void MG_shader_set_int(MG_Shader* shader, const char* name, int value)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader int: shader is NULL or not compiled\n");
		return;
	}
	glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void MG_shader_set_ivec2(MG_Shader* shader, const char* name, int* vec)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader ivec2: shader is NULL or not compiled\n");
		return;
	}
	glUniform2iv(glGetUniformLocation(shader->ID, name), 1, vec);
}

void MG_shader_set_ivec3(MG_Shader* shader, const char* name, int* vec)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader ivec3: shader is NULL or not compiled\n");
		return;
	}
	glUniform3iv(glGetUniformLocation(shader->ID, name), 1, vec);
}

void MG_shader_set_float(MG_Shader* shader, const char* name, float value)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader float: shader is NULL or not compiled\n");
		return;
	}
	glUniform1f(glGetUniformLocation(shader->ID, name), value);
}

void MG_shader_set_vec2(MG_Shader* shader, const char* name, MG_Vec2 vec)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader vec2: shader is NULL or not compiled\n");
		return;
	}
	glUniform2fv(glGetUniformLocation(shader->ID, name), 1, (const GLfloat*)&vec);
}

void MG_shader_set_vec3(MG_Shader* shader, const char* name, MG_Vec3 vec)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader vec3: shader is NULL or not compiled\n");
		return;
	}
	glUniform3fv(glGetUniformLocation(shader->ID, name), 1, (const GLfloat*)&vec);
}

void MG_shader_set_mat4(MG_Shader* shader, const char* name, MG_Matrix* mat)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader mat4: shader is NULL or not compiled\n");
		return;
	}
	
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, false, (const GLfloat*)mat);
}