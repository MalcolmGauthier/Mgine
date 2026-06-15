#include "MG_hash.h"

MG_NAME MG_id_hash_string(const char* name)
{
	if (!name)
	{
		printf("Error: MG_ID_get_hash: name is NULL\n");
		return 0;
	}

	// FNV-1a hash
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


int MG_hashmap_init(MG_Hashmap** hashset_ref, uint32_t bucket_count)
{
	if (!hashset_ref || bucket_count == 0)
	{
		printf("Failed to initialize hashset: invalid bucket count or hashset_ref is NULL\n");
		return -1;
	}

	*hashset_ref = malloc(sizeof(MG_Hashmap));
	if (!*hashset_ref)
	{
		printf("Failed to initialize hashset: out of memory\n");
		return -2;
	}

	(*hashset_ref)->bucket_count = bucket_count;
	(*hashset_ref)->buckets = calloc(bucket_count, sizeof(MG_HashmapNode_LL*));
	if (!(*hashset_ref)->buckets)
	{
		printf("Failed to initialize hashset: out of memory\n");
		free(*hashset_ref);
		*hashset_ref = NULL;
		return -3;
	}

	return 0;
}

static bool MG_hashmap_find_func(void* data, void* find)
{
	return ((struct MG_HashmapNode*)data)->key == (MG_ID)find;
}

int MG_hashmap_add(MG_Hashmap* hashset, MG_ID key, void* value)
{
	if (!hashset)
	{
		printf("Failed to add to hashset: hashset is NULL\n");
		return -1;
	}

	uint32_t bucket_index = key % hashset->bucket_count;
	if (MG_LL_find_func(hashset->buckets[bucket_index], (void*)key, MG_hashmap_find_func))
	{
		printf("Refused to add to hashset: key %u already exists\n", key);
		return 1;
	}
	
	struct MG_HashmapNode* node = malloc(sizeof(struct MG_HashmapNode));
	if (!node)
	{
		printf("Failed to add to hashset: out of memory\n");
		return -2;
	}
	node->key = key;
	node->value = value;

	int err = MG_LL_add(&hashset->buckets[bucket_index], node);
	if (err)
	{
		free(node);
		return err - 2;
	}

	return 0;
}

void* MG_hashmap_remove(MG_Hashmap* hashset, MG_ID key)
{
	if (!hashset)
	{
		printf("Failed to remove from hashset: hashset is NULL\n");
		return NULL;
	}

	uint32_t bucket_index = key % hashset->bucket_count;
	struct MG_HashmapNode* node = MG_LL_find_func(hashset->buckets[bucket_index], (void*)key, MG_hashmap_find_func);
	if (!node)
	{
		printf("Failed to remove from hashset: key %u not found\n", key);
		return NULL;
	}

	MG_LL_remove(&hashset->buckets[bucket_index], node);
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

	struct MG_HashmapNode* node = MG_LL_find_func(hashmap->buckets[key % hashmap->bucket_count], (void*)key, MG_hashmap_find_func);
	if (!node)
	{
		printf("Failed to get from hashmap: key %u not found\n", key);
		return NULL;
	}

	return node->value;
}

void MG_hashmap_free(MG_Hashmap** hashset_ref, void (*free_func)(void*))
{
	if (!hashset_ref)
	{
		return;
	}

	for (uint32_t i = 0; i < (*hashset_ref)->bucket_count; i++)
	{
		MG_LL_free(&(*hashset_ref)->buckets[i], free_func);
	}

	free((*hashset_ref)->buckets);
	free(*hashset_ref);
	*hashset_ref = NULL;
}
