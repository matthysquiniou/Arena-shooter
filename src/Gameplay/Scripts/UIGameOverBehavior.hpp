#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"
#include "Prefabs/EntityWrapper.h"

#include "Scripts/WeaponMagazineBehavior.hpp"
#include "Scripts/GunBehavior.hpp"
#include "Scripts/HealthBehavior.hpp"

#include "GameManager.h"
#include "SceneManager.h"
#include "Prefabs/InventoryManager.h"
#include "Prefabs/UIManager.h"

using namespace gce;

DECLARE_SCRIPT(UIGameOverBehavior, ScriptFlag::Start | ScriptFlag::Update)

UIManager* pUIManager = nullptr;

//Members
EntityWrapper* pBackground = nullptr;
EntityWrapper* pTitle = nullptr;
EntityWrapper* pParchemin = nullptr;

EntityWrapper* pButtonPlay = nullptr;
EntityWrapper* pButtonExit = nullptr;

std::wstring NOTHING = L"";

//Functions

static void OnTriggerButtonPlay()
{
	GameManager::GetSceneManager().ChangeScene(SceneType::GamePlayScene);
}

static void OnTriggerButtonSettings()
{

}

static void OnTriggerButtonExit()
{
	GameManager::GetSceneManager().ChangeScene(SceneType::MenuScene);
}

void Start()
{
	gce::Vector2f32 middle = { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };

	pUIManager = GameManager::GetSceneManager().GetUIManager();

	pBackground = &EntityWrapper::Create();
	pBackground->AddUIButton(middle, { 0, 0 }, { 1920, 1080 }, "res/2D_Assets/Menu/menu_bg.png");

	pParchemin = &EntityWrapper::Create();
	pParchemin->AddUIButton({ middle.x, middle.y }, { 0, 0 }, { 581, 1026 }, "res/2D_Assets/Menu/parchemin.png");

	pTitle = &EntityWrapper::Create();
	pTitle->AddUIButton({ middle.x, 250 }, { 0, 0 }, { 892 * 0.75f, 280 * 0.75f }, "res/2D_Assets/Menu/gameover_title.png");

	//PLAY
	pButtonPlay = &EntityWrapper::Create();
	pButtonPlay->AddUIButton({ middle.x, middle.y }, { 0, 0 }, { 312, 99 }, "res/2D_Assets/Menu/play.png", "res/2D_Assets/Menu/play_survol.png")->AddListener(OnTriggerButtonPlay);

	//EXIT
	pButtonExit = &EntityWrapper::Create();
	pButtonExit->AddUIButton({ middle.x, middle.y + 150 }, { 0, 0 }, { 312, 99 }, "res/2D_Assets/Menu/exit.png", "res/2D_Assets/Menu/exit_survol.png")->AddListener(OnTriggerButtonExit);
}

void Update()
{
	bool display = pUIManager->IsSceneType(SceneType::GameOverScene);

	if (display == true)
		ShowMouseCursor();

	pBackground->SetActive(display);
	pParchemin->SetActive(display);
	pTitle->SetActive(display);
	pButtonPlay->SetActive(display);
	pButtonExit->SetActive(display);
}

END_SCRIPT
