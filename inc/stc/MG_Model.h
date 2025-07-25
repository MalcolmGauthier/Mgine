#pragma once

#include "MG_include.h"
#include "MG_Model.h"
#include "MG_Component.h"
#include "MG_Object.h"
#include "MG_Shader.h"

typedef struct MG_Vertex
{
	float position[3];
	float normal[3];
	float UV[2];
}
MG_Vertex;

typedef struct MG_Material
{
	float diffuse_rgb[3];
	float specular_rgb[3];
	float shininess;

	char* diffuse_texture_path;
	GLuint diffuse_texture;
	char* specular_texture_path;
	GLuint specular_texture;
}
MG_Material;

struct MG_AABB
{
	float min[3];
	float max[3];
};

//struct MG_Triangle
//{
//	MG_Vertex vertices[3];
//	float normal[3];
//	float UV[2];
//
//	float dist_to_camera;
//	MG_Material* material;
//	MG_Shader* shader;
//};

typedef struct MG_Mesh
{
	MG_Vertex* vertices;
	uint32_t vertex_count;

	uint32_t* indices;
	uint32_t index_count;

	GLuint VAO, VBO, EBO;
	MG_Material* material;
	MG_Shader* shader;

	struct MG_AABB bounding_box;
	bool contains_transparency;
}
MG_Mesh;

typedef MG_Generic_LL MG_Mesh_LL;

typedef struct MG_Model
{
	MG_Mesh* meshes;
	uint32_t mesh_count;
}
MG_Model;