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

DECLARE_SCRIPT(UIWinBehavior, ScriptFlag::Start | ScriptFlag::Update)

UIManager* pUIManager = nullptr;

//Members
EntityWrapper* pBackground = nullptr;
EntityWrapper* pTitle = nullptr;
EntityWrapper* pParchemin = nullptr;

EntityWrapper* pButtonPlay = nullptr;
EntityWrapper* pButtonExit = nullptr;

EntityWrapper* pScore = nullptr;
std::wstring scoreTxt;

std::wstring NOTHING = L"";

//Functions

static void OnTriggerButtonPlay()
{
	GameManager::GetSceneManager().ChangeScene(SceneType::GamePlayScene);
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
	pTitle->AddUIButton({ middle.x, 250 }, { 0, 0 }, { 882 * 0.75f, 283 * 0.75f }, "res/2D_Assets/Win/victory.png");

	//PLAY
	pButtonPlay = &EntityWrapper::Create();
	pButtonPlay->AddUIButton({ middle.x, middle.y }, { 0, 0 }, { 312, 99 }, "res/2D_Assets/Menu/play.png", "res/2D_Assets/Menu/play_survol.png")->AddListener(OnTriggerButtonPlay);

	//EXIT
	pButtonExit = &EntityWrapper::Create();
	pButtonExit->AddUIButton({ middle.x, middle.y + 150 }, { 0, 0 }, { 312, 99 }, "res/2D_Assets/Menu/exit.png", "res/2D_Assets/Menu/exit_survol.png")->AddListener(OnTriggerButtonExit);

	//Score
	pScore = &EntityWrapper::Create();
	pScore->AddDynamicTextRenderer(scoreTxt, { middle.x - 450, middle.y - 200, middle.x, 0 }, gce::Color::Black, { 0.75, 0.75 });
	scoreTxt = std::to_wstring(ScoreManager::GetScore());
}

void Update()
{
	bool display = pUIManager->IsSceneType(SceneType::WinScene);

	if (display == true)
		ShowMouseCursor();

	pBackground->SetActive(display);
	pParchemin->SetActive(display);

	pTitle->SetActive(display);

	pButtonPlay->SetActive(display);
	pButtonExit->SetActive(display);

	pScore->SetActive(display);

	if (display == true)
	{
		scoreTxt = L"Score : " + std::to_wstring(ScoreManager::GetScore());
		pScore->GetComponent<TextRenderer>()->text = scoreTxt;
	}
}

END_SCRIPT
