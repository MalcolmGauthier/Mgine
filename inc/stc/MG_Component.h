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

typedef struct MG_Component
{
	struct MG_ComponentTemplate* base;
	struct MG_Object* owner;

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


typedef void (*MG_DeferredRegFunc)(MG_GameData*);
extern MG_ComponentTemplate* MG_component_register(MG_Instance* instance, size_t struct_size, const char* name,
	MG_ComponentFuncResult(*on_create)(struct MG_Component* self),
	MG_ComponentFuncResult(*on_update)(struct MG_Component* self, float delta_time),
	void (*on_destroy)(struct MG_Component* self));

#pragma section(".MGREG$A", read)
#pragma section(".MGREG$Z", read)
extern __declspec(allocate(".MGREG$A")) MG_DeferredRegFunc __MGreg_start;
extern __declspec(allocate(".MGREG$Z")) MG_DeferredRegFunc __MGreg_end;

#define MG_COMPONENT(struct_type, name_str, create_fn, update_fn, destroy_fn)            \
    static void __MGreg_##struct_type(MG_Instance* gdm);                                 \
    __pragma(section(".MGREG$M", read))                                                  \
    __declspec(allocate(".MGREG$M")) MG_DeferredRegFunc _MGreg_##struct_type = __MGreg_##struct_type; \
    static void __MGreg_##struct_type(MG_Instance* gdm) {                                \
        MG_component_register(gdm, sizeof(struct_type), name_str,                        \
                              create_fn, update_fn, destroy_fn);                         \
    }

typedef struct MG_ComponentTransform
{
	MG_Component base;
	
	MG_Transform transform;
	MG_Matrix transform_matrix;
}
MG_ComponentTransform;

typedef struct MG_ComponentModel
{
	MG_Component base;

	MG_Model model;
}
MG_ComponentModel;
