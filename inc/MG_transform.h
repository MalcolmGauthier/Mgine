#pragma once

#include "MG_include.h"
#include "stc/MG_Transform.h"
#include "stc/MG_Component.h"

MG_Vec3 MG_transform_position_get(MG_Transform* transform);
MG_Vec3 MG_transform_rotation_get(MG_Transform* transform);
MG_Vec3 MG_transform_scale_get(MG_Transform* transform);

void MG_transform_position_set(MG_Transform* transform, MG_Vec3 position);
void MG_transform_rotation_set(MG_Transform* transform, MG_Vec3 rotation);
void MG_transform_scale_set(MG_Transform* transform, MG_Vec3 scale);

MG_Matrix MG_transform_get_matrix(MG_Transform* transform);