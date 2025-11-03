#include "MG_render.h"

static void MG_render_update_data(MG_RenderData* render_data);
static void MG_render_update_interp_value(MG_RenderData* render_data);
static MG_Matrix MG_render_calculate_interp_model_matrix(MG_RenderData* render_data, MG_Matrix* new_matrix, uint64_t obj_id);
static MG_Matrix MG_render_calculate_interp_view_matrix(MG_RenderData* render_data);
static void MG_render_OIT_init(MG_RenderData* render_data);
static void MG_render_OIT_prepare(MG_RenderData* render_data);
static void MG_render_object(MG_RenderData* render_data, MG_Object* object);
static void MG_render_model(MG_RenderData* render_data, MG_Model* model, MG_Matrix* pos, int64_t obj_id);
static void MG_render_OIT(MG_RenderData* render_data);
static void MG_render_WBOIT_composite(MG_RenderData* render_data);

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
	MG_TransparentDraw_LL* transparency_ll = calloc(1, sizeof(MG_TransparentDraw_LL));
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

	MG_render_OIT_init(render_data);

	while (instance->active)
	{
		glClear(GL_COLOR_BUFFER_BIT | (MG_R_BACKGROUND_REFRESH ? GL_DEPTH_BUFFER_BIT : 0));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_CULL_FACE);

		// STEP 1: UPDATE GAME DATA
		MG_render_update_data(render_data);
		// STEP 2: UPDATE INTERPOLATION VALUE
		MG_render_update_interp_value(render_data);
		MG_render_calculate_interp_view_matrix(render_data);

		if (!instance->rendering_enabled)
		{
			SDL_Delay(1);
			continue;
		}

		// STEP 3: RENDER ALL OPAQUE OBJECTS
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		MG_Object_LL* current = render_data->latest_data.object_list;
		while (current)
		{
			// render function goes down child list, so only call orphan objects
			// infinite loop possible here, but only if user forgoes calling functions and instead just fucks with struct data
			if (current->data && !((MG_Object*)current->data)->parent)
			{
				MG_render_object(render_data, current->data);
			}

			current = current->next;
		}

		// STEP 4: RENDER TRANSPARENCY
		MG_render_OIT(render_data);

		SDL_GL_SwapWindow(instance->window);
	}

	MG_LL_free_LL_only(transparency_ll);
	return 0;
}

static void MG_render_update_data(MG_RenderData* render_data)
{
	while (render_data->instance->lock_owner == MG_GAME_DATA_LOCK_OWNER_LOGIC_THREAD);
	render_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_RENDER_THREAD;

	// in the case where for some reason ticks take longer than render frames, this will cause ticks to be skipped,
	// and interpolation could cause objects to take crazy paths.
	if (render_data->instance->game_data.global_timer == render_data->latest_data.global_timer)
	{
		render_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_NONE;
		return;
	}

	if (MG_R_INTERPOLATION_ENABLED)
	{
		MG_logic_free(&render_data->old_data);
		memcpy_s(&render_data->old_data, sizeof(MG_GameData), &render_data->latest_data, sizeof(MG_GameData));
	}
	else
	{
		MG_logic_free(&render_data->latest_data);
	}
	memcpy_s(&render_data->latest_data, sizeof(MG_GameData), &render_data->instance->game_data, sizeof(MG_GameData));

	// copy the object list from the game data to the render data
	render_data->latest_data.object_list = MG_LL_copy(render_data->instance->game_data.object_list, MG_object_create_untracked_copy);

	render_data->instance->lock_owner = MG_GAME_DATA_LOCK_OWNER_NONE;
	return;
}

static void MG_render_update_interp_value(MG_RenderData* render_data)
{
	if (!MG_R_INTERPOLATION_ENABLED || render_data->latest_data.delta_time <= 0)
	{
		render_data->interp_value = 1.0f;
		return;
	}

	float time_since_latest = (float)SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency() - (float)render_data->latest_data.uptime;

	if (render_data->latest_data.tickrate != 0)
	{
		float tick_len = 1.0f / render_data->latest_data.tickrate;
		render_data->interp_value = time_since_latest / tick_len;
	}
	else
	{
		render_data->interp_value = time_since_latest / render_data->latest_data.delta_time;
	}

	// if it's been more than some time since the latest data, freeze.
	if (!MG_R_INTERPOLATION_PREDICTION || time_since_latest >= 1.0f)
	{
		if (render_data->interp_value < 0.0f) render_data->interp_value = 0.0f;
		if (render_data->interp_value > 1.0f) render_data->interp_value = 1.0f;
	}
}

