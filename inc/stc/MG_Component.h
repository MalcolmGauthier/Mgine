#pragma once

#include "MG_Model.h"

#define MG_COMPONENT_TYPES_RESERVED 0xF0000000
typedef enum
{
	MG_COMPONENT_TYPE_NONE = 0 | MG_COMPONENT_TYPES_RESERVED,
	MG_COMPONENT_TYPE_MODEL = 1 | MG_COMPONENT_TYPES_RESERVED,
	MG_COMPONENT_TYPE_SCRIPT = 2 | MG_COMPONENT_TYPES_RESERVED,
} MG_ComponentTypes;
#undef MG_COMPONENT_TYPES_RESERVED

typedef enum
{
	MG_COMPONENT_FUNC_RESULT_OK = 0,
	MG_COMPONENT_FUNC_RESULT_ERROR = 1,
	MG_COMPONENT_FUNC_RESULT_NOT_IMPLEMENTED = 2,
	MG_COMPONENT_FUNC_RESULT_DELETE_OBJ = 3,
} MG_ComponentFuncResult;

typedef struct MG_Component
{
	MG_ComponentTypes type;
	struct MG_Object* owner;

	void* data;
	MG_ComponentFuncResult (*on_update)(struct MG_Component* self, void* data, float delta_time);
	void (*on_destroy)(struct MG_Component* self, void* data);

	uint32_t flags;
}
MG_Component;