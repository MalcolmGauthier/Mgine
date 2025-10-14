#include "MG_load_model.h"

MG_Model load_model(const char* path)
{
    const struct aiScene* scene = aiImportFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        0//aiProcess_ImproveCacheLocality //re-enable when normal maps get support
    );

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        fprintf(stderr, "Assimp error: %s\n", aiGetErrorString());
        // todo replace with error model
        return (MG_Model){0};
    }

    MG_Model model = { 0 };
    model.mesh_count = scene->mNumMeshes;
    model.meshes = calloc(model.mesh_count, sizeof(MG_Mesh));
    if (!model.meshes) goto done;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        const struct aiMesh* ai_mesh = scene->mMeshes[m];

        MG_Mesh* mesh = &model.meshes[m];
        mesh->vertex_count = ai_mesh->mNumVertices;
        mesh->vertices = calloc(mesh->vertex_count, sizeof(MG_Vertex));
		if (!mesh->vertices) goto fail;

        // this prevents the minimum/maximum of being stuck at 0
        mesh->bounding_box.min = (MG_Vec3){ FLT_MAX, FLT_MAX, FLT_MAX };
        mesh->bounding_box.max = (MG_Vec3){ -FLT_MAX, -FLT_MAX, -FLT_MAX };

        for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
        {
            MG_Vertex* v = &mesh->vertices[i];

            memcpy(v->position, &ai_mesh->mVertices[i], sizeof(float) * 3);
            memcpy(v->normal, &ai_mesh->mNormals[i], sizeof(float) * 3);

            if (ai_mesh->mTextureCoords[0])
            {
                v->UV[0] = ai_mesh->mTextureCoords[0][i].x;
                v->UV[1] = ai_mesh->mTextureCoords[0][i].y;
            }

#if MG_R_VERTEX_COLOR_ENABLED
            if (ai_mesh->mColors[0])
            {
                v->color[0] = ai_mesh->mColors[0][i].r;
                v->color[1] = ai_mesh->mColors[0][i].g;
                v->color[2] = ai_mesh->mColors[0][i].b;
                v->color[3] = ai_mesh->mColors[0][i].a;
            }
            else
            {
                v->color[0] = 1.0f;
                v->color[1] = 1.0f;
                v->color[2] = 1.0f;
                v->color[3] = 1.0f;
            }
#endif

			if (v->position[0] < mesh->bounding_box.min.x) mesh->bounding_box.min.x = v->position[0];
			if (v->position[0] > mesh->bounding_box.max.x) mesh->bounding_box.max.x = v->position[0];
			if (v->position[1] < mesh->bounding_box.min.y) mesh->bounding_box.min.y = v->position[1];
			if (v->position[1] > mesh->bounding_box.max.y) mesh->bounding_box.max.y = v->position[1];
			if (v->position[2] < mesh->bounding_box.min.z) mesh->bounding_box.min.z = v->position[2];
			if (v->position[2] > mesh->bounding_box.max.z) mesh->bounding_box.max.z = v->position[2];
        }

        mesh->index_count = ai_mesh->mNumFaces * 3;
        mesh->indices = malloc(sizeof(uint32_t) * mesh->index_count);
        if (!mesh->indices) goto fail;

        const struct aiFace* face;
        for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
        {
            face = &ai_mesh->mFaces[i];
            mesh->indices[i * 3 + 0] = face->mIndices[0];
            mesh->indices[i * 3 + 1] = face->mIndices[1];
            mesh->indices[i * 3 + 2] = face->mIndices[2];
        }
    }

    goto done;

fail:
    for (uint32_t i = 0; i < model.mesh_count; i++)
    {
		// vertices being null means we've reached the point where allocation failed
		if (!model.meshes[i].vertices)
            break;

		// compiler thinks junk data could be in here, but meshes are allocated with calloc
#pragma warning(suppress: 6001)
        free(model.meshes[i].vertices);
#pragma warning(suppress: 6001)
		free(model.meshes[i].indices);
	}
    free(model.meshes);
	model = (MG_Model){ 0 };

done:
    aiReleaseImport(scene);
    return model;
}