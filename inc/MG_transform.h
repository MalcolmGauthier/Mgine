#pragma once

#include "MG_include.h"
#include "stc/MG_Transform.h"
#include "stc/MG_Component.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API MG_Vec2 MG_vec2_add(MG_Vec2 a, MG_Vec2 b);
MG_API MG_Vec2 MG_vec2_sub(MG_Vec2 a, MG_Vec2 b);
MG_API MG_Vec2 MG_vec2_scale(MG_Vec2 a, float scalar);

MG_API MG_Vec3 MG_vec3_add(MG_Vec3 a, MG_Vec3 b);
MG_API MG_Vec3 MG_vec3_sub(MG_Vec3 a, MG_Vec3 b);
MG_API MG_Vec3 MG_vec3_scale(MG_Vec3 a, float scalar);

MG_API MG_Vec3 MG_vec4_add(MG_Vec4 a, MG_Vec4 b);
MG_API MG_Vec3 MG_vec4_sub(MG_Vec4 a, MG_Vec4 b);
MG_API MG_Vec3 MG_vec4_scale(MG_Vec4 a, float scalar);

MG_API SDL_Color MG_color_from_vec4(MG_Vec4 color);

MG_API MG_Vec3 MG_transform_position_get(MG_Transform* transform);
MG_API MG_Vec3 MG_transform_rotation_get(MG_Transform* transform);
MG_API MG_Vec3 MG_transform_scale_get(MG_Transform* transform);

MG_API void MG_transform_position_set(MG_Transform* transform, MG_Vec3 position);
MG_API void MG_transform_rotation_set(MG_Transform* transform, MG_Vec3 rotation);
MG_API void MG_transform_scale_set(MG_Transform* transform, MG_Vec3 scale);

MG_API MG_Vec3 MG_transform_deg_to_rad(MG_Vec3 degrees);
MG_API MG_Vec3 MG_transform_rad_to_deg(MG_Vec3 radians);

MG_API MG_Matrix MG_transform_get_matrix(MG_Transform* transform);
MG_API MG_Matrix MG_transform_make_matrix(MG_Vec3 position, MG_Vec3 rotation, MG_Vec3 scale);

MG_API MG_ComponentFuncResult MG_transformcomponent_on_update(struct MG_Component* self, float delta_time);

#ifdef __cplusplus
}
#endif