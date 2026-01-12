#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"
#include "WaveManager.h"

using namespace gce;

DECLARE_SCRIPT(WaveManagerBehavior, ScriptFlag::Start | ScriptFlag::Update)

//Members
WaveManager* pWaveManager = nullptr;

void Start()
{
	pWaveManager = WaveManager::GetInstance();
}

void Update()
{
	if(GameManager::GetSceneManager().GetSceneType() == SceneType::GamePlayScene)
		pWaveManager->Update();
}

END_SCRIPT
