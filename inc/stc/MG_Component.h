#pragma once

#include "MG_Model.h"

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

	void* data;
	size_t data_size;
	MG_ComponentFuncResult (*on_update)(struct MG_Component* self, void* data, float delta_time);
	void (*on_destroy)(struct MG_Component* self, void* data);

	uint32_t flags;
}
MG_Component;