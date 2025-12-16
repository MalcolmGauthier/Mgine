#include "MG_model.h"

int MG_model_load(MG_Model* model)
{
    const struct aiScene* scene = aiImportFileFromMemory(
        model->base.asset_file_data,
        (uint32_t)model->base.asset_file_size,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        0,//aiProcess_ImproveCacheLocality //re-enable when normal maps get support
        NULL
    );

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        fprintf(stderr, "Assimp error: %s\n", aiGetErrorString());
        // todo replace with error model
        return -1;
    }

    model->mesh_count = scene->mNumMeshes;
    model->meshes = calloc(model->mesh_count, sizeof(MG_Mesh));
    // goto done instead of fail because mesh count of 0 is valid
    if (!model->meshes) goto done;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        const struct aiMesh* ai_mesh = scene->mMeshes[m];

        MG_Mesh* mesh = &model->meshes[m];
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

		glGenVertexArrays(1, &mesh->VAO);
		glGenBuffers(1, &mesh->VBO);
		glGenBuffers(1, &mesh->EBO);
    }

done:
    model->base.loaded = true;
    free(model->base.asset_file_data);
	model->base.asset_file_data = NULL;
	model->base.asset_file_loaded = false;
    aiReleaseImport(scene);
    return 0;

fail:
    for (uint32_t i = 0; i < model->mesh_count; i++)
    {
		// vertices being null means we've reached the point where allocation failed
		if (!model->meshes[i].vertices)
            break;

		// compiler thinks junk data could be in here, but meshes are allocated with calloc
#pragma warning(suppress: 6001)
        free(model->meshes[i].vertices);
#pragma warning(suppress: 6001)
		free(model->meshes[i].indices);
	}
    free(model->meshes);

    aiReleaseImport(scene);
    return -2;
}

void MG_model_enable(MG_Model* model, bool static_model)
{
    if (!model || !model->meshes)
    {
		printf("MG_model_enable: model is NULL, empty or uninitialized\n");
        return;
    }

    for (uint32_t i = 0; i < model->mesh_count; i++)
    {
        MG_Mesh* mesh = &model->meshes[i];
        if (!mesh->vertices || !mesh->indices)
            continue;

        glBindVertexArray(mesh->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(MG_Vertex) * mesh->vertex_count, mesh->vertices, static_model ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mesh->index_count, mesh->indices, static_model ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MG_Vertex), (void*)offsetof(MG_Vertex, position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MG_Vertex), (void*)offsetof(MG_Vertex, normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MG_Vertex), (void*)offsetof(MG_Vertex, UV));
        glEnableVertexAttribArray(2);
#if MG_R_VERTEX_COLOR_ENABLED
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(MG_Vertex), (void*)offsetof(MG_Vertex, color));
		glEnableVertexAttribArray(3);
#endif
		glBindVertexArray(0);
    }

	//model->enabled = true;
    model->base.loaded = true;
}