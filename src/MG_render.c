#include "MG_render.h"

void MG_render_object(MG_Object* object, MG_RenderData* render_data);
void MG_RenderComponents(MG_Component* component, MG_RenderData* render_data);
void MG_RenderModel(MG_Model* model, MG_RenderData* render_data);

int MG_render_loop(void* MG_instance)
{
	if (!MG_instance)
	{
		printf("Render loop crash: instance is NULL\n");
		return -1;
	}

	MG_Instance* instance = (MG_Instance*)MG_instance;
	MG_RenderData* render_data = &instance->render_data;
	struct MG_Mesh_LL* transparency_ll = calloc(1, sizeof(struct MG_Mesh_LL));
	if (!transparency_ll)
	{
		printf("Render loop crash: Failed to allocate memory for transparency list\n");
		return -2;
	}
	render_data->transparency_list = transparency_ll;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	while (instance->active)
	{
		//TODO: remove color clear when background added
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_CULL_FACE);

		struct MG_Object_LL* current = render_data->latest_data.object_list;
		while (current)
		{
			if (current->object)
			{
				MG_render_object(current->object, &transparency_ll);
			}

			current = current->next;
		}

		glDisable(GL_CULL_FACE);
		MG_render_OIT(render_data);

		SDL_GL_SwapWindow(instance->window);
	}

	free(transparency_ll);
	return 0;
}

void MG_OIT_init(MG_RenderData* render_data)
{
	glGenTextures(1, &render_data->accum_tex);
	glBindTexture(GL_TEXTURE_2D, render_data->accum_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, MG_W_WIDTH, MG_W_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &render_data->reveal_tex);
	glBindTexture(GL_TEXTURE_2D, render_data->reveal_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, MG_W_WIDTH, MG_W_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenFramebuffers(1, &render_data->OIT_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, render_data->OIT_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_data->accum_tex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, render_data->reveal_tex, 0);
	GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
}

void MG_render_object(MG_Object* object, MG_RenderData* render_data)
{
	if (object->flags & MG_OBJECT_FLAG_INVISIBLE)
		return;



	MG_RenderComponents(&object->self, render_data);
}

void MG_RenderComponents(MG_Component* component, MG_RenderData* render_data)
{
	if (!component) return;

	if (component->model)
	{
		MG_RenderModel(component->model, render_data);
	}

	struct MG_Component_LL* children = component->children;
	while (children)
	{
		MG_RenderComponents(children->component, render_data);
		children = children->next;
	}
}

void MG_RenderModel(MG_Model* model, MG_RenderData* render_data)
{
	if (!model) return;

	MG_Mesh* mesh;
	for (uint32_t i = 0; i < model->mesh_count; i++)
	{
		mesh = &model->meshes[i];

		if (mesh->contains_transparency)
		{
			struct MG_Mesh_LL* new_node = calloc(1, sizeof(struct MG_Mesh_LL));
			if (!new_node) return;
			while (render_data->transparency_list->next)
			{
				render_data->transparency_list = render_data->transparency_list->next;
			}
			new_node->mesh = mesh;
			render_data->transparency_list->next = new_node;
			return;
		}

		if (mesh->material)
		{
			glBindTexture(GL_TEXTURE_2D, mesh->material->diffuse_texture);
			glBindTexture(GL_TEXTURE_2D, mesh->material->specular_texture);
		}

		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
	}
}

void MG_render_OIT(MG_RenderData* render_data)
{
	glBindFramebuffer(GL_FRAMEBUFFER, render_data->OIT_FBO);
	glClearBufferfv(GL_COLOR, 0, (GLfloat[]) { 0, 0, 0, 0 });
	glClearBufferfv(GL_COLOR, 1, (GLfloat[]) { 1.0f });

	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	MG_Mesh* mesh;
	while (render_data->transparency_list->next)
	{
		render_data->transparency_list = render_data->transparency_list->next;
		mesh = render_data->transparency_list->mesh;

		if (mesh->shader)
		{
			//UseShader(mesh->shader);
		}

		if (mesh->material)
		{
			glBindTexture(GL_TEXTURE_2D, mesh->material->diffuse_texture);
			glBindTexture(GL_TEXTURE_2D, mesh->material->specular_texture);
		}

		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//glUseProgram(compositeShader);
	//glBindVertexArray(fullscreenQuadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, render_data->accum_tex);
	//glUniform1i(glGetUniformLocation(compositeShader, "uAccum"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, render_data->reveal_tex);
	//glUniform1i(glGetUniformLocation(compositeShader, "uReveal"), 1);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}