#include "MG_shaders.h"

MG_Shader* MG_shader_create(const char* vertex_shader, const char* fragment_shader)
{
	MG_Shader* shader = calloc(1, sizeof(MG_Shader));
	if (!shader)
	{
		printf("Failed to allocate memory for shader\n");
		return NULL;
	}
	
	shader->vertex_shader_file = vertex_shader;
	shader->fragment_shader_file = fragment_shader;
	shader->status = MG_SHADER_STATUS_NOT_IMPLEMENTED;
	return shader;
}

int MG_shader_define(char** ptr_to_shader_file_text, int define_count, ...)
{
	char* defines = calloc(1, sizeof(char));
	if (!defines)
	{
		printf("Failed to allocate memory for shader defines (-1)\n");
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

	size_t shader_file_size = strlen(*ptr_to_shader_file_text) + 1;
	char* final_shader = calloc(defines_len + shader_file_size, 1);
	if (!final_shader)
	{
		printf("Failed to allocate memory for shader defines (-3)\n");
		free(defines);
		return -3;
	}

	strcpy_s(final_shader, defines_len + shader_file_size, defines);
	strcat_s(final_shader, defines_len + shader_file_size, *ptr_to_shader_file_text);

	free(*ptr_to_shader_file_text);
	*ptr_to_shader_file_text = final_shader;

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
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(shader->ID);
		vertex_shader = 0;
		fragment_shader = 0;
		shader->ID = 0;
	}

	shader->ID = glCreateProgram();
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	int success;

	glShaderSource(vertex_shader, 1, &shader->vertex_shader_file, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
		shader->status = MG_SHADER_STATUS_ERROR;
		return -2;
	}

	glShaderSource(fragment_shader, 1, &shader->fragment_shader_file, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
		glDeleteShader(vertex_shader);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
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
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
		shader->status = MG_SHADER_STATUS_ERROR;
		return -4;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	shader->status = MG_SHADER_STATUS_OK;
	return 0;
}

int MG_shader_register_variable(MG_Shader* shader, const char* name, GLenum type)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Failed to register shader variable: shader is NULL or not compiled\n");
		return -1;
	}

	if (!name || strlen(name) == 0)
	{
		printf("Failed to register shader variable: name is NULL or empty\n");
		return -2;
	}

	MG_ShaderVariable* var = calloc(1, sizeof(MG_ShaderVariable));
	if (!var)
	{
		printf("Failed to allocate memory for shader variable\n");
		return -4;
	}

	switch (type)
	{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			var->size = 1;
			break;

		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
			var->size = 2;
			break;

		case GL_FLOAT:
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_BOOL:
			var->size = 4;
			break;

		case GL_FLOAT_VEC2:
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
			var->size = 8;
			break;

		case GL_FLOAT_VEC3:
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
			var->size = 12;
			break;

		case GL_FLOAT_VEC4:
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
			var->size = 16;
			break;

		case GL_FLOAT_MAT2:
			var->size = 16;
			break;
		case GL_FLOAT_MAT3:
			var->size = 36;
			break;
		case GL_FLOAT_MAT4:
			var->size = 64;
			break;

		case GL_DOUBLE:
			var->size = 8;
			break;
		case GL_DOUBLE_VEC2:
			var->size = 16;
			break;
		case GL_DOUBLE_VEC3:
			var->size = 24;
			break;
		case GL_DOUBLE_VEC4:
			var->size = 32;
			break;

		case GL_DOUBLE_MAT2:
			var->size = 32;
			break;
		case GL_DOUBLE_MAT3:
			var->size = 72;
			break;
		case GL_DOUBLE_MAT4:
			var->size = 128;
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
			var->size = 4;
			break;

		default:
			printf("Failed to register shader variable: unsupported type %u\n", type);
			free(var);
			return -5;
	}

	var->name = name;
	var->type = type;
	MG_LL_Add(&shader->variables, var);
	return 0;
}

int MG_material_register_shader_variable(MG_Material* material, const char* name, GLenum type, uint32_t offset_in_material)
{
	if (!material || !material->shader || !name)
	{
		printf("Failed to register material shader variable: material, name or material shader is NULL\n");
		return -1;
	}

	if (offset_in_material > _msize(material))
	{
		printf("Failed to register material shader variable: offset_in_material is out of bounds\n");
		return -2;
	}

	MG_ShaderVariable_LL* var_ll = material->shader->variables;
	while (var_ll && var_ll->data)
	{
		MG_ShaderVariable* var = var_ll->data;
		if (var->type != type)
		{
			var_ll = var_ll->next;
			continue;
		}

		if (strcmp(var->name, name) != 0)
		{
			var_ll = var_ll->next;
			continue;
		}

		MG_MaterialShaderVariable* mat_var = calloc(1, sizeof(MG_MaterialShaderVariable));
		if (!mat_var)
		{
			printf("Failed to allocate memory for material shader variable translation unit\n");
			return -3;
		}
		mat_var->name = name;
		mat_var->type = type;
		mat_var->offset_in_material = offset_in_material;
		MG_LL_Add(&material->shader_variable_translations, mat_var);

		return 0;
	}

	return 1;
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

int MG_shader_free(MG_Shader* shader)
{
	if (!shader)
	{
		printf("Failed to free shader: shader is NULL\n");
		return -1;
	}
	if (shader->ID)
	{
		glDeleteProgram(shader->ID);
		shader->ID = 0;
	}
	free(shader);
	shader = NULL;
	return 0;
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
	glUniform2iv(glGetUniformLocation(shader->ID, name), 1, &vec);
}

void MG_shader_set_ivec3(MG_Shader* shader, const char* name, int* vec)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader ivec3: shader is NULL or not compiled\n");
		return;
	}
	glUniform3iv(glGetUniformLocation(shader->ID, name), 1, &vec);
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
	glUniform2fv(glGetUniformLocation(shader->ID, name), 1, &vec);
}

void MG_shader_set_vec3(MG_Shader* shader, const char* name, MG_Vec3 vec)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader vec3: shader is NULL or not compiled\n");
		return;
	}
	glUniform3fv(glGetUniformLocation(shader->ID, name), 1, &vec);
}

void MG_shader_set_mat4(MG_Shader* shader, const char* name, MG_Matrix* mat)
{
	if (!shader || shader->status != MG_SHADER_STATUS_OK)
	{
		printf("Warning: Failed to set shader mat4: shader is NULL or not compiled\n");
		return;
	}
	
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, false, mat);
}