#include "MG_id.h"

uint32_t MG_ID_get_id(const char* name)
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