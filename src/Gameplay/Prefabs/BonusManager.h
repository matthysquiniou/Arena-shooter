#pragma once
#include "EntityWrapper.h"

class BonusManager
{
public:
	static EntityWrapper* CreateNem(gce::Vector3f32 pos);
	static EntityWrapper* CreateRiceBowl(gce::Vector3f32 pos);
	static EntityWrapper* CreateNoodles(gce::Vector3f32 pos);
};

