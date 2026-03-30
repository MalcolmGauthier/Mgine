#pragma once

#include "stc/MG_Transform.h"
#include "stc/MG_Model.h"
#include "MG_id.h"

typedef enum
{
	MG_COMPONENT_FUNC_RESULT_OK = 0,
	MG_COMPONENT_FUNC_RESULT_ERROR = 1,
	MG_COMPONENT_FUNC_RESULT_NOT_IMPLEMENTED = 2,
	MG_COMPONENT_FUNC_RESULT_DELETE_OBJ = 3,
}
MG_ComponentFuncResult;

typedef struct MG_ComponentInstanceID
{
	MG_OBJ owner;
	MG_ID self_id;
}
MG_ComponentInstanceID;

// COMPONENTS MUST NEVER CONTAIN POINTERS.
// if they do, during serialization on a game save, the loaded pointer will be invalid.
// SAFE FIELD TYPES:
// - primitive numbers (int/short/long, float, bool, char)
// - structs that also only contain allowed types (MG_Vec3, MG_Transform, MG_Matrix)
// - Mgine asset IDs (MG_ID, MG_OBJ, MG_ComponentInstanceID)
// - Mgine string type
// - Mgine registered functions
typedef struct MG_Component
{
	struct MG_ComponentTemplate* base;
	MG_ComponentInstanceID id;

	uint32_t flags;
}
MG_Component;

typedef struct MG_ComponentTemplate
{
	MG_ID id;
	size_t size;

	MG_ComponentFuncResult (*on_create)(struct MG_Component* self);
	MG_ComponentFuncResult (*on_tick)(struct MG_Component* self, float delta_time);
	void (*on_destroy)(struct MG_Component* self);
}
MG_ComponentTemplate;

typedef MG_Generic_LL MG_ComponentTemplate_LL;


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*MG_DeferredRegFunc)();
MG_API extern MG_ComponentTemplate* MG_component_register(size_t struct_size, const char* name,
	MG_ComponentFuncResult(*on_create)(struct MG_Component* self),
	MG_ComponentFuncResult(*on_update)(struct MG_Component* self, float delta_time),
	void (*on_destroy)(struct MG_Component* self));

#ifdef __cplusplus
}
#endif

#pragma section(".MGREG$A", read)
#pragma section(".MGREG$Z", read)
extern __declspec(allocate(".MGREG$A")) MG_DeferredRegFunc __MGreg_start;
extern __declspec(allocate(".MGREG$Z")) MG_DeferredRegFunc __MGreg_end;

// don't worry about it
#define MG_COMPONENT(struct_type, name_str, create_fn, update_fn, destroy_fn)                         \
	typedef struct MG_ComponentInstanceID struct_type##ID;                                                \
    static void __MGreg_##struct_type();                                                              \
    __pragma(section(".MGREG$M", read))                                                               \
    __declspec(allocate(".MGREG$M")) MG_DeferredRegFunc _MGreg_##struct_type = __MGreg_##struct_type; \
    static void __MGreg_##struct_type() {                                                             \
        MG_component_register(sizeof(struct_type), name_str, create_fn, update_fn, destroy_fn);       \
    }

typedef struct MG_ComponentTransform
{
	MG_Component base;

	MG_Transform transform;
	MG_Matrix transform_matrix;
} MG_ComponentTransform;

typedef struct MG_ComponentModel
{
	MG_Component base;

	MG_Model* model;
	bool UI_element;
} MG_ComponentModel;
