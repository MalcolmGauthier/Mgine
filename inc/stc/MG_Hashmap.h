#pragma once
#include "MG_hash.h"
#include "MG_LL.h"

struct MG_HashmapNode
{
	MG_ID key;
	void* value;

	struct MG_HashmapNode* next;
	struct MG_HashmapNode* prev;
};

typedef MG_LinkedList MG_HashmapNode_LL;

typedef struct MG_Hashmap
{
	uint32_t bucket_count;
	MG_HashmapNode_LL** buckets;

	// allows for O(1) iteration through the nodes from oldest to newest (and back)
	struct MG_HashmapNode* first;
	struct MG_HashmapNode* last;

	uint32_t count;
}
MG_Hashmap;