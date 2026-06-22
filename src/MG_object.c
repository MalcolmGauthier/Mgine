#include "MG_object.h"

void MG_object_free(MG_Object* object);
MG_Object* MG_object_create_tracked_copy(MG_Object* source);

extern MG_Component* MG_component_copy_untracked(MG_Component* source);

MG_OBJ MG_object_create(MG_OBJ parent, const char* name, MG_ObjectFlags flags)
{
	MG_Object* object = calloc(1, sizeof(MG_Object));
	if (!object)
	{
		printf("Failed to allocate memory for object\n");
		free(object);
		return 0;
	}

	object->parent = parent ? MG_object_ptr(parent) : NULL;
	object->flags = flags;
	object->name = MG_id_hash_string(name);
	object->id = MG_INSTANCE->game_data.object_list->next_id;

	if (MG_hashmap_add(MG_INSTANCE->game_data.object_list->assets, object->id, object))
	{
		printf("Failed to add object to instance object list\n");
		free(object);
		return 0;
	}

	if (parent && MG_object_add_child(parent, object->id))
	{
		printf("Failed to add object to parent\n");
		free(object);
		MG_object_free(object);
		return 0;
	}

	MG_asset_add(MG_INSTANCE->game_data.object_list, object);
	return object->id;
}

MG_OBJ MG_object_create_by_copy(MG_OBJ src)
{
	if (!src)
	{
		printf("Failed to copy object: object is NULL\n");
		return 0;
	}
	
	MG_Object* src_obj = MG_object_ptr(src);
	MG_OBJ obj_id = MG_object_create(src_obj->parent ? src_obj->parent->id : 0, "", src_obj->flags);
	MG_Object* obj = MG_object_ptr(obj_id);

	obj->name = src_obj->name;
	obj->children = MG_LL_copy(src_obj->children, MG_object_create_tracked_copy);
	// can't use real copy function because that takes 2 params and LL_Copy only takes 1
	// thus, we need to manually set the component's owner to the new object
	obj->components = MG_LL_copy(src_obj->components, MG_component_copy_untracked);
	MG_Component_LL* c = obj->components;
	while (c && c->data)
	{
		((MG_Component*)c->data)->id.owner = obj->id;
		c = c->next;
	}

	return obj_id;
}

MG_Object* MG_object_create_untracked_copy(MG_Object* src)
{
	MG_Object* source = src;
	if (!source)
	{
		printf("Failed to create untracked copy: source is NULL\n");
		return NULL;
	}

	MG_Object* object = calloc(1, sizeof(MG_Object));
	if (!object)
	{
		printf("Failed to allocate memory for untracked object copy\n");
		return NULL;
	}
	memcpy_s(object, sizeof(MG_Object), source, sizeof(MG_Object));
	object->children = NULL;
	object->components = NULL;
	object->id = 0;

	if (source->children)
		object->children = MG_LL_copy(source->children, MG_object_create_untracked_copy);
	if (source->components)
		object->components = MG_LL_copy(source->components, MG_component_copy_untracked);

	return object;
}

MG_Object* MG_object_create_tracked_copy(MG_Object* source)
{
	return MG_object_ptr(MG_object_create_by_copy(source->id));
}


MG_Object* MG_object_ptr(MG_OBJ id)
{
	return MG_hashmap_get(MG_INSTANCE->game_data.object_list->assets, id);
}

MG_OBJ MG_object_get_by_name(const char* name)
{
	MG_NAME name_id = MG_id_hash_string(name);

	struct MG_HashmapNode* current = MG_INSTANCE->game_data.object_list->assets->first;
	while (current)
	{
		if (((MG_Object*)current->value)->name == name_id)
		{
			return ((MG_Object*)current->value)->id;
		}
		current = current->next;
	}

	printf("Failed to get object with name %s: not found\n", name);
	return 0;
}


//MG_OBJ_ARRAY MG_object_get_all_top_level()
//{
//	MG_Object_LL* top_level_objects = NULL;
//	MG_Object_LL* current = MG_INSTANCE->game_data.object_list;
//	while (current)
//	{
//		if (!((MG_Object*)current->data)->parent)
//		{
//			MG_LL_add(&top_level_objects, current->data);
//		}
//		current = current->next;
//	}
//
//	return top_level_objects;
//}


