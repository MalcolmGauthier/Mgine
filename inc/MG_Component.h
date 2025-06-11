#pragma once

#include "MG_Model.h"

typedef struct MG_Component MG_Component;
typedef struct MG_Model MG_Model;

struct MG_Component_LL
{
	MG_Component* component;
	struct MG_Component_LL* next;
};

typedef struct MG_Component
{
	MG_Component* parent;
	struct MG_Component_LL* children;

	float position[3];
	float rotation[3];
	float scale[3];

	MG_Model* model;
}
MG_Component;