#pragma once
#include <iostream>
#include "EntityWrapper.h"
#include "../SceneManager.h"
#include <vector>

class UIManager
{
public:
	void Init();

	bool IsSceneType(SceneType type);
};
