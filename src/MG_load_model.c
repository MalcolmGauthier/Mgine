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

    MG_Model model;
    model.mesh_count = scene->mNumMeshes;
    model.meshes = calloc(model.mesh_count, sizeof(MG_Mesh));

    for (unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        const struct aiMesh* ai_mesh = scene->mMeshes[m];

        MG_Mesh* mesh = &model.meshes[m];
        mesh->vertex_count = ai_mesh->mNumVertices;
        mesh->vertices = calloc(mesh->vertex_count, sizeof(MG_Vertex));

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

            /*if (ai_mesh->mColors[0])
            {
                v->color[0] = ai_mesh->mColors[0][i].r;
                v->color[1] = ai_mesh->mColors[0][i].g;
                v->color[2] = ai_mesh->mColors[0][i].b;
            }
            else
            {
                v->color[0] = 1.0f;
                v->color[1] = 1.0f;
                v->color[2] = 1.0f;
            }*/
        }

        mesh->index_count = ai_mesh->mNumFaces * 3;
        mesh->indices = malloc(sizeof(unsigned int) * mesh->index_count);
        if (!mesh->indices) return (MG_Model) { 0 };

        const struct aiFace* face;
        for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
        {
            face = &ai_mesh->mFaces[i];
            mesh->indices[i * 3 + 0] = face->mIndices[0];
            mesh->indices[i * 3 + 1] = face->mIndices[1];
            mesh->indices[i * 3 + 2] = face->mIndices[2];
        }
    }

    aiReleaseImport(scene);
    return model;
}