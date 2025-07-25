#include "MG_render.h"

// kept here to keep private
static void MG_render_object(MG_RenderData* render_data, MG_Object* object);
static void MG_render_components(MG_RenderData* render_data, MG_Component* component);
static void MG_render_model(MG_RenderData* render_data, MG_Model* model);

// The main render loop of the game engine. This renders the game objects to the screen.
// It iterates down the component tree of each object and renders the found models.
int MG_render_loop(void* MG_instance)
{
	if (!MG_instance)
	{
		printf("Render loop crash: instance is NULL\n");
		return -1;
	}

	MG_Instance* instance = (MG_Instance*)MG_instance;
	MG_RenderData* render_data = &instance->render_data;
	MG_Mesh_LL* transparency_ll = calloc(1, sizeof(MG_Mesh_LL));
	if (!transparency_ll)
	{
		printf("Render loop crash: Failed to allocate memory for transparency list\n");
		instance->active = false;
		return -2;
	}
	render_data->transparency_list = transparency_ll;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	while (instance->active)
	{
		// [TODO] remove color clear when background added
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_CULL_FACE);

		MG_render_update_data(render_data);
		MG_render_update_interp_value(render_data);

		MG_Object_LL* current = render_data->latest_data.object_list;
		while (current)
		{
			if (current->data)
			{
				//MG_render_object(current->data, &transparency_ll);
			}

			current = current->next;
		}

		glDisable(GL_CULL_FACE);
		MG_render_OIT(render_data);

		SDL_GL_SwapWindow(instance->window);
	}

	MG_render_free_transparency_ll(transparency_ll);
	return 0;
}

// Initializes the OIT (Order Independent Transparency) rendering system.
static void MG_render_OIT_init(MG_RenderData* render_data)
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

static void MG_render_object(MG_RenderData* render_data, MG_Object* object)
{
	if (object->flags & MG_OBJECT_FLAG_INVISIBLE)
		return;



	//MG_render_component(render_data, object->components->data);
}

static void MG_render_component(MG_RenderData* render_data, MG_Component* component)
{
	if (!component) return;

	if (component->model)
	{
		MG_render_model(render_data, component->model);
	}

	MG_Component_LL* children = component->children;
	while (children)
	{
		MG_render_components(render_data, children->component);
		children = children->next;
	}
}

static void MG_render_model(MG_RenderData* render_data, MG_Model* model)
{
	if (!model) return;

	MG_Mesh* mesh;
	for (uint32_t i = 0; i < model->mesh_count; i++)
	{
		mesh = &model->meshes[i];

		// if the mesh contains transparency, add it to the transparency list for later
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

static void MG_render_OIT(MG_RenderData* render_data)
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

static void MG_render_update_data(MG_RenderData* render_data)
{
	while (render_data->instance->lock_owner == MG_GAME_DATA_LOCK_OWNER_LOGIC_THREAD);
	render_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_RENDER_THREAD;

	if (render_data->instance->game_data.global_timer == render_data->latest_data.global_timer)
	{
		render_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_NONE;
		return;
	}

	MG_LL_Free(&render_data->old_data);
	memcpy_s(&render_data->old_data, sizeof(MG_GameData), &render_data->latest_data, sizeof(MG_GameData));
	memcpy_s(&render_data->latest_data, sizeof(MG_GameData), &render_data->instance->game_data, sizeof(MG_GameData));

	// copy the object list from the game data to the render data
	render_data->latest_data.object_list = NULL;
	MG_Object_LL* current = render_data->instance->game_data.object_list;
	while (current)
	{
		if (current->data)
		{
			MG_Object_LL* new_node = calloc(1, sizeof(MG_Object_LL));
			if (!new_node) return;
			new_node->data = current->data;
			new_node->next = render_data->latest_data.object_list;
			render_data->latest_data.object_list = new_node;
		}
		current = current->next;
	}

	render_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_NONE;
	return;
}
