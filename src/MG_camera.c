#include "MG_camera.h"

MG_Camera MG_camera_init(float FOV, float screen_width, float screen_height, float near_clip, float far_clip)
{
	MG_Camera output = { 0 };
	MG_camera_set_perspective(&output, FOV, screen_width, screen_height, near_clip, far_clip);
	return output;
}

MG_Vec3 MG_camera_get_world_position(MG_GameData* game_data)
{
    MG_Camera* camera = &game_data->camera;

    if (!camera->anchor)
		return camera->position;

    MG_Object* obj = MG_object_get_by_id(game_data->instance, camera->anchor);
    if (!obj)
		return camera->position;

    MG_Vec3 world_pos = MG_object_get_world_position(obj);

    return (MG_Vec3)
    {
        world_pos.x + camera->position.x,
        world_pos.y + camera->position.y,
        world_pos.z + camera->position.z
    };
}

MG_Vec3 MG_camera_get_world_rotation(MG_GameData* game_data)
{
    MG_Camera* camera = &game_data->camera;
    // set roll+default
    MG_Vec3 result = camera->rotation;

    if (!camera->focus)
		return result;

    MG_Object* obj = MG_object_get_by_id(game_data->instance, camera->focus);
    if (!obj)
		return result;

    MG_Vec3 cam_pos = MG_camera_get_world_position(game_data);
    MG_Vec3 world_pos = MG_object_get_world_position(obj);
    vec3 diff = {
        world_pos.x - cam_pos.x,
        world_pos.y - cam_pos.y,
        world_pos.z - cam_pos.z
    };
    glm_vec3_normalize(diff);
    result.pitch = asinf(diff[1]) / (GLM_PIf * 2.0f);
    result.yaw = atan2f(diff[2], diff[0]) / (GLM_PIf * 2.0f);
    return result;
}

MG_Matrix MG_camera_get_view_matrix(MG_GameData* game_data)
{
    MG_Vec3 pos = MG_camera_get_world_position(game_data);
    MG_Vec3 rot = MG_camera_get_world_rotation(game_data);
    MG_Vec3 rot_rad = MG_transform_to_rad(rot);

    vec3 front = {
        cosf(rot_rad.pitch) * cosf(rot_rad.yaw),
        sinf(rot_rad.pitch),
        cosf(rot_rad.pitch) * sinf(rot_rad.yaw)
    };
    glm_vec3_normalize(front);

    vec3 up = { 0.0f, 1.0f, 0.0f };
    if (rot.roll != 0.0f)
    {
        mat4 roll_mat;
        glm_rotate_make(roll_mat, rot_rad.roll, front);
        vec3 up_rot;
        glm_mat4_mulv3(roll_mat, up, 0.0f, up_rot);
        glm_vec3_copy(up_rot, up);
    }

    vec3 world_pos = { pos.x, pos.y, pos.z };
    vec3 center;
    glm_vec3_add(world_pos, front, center);

    MG_Matrix view;
    glm_lookat(world_pos, center, up, (vec4*)&view);
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