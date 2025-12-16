#pragma once

#include "MG_include.h"
#include "stc/MG_Transform.h"
#include "stc/MG_Component.h"

inline SDL_Color MG_color_from_vec4(MG_Vec4 color);

MG_Vec3 MG_transform_position_get(MG_Transform* transform);
MG_Vec3 MG_transform_rotation_get(MG_Transform* transform);
MG_Vec3 MG_transform_scale_get(MG_Transform* transform);

void MG_transform_position_set(MG_Transform* transform, MG_Vec3 position);
void MG_transform_rotation_set(MG_Transform* transform, MG_Vec3 rotation);
void MG_transform_scale_set(MG_Transform* transform, MG_Vec3 scale);

MG_Vec3 MG_transform_deg_to_rad(MG_Vec3 degrees);

MG_Matrix MG_transform_get_matrix(MG_Transform* transform);
MG_Matrix MG_transform_make_matrix(MG_Vec3 position, MG_Vec3 rotation, MG_Vec3 scale);

MG_ComponentFuncResult MG_transformcomponent_on_update(struct MG_Component* self, float delta_time);