#pragma once
#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "GameManager.h"
#include "../SceneManager.h"
#include "../Prefabs/InventoryManager.h"
#include "../Prefabs/ScoreManager.h"
#include "../Tags.h"
#include "KeyBinds.h"

using namespace gce;

DECLARE_SCRIPT(SceneManagerBehavior, ScriptFlag::Start | ScriptFlag::Update)

SceneManager* pSceneManager = nullptr;
bool windowFullscreen = true;

float decreaseSpeed = 1.f;
float decreaseProgress = 0.f;

void HandleScore()
{
	if (GameManager::GetSceneManager().GetSceneType() != SceneType::GamePlayScene)
		return;

	if (decreaseProgress < decreaseSpeed)
	{
		decreaseProgress += GameManager::DeltaTime();
	}
	else
	{
		ScoreManager::Add(-1);
		decreaseProgress = 0.f;
	}
}

void Start()
{
	pSceneManager = &GameManager::GetSceneManager();
}

void Update()
{
	if (pSceneManager == nullptr)
		return;

	if (GetKeyDown(KeyBinds::GetKeyBind(KeyAction::ToggleFullScreen)))
	{
		pSceneManager->ToggleFullScreenMode();
	}

	HandleScore();
}

END_SCRIPT