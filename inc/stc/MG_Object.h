#pragma once

#include "MG_include.h"
#include "MG_Instance.h"
#include "MG_Model.h"
#include "MG_Component.h"
#include "MG_LL.h"
#include "MG_Transform.h"

typedef enum MG_ObjectFlags
{
	MG_OBJECT_FLAG_DYNAMIC = 1 << 0,
	MG_OBJECT_FLAG_SOLID = 1 << 1,
	MG_OBJECT_FLAG_INVISIBLE = 1 << 2,
	//MG_OBJECT_FLAG_TRANSPARENCY = 1 << 3,
	MG_OBJECT_FLAG_MARKED_FOR_DELETION = 1 << 4,
}
MG_ObjectFlags;

typedef MG_Generic_LL MG_Object_LL;
typedef MG_Generic_LL MG_Component_LL;

typedef struct MG_Object
{
	struct MG_Object* parent;
	MG_Object_LL* children;
	MG_Component_LL* components;

	MG_Instance* instance;

	uint64_t id;
	uint32_t flags;

	MG_Transform transform;

	void (*on_load)(struct MG_Object* self);
	void (*on_tick)(struct MG_Object* self);
}
MG_Object;