static MG_Matrix MG_render_calculate_interp_model_matrix(MG_RenderData* render_data, MG_Matrix* new_matrix, uint64_t obj_id)
{
	if ((render_data->interp_value >= 1.f && !MG_R_INTERPOLATION_PREDICTION) || !MG_R_INTERPOLATION_ENABLED)
		return *new_matrix;

	MG_Transform* old_transform = NULL;
	MG_Vec3 old_ang_rad;
	vec4 old_quat;

	MG_Transform new_transform = { 0 };
	MG_Vec3 new_ang_rad;
	vec4 new_quat;

	MG_Transform interp_transform;
	vec4 interp_quat;
	MG_Matrix output;

	// can't call MG_object_find_by_id here because it wants the instance, but we can only provide game data.
	MG_Object_LL* current = render_data->old_data.object_list;
	while (current)
	{
		if (((MG_Object*)current->data)->id == obj_id)
		{
			MG_ComponentTransform* t = (MG_ComponentTransform*)MG_object_get_component_by_name(current->data, "MG_transform");
			if (!t) return *new_matrix;
			old_transform = &t->transform;
			break;
		}
		current = current->next;
	}

	// this happens when an object is brand new or gained a transform component over a frame
	if (!old_transform)
		return *new_matrix;

	old_ang_rad = MG_transform_deg_to_rad(old_transform->rotation);
	new_ang_rad = MG_transform_deg_to_rad(new_transform.rotation);

	// can't use glm_decompose because it copies a vec4 into translation, which would be a memory overrun in this scenario
	glm_vec3_copy((float*)(&new_matrix->m30), (float*)&new_transform.position);
	glm_decompose_rs((vec4*)new_matrix, &new_quat, (float*)&new_transform.scale);
	glm_euler_yzx_quat((float*)&new_ang_rad, new_quat);
	glm_euler_yzx_quat((float*)&old_ang_rad, old_quat);

	glm_vec3_lerp((float*)&old_transform->position, (float*)&new_transform.position, render_data->interp_value, (float*)&interp_transform.position);
	glm_quat_slerp(old_quat, new_quat, render_data->interp_value, interp_quat);
	glm_vec3_lerp((float*)&old_transform->scale, (float*)&new_transform.scale, render_data->interp_value, (float*)&interp_transform.scale);

	glm_mat4_identity((vec4*)&output);
	glm_translate((vec4*)&output, (float*)&interp_transform.position);
	glm_quat_rotate((vec4*)&output, interp_quat, (vec4*)&output);
	glm_scale((vec4*)&output, (float*)&interp_transform.scale);
	return output;
}

static MG_Matrix MG_render_calculate_interp_view_matrix(MG_RenderData* render_data)
{
	if ((render_data->interp_value >= 1.f && !MG_R_INTERPOLATION_PREDICTION) || !MG_R_INTERPOLATION_ENABLED)
		return MG_camera_get_view_matrix(&render_data->latest_data.camera);

	MG_Camera interp_cam = { 0 };

	// Interpolate position linearly
	glm_vec3_lerp((float*)&render_data->old_data.camera.position, (float*)&render_data->latest_data.camera.position, render_data->interp_value, (float*)&interp_cam.position);

	// Convert old and new rotations to quaternions
	versor old_q, new_q;
	MG_Vec3 old_euler = MG_transform_deg_to_rad(render_data->old_data.camera.rotation);
	MG_Vec3 new_euler = MG_transform_deg_to_rad(render_data->latest_data.camera.rotation);

	glm_euler_xyz_quat((float*)&old_euler, old_q);
	glm_euler_xyz_quat((float*)&new_euler, new_q);

	// SLERP between quaternions
	versor interp_q;
	glm_quat_slerp(old_q, new_q, render_data->interp_value, interp_q);

	// Convert interpolated quaternion back to Euler angles
	vec4 interp_euler;
	mat4 temp;
	glm_quat_mat4(interp_q, temp);
	glm_euler_angles(temp, interp_euler);

	// glm does roll pitch yaw, we use pitch yaw roll
	interp_cam.rotation.pitch = glm_deg(interp_euler[1]);
	interp_cam.rotation.yaw = glm_deg(interp_euler[2]);
	interp_cam.rotation.roll = glm_deg(interp_euler[0]);

	if (render_data->old_data.camera.focus && render_data->latest_data.camera.focus)
	{
		static MG_Vec3 interp_focus;
		glm_vec3_lerp((float*)render_data->old_data.camera.focus, (float*)render_data->latest_data.camera.focus, render_data->interp_value, (float*)&interp_focus);

		interp_cam.focus = &interp_focus;
	}

	// Compute view matrix
	render_data->view_matrix = MG_camera_get_view_matrix(&interp_cam);
	return render_data->view_matrix;
}

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

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Error: OIT framebuffer incomplete!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void MG_render_OIT_prepare(MG_RenderData* render_data)
{
	glBindFramebuffer(GL_FRAMEBUFFER, render_data->OIT_FBO);
	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	const GLfloat zeroF[4] = { 0, 0, 0, 0 };
	const GLfloat oneF[4] = { 1, 1, 1, 1 };
	glClearBufferfv(GL_COLOR, 0, zeroF);
	glClearBufferfv(GL_COLOR, 1, oneF);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	// Accumulation blending
	glBlendFunci(0, GL_ONE, GL_ONE);

	// Revealage blending
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
}

