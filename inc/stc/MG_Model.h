#pragma once

#include "MG_include.h"
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

	bool contains_transparency;

	MG_Shader* shader;
}
MG_Material;

typedef struct MG_Mesh
{
	struct MG_Model* parent;

	MG_Vertex* vertices;
	uint32_t vertex_count;

	uint32_t* indices;
	uint32_t index_count;

	GLuint VAO, VBO, EBO;
	MG_Material* material;

	struct MG_AABB
	{
		MG_Vec3 min;
		MG_Vec3 max;
	} bounding_box;

	bool double_sided;
}
MG_Mesh;

// for use in the render loop, where unsorted meshes still need info on where they are
typedef struct MG_TransparentDraw
{
	MG_Mesh* mesh;
	MG_Matrix render_matrix;
}
MG_TransparentDraw;

typedef MG_Generic_LL MG_Mesh_LL;
typedef MG_Generic_LL MG_TransparentDraw_LL;

typedef struct MG_Model
{
	MG_Mesh* meshes;
	uint32_t mesh_count;
}
MG_Model;