int MG_object_add_child(MG_OBJ parent_id, MG_OBJ child_id)
{
	MG_Object* parent = MG_object_ptr(parent_id);
	MG_Object* child = MG_object_ptr(child_id);
	if (!parent || !child)
	{
		printf("Failed to add child: parent or child does not exist\n");
		return -1;
	}

	// check to see if adding child would cause infinite loop
	MG_Object* current = parent;
	while (current->parent)
	{
		if (current->parent == child)
		{
			// say no to incest!
			printf("Failed to add child: child is parent of new parent\n");
			return -2;
		}

		current = current->parent;
	}


	if (child->parent)
	{
		MG_object_remove_child(child->parent->id, child->id);
	}

	child->parent = parent;
	MG_LL_add(&parent->children, child);
	return 0;
}

int MG_object_remove_child(MG_OBJ parent_id, MG_OBJ child_id)
{
	MG_Object* child = MG_object_ptr(child_id);
	MG_Object* parent = MG_object_ptr(parent_id);
	if (!parent || !child)
	{
		printf("Failed to remove child: parent/child does not exist\n");
		return -1;
	}

	if (MG_LL_remove(&parent->children, child))
		return 0;

	printf("Failed to remove child with ID %u: not found\n", (uint32_t)child_id);
	return -2;
}


MG_ComponentInstanceID MG_object_add_component(MG_OBJ object, const char* component_name)
{
	MG_ComponentTemplate* comp_template_ptr = MG_component_get_template_ptr(MG_component_get_template(component_name));
	MG_Object* object_ptr = MG_object_ptr(object);

	if (!comp_template_ptr || !object_ptr)
	{
		printf("Failed to add component: component template with name %s or object with ID %u not found\n", component_name, object);
		return (MG_ComponentInstanceID){ 0 };
	}

	MG_Component* component = calloc(1, sizeof(comp_template_ptr->size));
	if (!component)
	{
		printf("ERROR: out of memory, unable to allocate new component");
		return (MG_ComponentInstanceID){ 0 };
	}

	component->base = comp_template_ptr;

	// copy everything except first 2 pointers.
	//UPDATE: not safe enough, just force user to initialize everything in on_create. objects loaded from mg file or prefab will have data copied by the engine shortly after this function.
	//size_t offset = offsetof(MG_Component, flags);
	//size_t size = sizeof(comp_template_ptr->size) - offset;
	//if (size > 0 && data)
	//    memcpy_s((char*)component + offset, size, (char*)data + offset, size);

	MG_LL_add(&object_ptr->components, component);

	if (component->base->on_create && component->base->on_create(component) == MG_COMPONENT_FUNC_RESULT_DELETE_OBJ)
	{
		MG_object_delete(object);
	}

	return component->id;
}

MG_Component* MG_object_get_component_by_name(MG_OBJ object, const char* name)
{
	MG_Object* obj = MG_object_ptr(object);
	if (!obj)
	{
		printf("Failed to get component: object is NULL\n");
		return NULL;
	}

	MG_ID id = MG_id_hash_string(name);

	MG_Component_LL* current = obj->components;
	while (current)
	{
		if (current->data && ((MG_Component*)current->data)->base->id == id)
		{
			return current->data;
		}
		current = current->next;
	}

	return NULL;
}

MG_Component* MG_object_get_component(MG_ComponentInstanceID component_id)
{
	MG_Object* obj = MG_object_ptr(component_id.owner);
	if (!obj)
	{
		printf("Failed to get component: object is NULL\n");
		return NULL;
	}

	MG_Component_LL* current = obj->components;
	while (current)
	{
		if (current->data && ((MG_Component*)current->data)->base->id == component_id.self_id)
		{
			return current->data;
		}
		current = current->next;
	}

	return NULL;
}


