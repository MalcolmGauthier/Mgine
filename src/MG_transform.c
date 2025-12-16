#include "MG_transform.h"

inline SDL_Color MG_color_from_vec4(MG_Vec4 color)
{
    return (SDL_Color) {
        .r = (byte)(color.r * 255.0f),
        .g = (byte)(color.r * 255.0f),
        .b = (byte)(color.r * 255.0f),
        .a = (byte)(color.r * 255.0f)
    };
}

MG_Vec3 MG_transform_position_get(MG_Transform* transform)
{
    if (!transform)
        return (MG_Vec3){ 0.0f, 0.0f, 0.0f };

    return (MG_Vec3){ transform->position.x, transform->position.y, transform->position.z };
}

MG_Vec3 MG_transform_rotation_get(MG_Transform* transform)
{
    if (!transform)
        return (MG_Vec3){ 0.0f, 0.0f, 0.0f };
    
    return (MG_Vec3){ transform->rotation.pitch, transform->rotation.yaw, transform->rotation.roll };
}

MG_Vec3 MG_transform_scale_get(MG_Transform* transform)
{
    if (!transform)
        return (MG_Vec3){ 1.0f, 1.0f, 1.0f };

    return (MG_Vec3){ transform->scale.x, transform->scale.y, transform->scale.z };
}

void MG_transform_position_set(MG_Transform* transform, MG_Vec3 position)
{
    if (!transform)
        return;
	transform->position = position;
}

void MG_transform_rotation_set(MG_Transform* transform, MG_Vec3 rotation)
{
    if (!transform)
        return;
	transform->rotation = rotation;
}

void MG_transform_scale_set(MG_Transform* transform, MG_Vec3 scale)
{
    if (!transform)
        return;
	transform->scale = scale;
}

MG_Vec3 MG_transform_deg_to_rad(MG_Vec3 degrees)
{
    return (MG_Vec3){ glm_rad(degrees.pitch), glm_rad(degrees.yaw), glm_rad(degrees.roll) };
}

MG_Matrix MG_transform_get_matrix(MG_Transform* transform)  
{  
    if (!transform)  
        return (MG_Matrix){ MG_MATRIX_IDENTITY };  

    vec4 quat;
    MG_Matrix result;

	glm_mat4_identity((vec4*)&result);
	glm_translate((vec4*)&result, (float*)&transform->position);
	glm_euler_yzx_quat((float*)&transform->rotation, quat);
	glm_quat_rotate((vec4*)&result, quat, (vec4*)&result);
	glm_scale((vec4*)&result, (float*)&transform->scale);

    return result;  
}

MG_Matrix MG_transform_make_matrix(MG_Vec3 position, MG_Vec3 rotation, MG_Vec3 scale)
{
    MG_Transform temp = { position, rotation, scale };
    return MG_transform_get_matrix(&temp);
}

// update function for the transform component to recalculate the quaternion and matrix
MG_ComponentFuncResult MG_transformcomponent_on_update(struct MG_Component* self, float delta_time)
{
	MG_ComponentTransform* t_self = (MG_ComponentTransform*)self;
	t_self->transform_matrix = MG_transform_get_matrix(&t_self->transform);
	return MG_COMPONENT_FUNC_RESULT_OK;
}