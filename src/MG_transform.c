#include "MG_transform.h"

MG_Vec3 MG_Get_Position(MG_Transform* transform)
{
    if (!transform)
        return (MG_Vec3){ 0.0f, 0.0f, 0.0f };

    return (MG_Vec3){ transform->X, transform->Y, transform->Z };
}

MG_Vec3 MG_Get_Rotation(MG_Transform* transform)
{
    if (!transform)
        return (MG_Vec3){ 0.0f, 0.0f, 0.0f };

    return (MG_Vec3){ transform->pitch, transform->yaw, transform->roll };
}

MG_Vec3 MG_Get_Scale(MG_Transform* transform)
{
    if (!transform)
        return (MG_Vec3){ 1.0f, 1.0f, 1.0f };

    return (MG_Vec3){ transform->scale_x, transform->scale_y, transform->scale_z };
}

void MG_Set_Position(MG_Transform* transform, MG_Vec3 position)
{
    if (!transform)
        return;
    transform->X = position.x;
    transform->Y = position.y;
    transform->Z = position.z;
}

void MG_Set_Rotation(MG_Transform* transform, MG_Vec3 rotation)
{
    if (!transform)
        return;
    transform->pitch = rotation.x;
    transform->yaw = rotation.y;
    transform->roll = rotation.z;
}

void MG_Set_Scale(MG_Transform* transform, MG_Vec3 scale)
{
    if (!transform)
        return;
    transform->scale_x = scale.x;
    transform->scale_y = scale.y;
    transform->scale_z = scale.z;
}

MG_Matrix MG_Get_Transform_Matrix(MG_Transform* transform)  
{  
    if (!transform)  
        return (MG_Matrix){ .matrix = GLM_MAT4_IDENTITY_INIT };  

    mat4 translation, rotation, scale, TR, output;

    glm_translate_make(translation, (vec3){ transform->X, transform->Y, transform->Z });
    glm_euler_xyz((vec3){ transform->pitch, transform->yaw, transform->roll }, rotation);
    glm_scale_make(scale, (vec3){ transform->scale_x, transform->scale_y, transform->scale_z });

    // Combine T * R * S  
    glm_mul(translation, rotation, TR);
    glm_mul(TR, scale, output);

    MG_Matrix result = {0};
    memcpy_s(result.matrix, sizeof(mat4), output, sizeof(mat4));
    return result;  
}