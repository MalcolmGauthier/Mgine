#include "MG_shaders.h"

MG_Shader* MG_create_shader(const char* vertex_shader, const char* fragment_shader)
{
	MG_Shader* shader = calloc(1, sizeof(MG_Shader));
	if (!shader)
	{
		printf("Failed to allocate memory for shader\n");
		return NULL;
	}
	
	shader->vertex_shader_file = vertex_shader;
	shader->fragment_shader_file = fragment_shader;
	return shader;
}

int MG_compile_shader(MG_Shader* shader)
{
	if (!shader)
	{
		printf("Failed to compile shader: shader is NULL\n");
		return -1;
	}

	if (shader->ID)
	{
		printf("WARNING: shader already compiled. recompiling.\n");
		glDeleteShader(shader->vertex_shader);
		glDeleteShader(shader->fragment_shader);
		glDeleteProgram(shader->ID);
		shader->vertex_shader = 0;
		shader->fragment_shader = 0;
		shader->ID = 0;
	}

	shader->ID = glCreateProgram();
	shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	int success;

	glShaderSource(shader->vertex_shader, 1, &shader->vertex_shader_file, NULL);
	glCompileShader(shader->vertex_shader);
	glGetShaderiv(shader->vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader->vertex_shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
		free(shader);
		return -2;
	}

	glShaderSource(shader->fragment_shader, 1, &shader->fragment_shader_file, NULL);
	glCompileShader(shader->fragment_shader);
	glGetShaderiv(shader->fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader->fragment_shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
		free(shader);
		return -3;
	}

	glAttachShader(shader->ID, shader->vertex_shader);
	glAttachShader(shader->ID, shader->fragment_shader);
	glLinkProgram(shader->ID);
	glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shader->ID, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
		free(shader);
		return -4;
	}

	return 0;
}
