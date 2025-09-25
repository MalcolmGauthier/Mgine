#pragma once

#include "MG_Model.h"
#include "MG_Transform.h"

typedef enum
{
	MG_COMPONENT_TYPE_MODEL = 1 << 1,
	MG_COMPONENT_TYPE_TRANSFORM = 1 << 2,
} MG_ComponentFlags;

typedef enum
{
	MG_COMPONENT_FUNC_RESULT_OK = 0,
	MG_COMPONENT_FUNC_RESULT_ERROR = 1,
	MG_COMPONENT_FUNC_RESULT_NOT_IMPLEMENTED = 2,
	MG_COMPONENT_FUNC_RESULT_DELETE_OBJ = 3,
} MG_ComponentFuncResult;

typedef struct MG_Component
{
	uint32_t id;
	struct MG_Object* owner;

	MG_ComponentFuncResult (*on_create)(struct MG_Component* self);
	MG_ComponentFuncResult (*on_update)(struct MG_Component* self, float delta_time);
	void (*on_destroy)(struct MG_Component* self);

	uint32_t flags;
}
MG_Component;

typedef struct MG_ComponentTemplate
{
	uint32_t id;
	size_t size;

	MG_ComponentFuncResult (*on_create)(struct MG_Component* self);
	MG_ComponentFuncResult (*on_update)(struct MG_Component* self, float delta_time);
	void (*on_destroy)(struct MG_Component* self);
}
MG_ComponentTemplate;


#define MG_COMPONENT_TRANSFORM_ID -1
typedef struct MG_ComponentTransform
{
	MG_Component base;
	
	MG_Transform transform;
	MG_Matrix transform_matrix;
}
MG_ComponentTransform;

#define	MG_COMPONENT_MODEL_ID -2
typedef struct MG_ComponentModel
{
	MG_Component base;

	MG_Model model;
}
MG_ComponentModel;