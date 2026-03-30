#include "MG_id.h"

MG_NAME MG_ID_hash_string(const char* name)
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