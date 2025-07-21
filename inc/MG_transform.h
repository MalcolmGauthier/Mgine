#pragma once

#include "MG_include.h"
#include "stc/MG_Transform.h"

MG_Vec3 MG_Get_Position(MG_Transform* transform)  
{  
    return (MG_Vec3){ transform->X, transform->Y, transform->Z };
}