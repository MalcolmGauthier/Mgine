#include "MG_camera.h"

MG_Camera MG_camera_init(float FOV, float screen_width, float screen_height, float near_clip, float far_clip)
{
	MG_Camera output = { 0 };
	MG_camera_set_perspective(&output, FOV, screen_width, screen_height, near_clip, far_clip);
	return output;
}

MG_Matrix MG_camera_get_view_matrix(MG_Camera* camera)
{
	MG_Matrix view;
	vec3 front;

	// calculate front vector
	if (camera->focus)
	{
		glm_vec3_sub((float*)camera->focus, (float*)&camera->position, front);
		glm_vec3_normalize(front);
	}
	else
	{
		float pitch = glm_rad(camera->rotation.pitch);
		float yaw = glm_rad(camera->rotation.yaw);

		front[0] = cosf(pitch) * cosf(yaw);
		front[1] = sinf(pitch);
		front[2] = cosf(pitch) * sinf(yaw);

		glm_vec3_normalize(front);
	}

	vec3 world_up = { 0.0f, 1.0f, 0.0f };

	// calculate up vector (only required when roll added)
	// apparently glm_lookat doesn't care if up isn't perpendicular to front/right???
	if (camera->rotation.roll != 0.0f)
	{
		mat4 rollMat;
		glm_rotate_make(rollMat, glm_rad(camera->rotation.roll), front);
		vec3 upRot;
		glm_mat4_mulv3(rollMat, world_up, 0.0f, upRot);
		glm_vec3_copy(upRot, world_up);
	}

	vec3 center;
	glm_vec3_add((float*)&camera->position, front, center);
	glm_lookat((float*)&camera->position, center, world_up, (vec4*)&view);
	return view;
}

void MG_camera_set_perspective(MG_Camera* camera, float FOV, float screen_width, float screen_height, float near_clip, float far_clip)
{
	camera->fov = FOV;
	if (screen_height == 0) screen_height = 1;
	camera->screen_width = screen_width;
	camera->screen_height = screen_height;
	camera->near_clip = near_clip;
	camera->far_clip = far_clip;

	if (near_clip <= 0) near_clip = 0.01f;
	if (far_clip > FLT_MAX) far_clip = FLT_MAX;

	if (near_clip >= far_clip)
	{
		float temp = near_clip;
		near_clip = far_clip;
		far_clip = temp;
	}

	glm_perspective(glm_rad(FOV), screen_width / screen_height, near_clip, far_clip, (vec4*)&camera->projection_matrix);
}

void MG_camera_set_orthographic(MG_Camera* camera, float left, float right, float bottom, float top, float near_clip, float far_clip)
{
	camera->fov = 0.0f;
	camera->screen_width = right - left;
	camera->screen_height = top - bottom;
	camera->near_clip = near_clip;
	camera->far_clip = far_clip;

	if (near_clip <= 0) near_clip = 0.01f;
	if (far_clip > FLT_MAX) far_clip = FLT_MAX;

	if (near_clip >= far_clip)
	{
		float temp = near_clip;
		near_clip = far_clip;
		far_clip = temp;
	}

	glm_ortho(left, right, bottom, top, near_clip, far_clip, (vec4*)&camera->projection_matrix);
}