static void MG_render_object(MG_RenderData* render_data, MG_Object* object)
{
	if (object->flags & MG_OBJECT_FLAG_INVISIBLE)
		return;

	MG_Object_LL* children = object->children;
	while (children && children->data)
	{
		MG_render_object(render_data, children->data);
		children = children->next;
	}

	if (!object->components) return;

	MG_ComponentModel* current_model = (MG_ComponentModel*)MG_object_get_component_by_name(object, "MG_Model");
	if (!current_model || !current_model->model.enabled)
		return;

	MG_Matrix current_matrix = MG_object_get_world_transform_matrix(object);

	MG_render_model(render_data, &current_model->model, &current_matrix, object->id);
}

static void MG_render_model(MG_RenderData* render_data, MG_Model* model, MG_Matrix* pos, int64_t obj_id)
{
	if (!model || !pos)
		return;

	MG_Matrix render_matrix = MG_render_calculate_interp_model_matrix(render_data, pos, obj_id);

	MG_Mesh* mesh;
	for (uint32_t i = 0; i < model->mesh_count; i++)
	{
		mesh = &model->meshes[i];

		if (!mesh->material)
		{
			continue;
		}

		// if the mesh contains transparency, add it to the transparency list for later
		if (mesh->material->contains_transparency)
		{
			MG_TransparentDraw* new_node = calloc(1, sizeof(MG_TransparentDraw));
			if (!new_node)
				return;

			new_node->mesh = mesh;
			new_node->render_matrix = render_matrix;
			MG_LL_add(render_data->transparency_list, new_node);
			continue;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->material->diffuse_texture);

		//TODO: if shader null, set shader to default minimal shader
		if (!mesh->material->shader)
		{
			mesh->material->shader = 0; // MG_shader_get_default();
		}

		MG_shader_use(mesh->material->shader);
		MG_shader_set_mat4(mesh->material->shader, "uModel", &render_matrix);
		MG_shader_set_mat4(mesh->material->shader, "uView", &render_data->view_matrix);
		MG_shader_set_mat4(mesh->material->shader, "uProj", &render_data->latest_data.camera.projection_matrix);

		MG_MaterialShaderVariable_LL* var_ll = mesh->material->shader_variables;
		int num_textures = 0;
		int max_textures;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_textures);
		while (var_ll && var_ll->data)
		{
			MG_MaterialShaderVariable* var = var_ll->data;

			void* value_ptr = (void*)((char*)mesh->material + var->offset_in_material);
			switch (var->type)
			{
			case GL_FLOAT:
				MG_shader_set_float(mesh->material->shader, var->name, *(float*)value_ptr);
				break;
			case GL_FLOAT_VEC2:
				MG_shader_set_vec2(mesh->material->shader, var->name, *(MG_Vec2*)value_ptr);
				break;
			case GL_FLOAT_VEC3:
				MG_shader_set_vec3(mesh->material->shader, var->name, *(MG_Vec3*)value_ptr);
				break;
			case GL_INT:
			case GL_BOOL:
				MG_shader_set_int(mesh->material->shader, var->name, *(int*)value_ptr);
				break;
			case GL_INT_VEC2:
			case GL_BOOL_VEC2:
				MG_shader_set_ivec2(mesh->material->shader, var->name, (int*)value_ptr);
				break;
			case GL_INT_VEC3:
			case GL_BOOL_VEC3:
				MG_shader_set_ivec3(mesh->material->shader, var->name, (int*)value_ptr);
				break;

			case GL_FLOAT_MAT4:
				MG_shader_set_mat4(mesh->material->shader, var->name, (MG_Matrix*)value_ptr);
				break;

			case GL_SAMPLER_2D:
				glActiveTexture(GL_TEXTURE0 + num_textures);
				glBindTexture(GL_TEXTURE_2D, *(GLuint*)((char*)mesh->material + var->offset_in_material));
				MG_shader_set_int(mesh->material->shader, var->name, *(GLuint*)value_ptr);
				num_textures++;
				if (num_textures >= max_textures)
				{
					num_textures--;
					printf("Warning: Maximum texture units exceeded when rendering model. Some textures may not appear correctly.\n");
				}
				break;

			default:
				printf("Warning: Unsupported shader variable type %u for variable %s\n", var->type, var->name);
				break;
			}

			var_ll = var_ll->next;
		}

		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
	}
}

