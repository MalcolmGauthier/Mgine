#pragma once

#include "MG_include.h"
#include "stc/MG_Object.h"
#include "stc/MG_Shader.h"
#include "stc/MG_Material.h"

typedef struct MG_Vertex
{
	float position[3];
	float normal[3];
	float UV[2];
#if MG_R_VERTEX_COLOR_ENABLED
	float color[4];
#endif
}
MG_Vertex;

typedef struct MG_Mesh
{
	struct MG_Model* parent;

	MG_Vertex* vertices;
	uint32_t vertex_count;

	uint32_t* indices;
	uint32_t index_count;

	GLuint VAO, VBO, EBO;
	struct MG_Material* material;

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
	struct MG_Mesh* mesh;
	MG_Matrix render_matrix;
}
MG_TransparentDraw;

typedef MG_Generic_LL MG_Mesh_LL;
typedef MG_Generic_LL MG_TransparentDraw_LL;

typedef struct MG_Model
{
	struct MG_Mesh* meshes;
	uint32_t mesh_count;

	bool enabled;

	const char* path;
	uint32_t index_in_file;
}
MG_Model;