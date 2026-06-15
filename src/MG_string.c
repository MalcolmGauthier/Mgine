#include "MG_string.h"

MG_String* MG_stringA(const char* text)
{
	if (!text)
		return NULL;

	MG_String* str = malloc(sizeof(MG_String));
	if (!str)
		return NULL;

	str->length = (uint32_t)strlen(text);
	str->data = malloc(((size_t)str->length + 1) * sizeof(wchar_t));
	if (!str->data)
	{
		free(str);
		return NULL;
	}

	mbstowcs_s(NULL, str->data, str->length + 1, text, str->length);
	return str;
}

MG_String* MG_string(const wchar_t* text)
{
	if (!text)
		return NULL;

	MG_String* str = malloc(sizeof(MG_String));
	if (!str)
		return NULL;

	str->length = (uint32_t)wcslen(text);
	str->data = malloc(((size_t)str->length + 1) * sizeof(wchar_t));
	if (!str->data)
	{
		free(str);
		return NULL;
	}

	wcscpy_s(str->data, str->length, text);
	return str;
}