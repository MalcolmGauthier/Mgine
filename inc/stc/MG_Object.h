#pragma once

#include "MG_include.h"
#include "MG_Instance.h"
#include "MG_Model.h"
#include "MG_LL.h"
#include "MG_Transform.h"

typedef enum MG_ObjectFlags
{
	// signals to the engine if the object is able to move
	MG_OBJECT_FLAG_DYNAMIC = 1 << 0,
	// signals to the engine if the object performs collision checks
	MG_OBJECT_FLAG_SOLID = 1 << 1,
	// signals to the renderer if the object should skip rendering
	MG_OBJECT_FLAG_INVISIBLE = 1 << 2,
	// used internally for deletion. ensures the object doesn't get immediately deleted, preventing crashes from dangling pointers.
	MG_OBJECT_FLAG_MARKED_FOR_DELETION = 1 << 3,
	// signals to the renderer to not perform interpolation between frames. if for exmaple an object teleports or juts intentionally, it would look weird.
	MG_OBJECT_FLAG_NO_INTERP = 1 << 4,
	// signals to the renderer to always point the object directly at the camera
	MG_OBJECT_FLAG_BILLBOARD = 1 << 5,
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

	MG_ID id;
	MG_ID name;
	uint32_t flags;
}
MG_Object;

typedef MG_Object MG_Prefab;