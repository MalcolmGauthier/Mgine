#pragma once

#include "MG_include.h"
#include "stc/MG_Transform.h"

MG_Vec3 MG_Get_Position(MG_Transform* transform);
MG_Vec3 MG_Get_Rotation(MG_Transform* transform);
MG_Vec3 MG_Get_Scale(MG_Transform* transform);

void MG_Set_Position(MG_Transform* transform, MG_Vec3 position);
void MG_Set_Rotation(MG_Transform* transform, MG_Vec3 rotation);
void MG_Set_Scale(MG_Transform* transform, MG_Vec3 scale);

MG_Matrix MG_Get_Transform_Matrix(MG_Transform* transform);