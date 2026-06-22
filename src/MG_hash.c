#include "MG_hash.h"

MG_NAME MG_id_hash_string(const char* name)
{
	if (!name)
	{
		printf("Error: MG_ID_get_hash: name is NULL\n");
		return 0;
	}

	// FNV-1a hash
	//TODO: 64-bit?
	uint32_t hash = 2166136261u;
	const char* ptr = name;
	while (*ptr)
	{
		hash ^= (uint8_t)(toupper(*ptr));
		hash *= 16777619u;
		ptr++;
	}
	return hash;
}


int MG_hashmap_init(MG_Hashmap** hashmap_ref, uint32_t bucket_count)
{
	if (!hashmap_ref || bucket_count == 0)
	{
		printf("Failed to initialize hashmap: invalid bucket count or hashmap_ref is NULL\n");
		return -1;
	}

	*hashmap_ref = calloc(1, sizeof(MG_Hashmap));
	if (!*hashmap_ref)
	{
		printf("Failed to initialize hashmap: out of memory\n");
		return -2;
	}

	(*hashmap_ref)->bucket_count = bucket_count;
	(*hashmap_ref)->buckets = calloc(bucket_count, sizeof(MG_HashmapNode_LL*));
	if (!(*hashmap_ref)->buckets)
	{
		printf("Failed to initialize hashmap: out of memory\n");
		free(*hashmap_ref);
		*hashmap_ref = NULL;
		return -3;
	}

	return 0;
}

static bool MG_hashmap_find_func(void* data, void* find)
{
	// all warning suppression in this file is due to casting types of different width. however, no data is lost.
#pragma warning(suppress: 4311)
	return ((struct MG_HashmapNode*)data)->key == (MG_ID)find;
}

int MG_hashmap_add(MG_Hashmap* hashmap, MG_ID key, void* value)
{
	if (!hashmap)
	{
		printf("Failed to add to hashmap: hashmap is NULL\n");
		return -1;
	}

	uint32_t bucket_index = key % hashmap->bucket_count;
#pragma warning(suppress: 4312)
	if (MG_LL_find_func(hashmap->buckets[bucket_index], (void*)key, MG_hashmap_find_func))
	{
		printf("Refused to add to hashmap: key %u already exists\n", key);
		return 1;
	}
	
	struct MG_HashmapNode* node = calloc(1, sizeof(struct MG_HashmapNode));
	if (!node)
	{
		printf("Failed to add to hashmap: out of memory\n");
		return -2;
	}
	node->key = key;
	node->value = value;

	int err = MG_LL_add(&hashmap->buckets[bucket_index], node);
	if (err)
	{
		free(node);
		return err - 2;
	}

	if (hashmap->last)
		hashmap->last->next = node;
	if (!hashmap->first)
		hashmap->first = node;
	node->prev = hashmap->last;
	hashmap->last = node;
	hashmap->count++;

	return 0;
}

void* MG_hashmap_remove(MG_Hashmap* hashmap, MG_ID key)
{
	if (!hashmap)
	{
		printf("Failed to remove from hashmap: hashmap is NULL\n");
		return NULL;
	}

	uint32_t bucket_index = key % hashmap->bucket_count;
#pragma warning(suppress: 4312)
	struct MG_HashmapNode* node = MG_LL_find_func(hashmap->buckets[bucket_index], (void*)key, MG_hashmap_find_func);
	if (!node)
	{
		printf("Failed to remove from hashmap: key %u not found\n", key);
		return NULL;
	}

	if (hashmap->first == node)
		hashmap->first = node->next;
	else
		node->prev->next = node->next;

	if (hashmap->last == node)
		hashmap->last = node->prev;
	else
		node->next->prev = node->prev;

	hashmap->count--;

	MG_LL_remove(&hashmap->buckets[bucket_index], node);
	void* value = node->value;
	free(node);
	return value;
}

void* MG_hashmap_get(MG_Hashmap* hashmap, MG_ID key)
{
	if (!hashmap)
	{
		printf("Failed to get from hashmap: hashmap is NULL\n");
		return NULL;
	}

#pragma warning(suppress: 4312)
	struct MG_HashmapNode* node = MG_LL_find_func(hashmap->buckets[key % hashmap->bucket_count], (void*)key, MG_hashmap_find_func);
	if (!node)
	{
		printf("Failed to get from hashmap: key %u not found\n", key);
		return NULL;
	}

	return node->value;
}

MG_Hashmap* MG_hashmap_copy(MG_Hashmap* src, void* (*copy_func)(void*))
{
	if (!src)
	{
		printf("Failed to copy hashmap: source hashmap is NULL\n");
		return NULL;
	}

	MG_Hashmap* copy = NULL;
	if (MG_hashmap_init(&copy, src->bucket_count))
	{
		printf("Failed to copy hashmap: failed to initialize copy\n");
		return NULL;
	}

	struct MG_HashmapNode* current = src->first;
	while (current)
	{
		void* value_copy = copy_func ? copy_func(current->value) : current->value;
		if (MG_hashmap_add(copy, current->key, value_copy) < 0)
		{
			printf("Failed to copy hashmap: failed to add key %u to copy.\n", current->key);
			MG_hashmap_free(&copy, NULL);
			return NULL;
		}

		current = current->next;
	}

	return copy;
}

void MG_hashmap_free(MG_Hashmap** hashmap_ref, void (*free_func)(void*))
{
	if (!hashmap_ref)
	{
		return;
	}

	for (uint32_t i = 0; i < (*hashmap_ref)->bucket_count; i++)
	{
		MG_LL_free(&(*hashmap_ref)->buckets[i], free_func);
	}

	free((*hashmap_ref)->buckets);
	free(*hashmap_ref);
	*hashmap_ref = NULL;
}
