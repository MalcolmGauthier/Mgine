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

	shader->status = MG_SHADER_STATUS_OK;
	return 0;
}