MG_Vec3 MG_object_get_world_position(MG_OBJ object)
{
	MG_Object* obj = MG_object_ptr(object);
	if (!obj)
	{
		printf("Failed to get world position: object not foun\n");
		return (MG_Vec3){ 0 };
	}

	MG_Vec3 world_pos = { 0 };

	MG_Object* current = obj;
	while (current)
	{
		MG_ComponentTransform* transform = (MG_ComponentTransform*)MG_object_get_component_by_name(current->id, "transform");
		if (transform)
		{
			world_pos = MG_vec3_add(world_pos, transform->transform.position);
		}

		current = current->parent;
	}

	return world_pos;
}

MG_Vec3 MG_object_get_world_rotation(MG_OBJ object)
{
	MG_Object* obj = MG_object_ptr(object);
	if (!obj)
	{
		printf("Failed to get world rotation: object not found\n");
		return (MG_Vec3) { 0 };
	}

	MG_Vec3 world_rot = { 0 };

	MG_Object* current = obj;
	while (current)
	{
		MG_ComponentTransform* transform = (MG_ComponentTransform*)MG_object_get_component_by_name(current->id, "transform");
		if (transform)
		{
			world_rot = MG_vec3_add(world_rot, transform->transform.rotation);
		}

		current = current->parent;
	}

	return world_rot;
}

MG_Matrix MG_object_calculate_world_transform_matrix(MG_Object* obj)
{
	if (!obj)
	{
		printf("Failed to get world transform matrix: object is NULL\n");
		return (MG_Matrix) { MG_MATRIX_IDENTITY };
	}

	MG_Vec3 world_pos = { 0 };
	MG_Vec3 world_rot = { 0 };
	MG_Vec3 world_scale = { 1, 1, 1 };

	MG_Object* current = obj;
	while (current)
	{
		MG_ComponentTransform* transform = (MG_ComponentTransform*)MG_object_get_component_by_name(current->id, "transform");
		if (transform)
		{
			world_pos = MG_vec3_add(world_pos, transform->transform.position);
			world_rot = MG_vec3_add(world_rot, transform->transform.rotation);
			world_scale.x *= transform->transform.scale.x;
			world_scale.y *= transform->transform.scale.y;
			world_scale.z *= transform->transform.scale.z;
		}

		current = current->parent;
	}

	return MG_transform_make_matrix(world_pos, world_rot, world_scale);
}


void MG_object_free_components(MG_Object* object)
{
	if (!object)
	{
		printf("Failed to free components: object is NULL\n");
		return;
	}

	MG_LL_free(&object->components, MG_component_free);
	object->components = NULL;
}


void MG_object_delete(MG_OBJ object)
{
	MG_Object* obj = MG_object_ptr(object);

	if (obj)
	{
		obj->flags |= MG_OBJECT_FLAG_MARKED_FOR_DELETION;
	}
}

// frees memory. not to be used during game runtime.
void MG_object_free(MG_Object* object)
{
	if (!object)
	{
		printf("Failed to delete null object\n");
		return;
	}

	// delete children recursively
	MG_LL_free(&object->children, MG_object_free);
	// call on_destroy for all components
	MG_LL_free(&object->components, MG_component_free);
	MG_hashmap_remove(MG_INSTANCE->game_data.object_list->assets, object->id);

	free(object);
	return;
}

int MG_object_delete_non_recursive(MG_OBJ id)
{
	MG_Object* object = MG_object_ptr(id);
	if (!object)
	{
		printf("Failed to delete object with ID %u: not found\n", id);
		return -2;
	}

	// set the parent of all the children to the parent of the object being deleted
	// create temp object to avoid memory leak of LL nodes
	MG_Object_LL* current = object->children;
	while (current && current->data)
	{
		MG_Object* child = (MG_Object*)current->data;
		child->parent = object->parent;
		current = current->next;
	}

	// delete childless object
	MG_object_delete(id);
	return 0;
}


void MG_object_free_prefab(MG_Object* prefab)
{
	MG_LL_free(&prefab->children, MG_object_free_prefab);
	MG_LL_free(&prefab->components, MG_component_free);

	free(prefab);
}