static void MG_render_OIT(MG_RenderData* render_data)
{
	MG_render_OIT_prepare(render_data);

	MG_TransparentDraw* t_draw;
	MG_Material* material;
	while (render_data->transparency_list->next)
	{
		render_data->transparency_list = render_data->transparency_list->next;
		t_draw = (MG_TransparentDraw*)render_data->transparency_list->data;
		if (!t_draw) continue;

		if (!t_draw->mesh->material)
		{
			//TODO: add default material
			continue;
		}
		material = t_draw->mesh->material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->diffuse_texture);

		//TODO: if shader null, set shader to default minimal shader
		if (!material->shader)
		{
			material->shader = 0; // MG_shader_get_default();
		}

		MG_shader_use(material->shader);
		MG_shader_set_mat4(material->shader, "uModel", &t_draw->render_matrix);
		MG_shader_set_mat4(material->shader, "uView", &render_data->view_matrix);
		MG_shader_set_mat4(material->shader, "uProj", &render_data->latest_data.camera.projection_matrix);

		MG_MaterialShaderVariable_LL* var_ll = material->shader_variables;
		while (var_ll && var_ll->data)
		{
			MG_MaterialShaderVariable* var = var_ll->data;

			void* value_ptr = (void*)((char*)material + var->offset_in_material);
			switch (var->type)
			{
			case GL_FLOAT:
				MG_shader_set_float(material->shader, var->name, *(float*)value_ptr);
				break;
			case GL_FLOAT_VEC2:
				MG_shader_set_vec2(material->shader, var->name, *(MG_Vec2*)value_ptr);
				break;
			case GL_FLOAT_VEC3:
				MG_shader_set_vec3(material->shader, var->name, *(MG_Vec3*)value_ptr);
				break;
			case GL_INT:
			case GL_BOOL:
				MG_shader_set_int(material->shader, var->name, *(int*)value_ptr);
				break;
			case GL_INT_VEC2:
			case GL_BOOL_VEC2:
				MG_shader_set_ivec2(material->shader, var->name, (int*)value_ptr);
				break;
			case GL_INT_VEC3:
			case GL_BOOL_VEC3:
				MG_shader_set_ivec3(material->shader, var->name, (int*)value_ptr);
				break;

			case GL_FLOAT_MAT4:
				MG_shader_set_mat4(material->shader, var->name, (MG_Matrix*)value_ptr);
				break;

			default:
				printf("Warning: Unsupported shader variable type %u for variable %s\n", var->type, var->name);
				break;
			}

			var_ll = var_ll->next;
		}

		glBindVertexArray(t_draw->mesh->VAO);
		glDrawElements(GL_TRIANGLES, t_draw->mesh->index_count, GL_UNSIGNED_INT, 0);
	}

	MG_render_WBOIT_composite(render_data);

	MG_LL_free(render_data->transparency_list, NULL);
}

static void MG_render_WBOIT_composite(MG_RenderData* render_data)
{
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	MG_shader_use(render_data->OIT_shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, render_data->OIT_shader->ID);
	MG_shader_set_int(render_data->OIT_shader, "uAccumTex", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, render_data->OIT_shader->ID);
	MG_shader_set_int(render_data->OIT_shader, "uRevealTex", 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
