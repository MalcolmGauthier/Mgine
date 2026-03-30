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

// In order to make serialization easy and possible in many contexts and also to avoid letting the user face the terrifying world of public struct members and raw 
// pointers in a C++ world, all asset interactions are done with an ID, strongly encouraging the requirement of having no pointers in component fields.
// the ID is used along with the instance pointer to find the object before interacting with it. The coder thus only sees a number on his end. The giant downside of this
// is that any object function will require an O(n) search. However, this is likely to change in the future.
typedef MG_ID MG_OBJ;
typedef struct MG_OBJ_ARRAY
{
	MG_OBJ* data;
	uint32_t count;
}
MG_OBJ_ARRAY;

typedef struct MG_Object
{
	// an object's parent will be their personal origin.
	// components can make use of an object's parent for various things. for example,
	// an object's transform component's will be relative to the parent's transform, if it has one. (this keeps going recursively is the parent also has a parent)
	// objects with no parent are either called an orphan object or a top-level object.
	struct MG_Object* parent;
	MG_Object_LL* children;
	MG_Component_LL* components;

	MG_OBJ id;
	MG_NAME name;
	uint32_t flags;
}
MG_Object;

typedef MG_Object MG_Prefab;