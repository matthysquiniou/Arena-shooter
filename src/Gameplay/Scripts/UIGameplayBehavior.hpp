#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include "Components.h"
#include "GameManager.h"
#include "SceneManager.h"

#include "Prefabs/EntityWrapper.h"

#include "Scripts/WeaponMagazineBehavior.hpp"
#include "Scripts/GunBehavior.hpp"
#include "Scripts/HealthBehavior.hpp"
#include "Scripts/PlayerBehavior.hpp"
#include "Scripts/DragonBehavior.hpp"
#include "Scripts/DragonHpBehavior.hpp"

#include "../Prefabs/InventoryManager.h"
#include "../Prefabs/UIManager.h"
#include "../Prefabs/UiBar.h"
#include "../Prefabs/CrosshairManager.h"
#include "../Prefabs/UIWeaponManager.h"
#include "../WaveManager.h"
#include "../Prefabs/ScoreManager.h"

using namespace gce;

DECLARE_SCRIPT(UIGameplayBehavior, ScriptFlag::Start | ScriptFlag::Update)

UIManager* pUIManager = nullptr;

//Members
EntityWrapper* pScoreUI = nullptr;
std::wstring scoreTxt;

EntityWrapper* pAmmoUI = nullptr;
std::wstring ammoTxt;

EntityWrapper* pTotalAmmoUI = nullptr;
std::wstring totalAmmoTxt;

UIWeaponManager uiWeapon;
CrosshairManager crosshair;

UiBar hpBar;
UiBar waveBar;
UiBar dashBar;

UiBar dragonBar;

std::wstring NOTHING = L"";

EntityWrapper* pTakeDamageUI = nullptr;
int oldPlayerHp = -1;
bool takeDamageTriggerVisual = false;
float takeDamageDuration = 0.75f;
float takeDamageProgressDuration;


//TMP
EntityWrapper* pWaveUI = nullptr;
std::wstring waveTxt = L"";

//Functions
void UpdateTakeDamageUI()
{
	auto pPlayer = GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer });

	if (takeDamageTriggerVisual)
	{
		if (takeDamageProgressDuration < takeDamageDuration)
		{
			takeDamageProgressDuration += GameManager::DeltaTime();
			pTakeDamageUI->SetActive(true);
		}
		else
		{
			takeDamageProgressDuration = 0.f;
			takeDamageTriggerVisual = false;
			pTakeDamageUI->SetActive(false);
		}
	}

	if (HealthBehavior* pScript = pPlayer->GetScript<HealthBehavior>())
	{
		if (oldPlayerHp == -1)
		{
			if(pScript->health > 0)
				oldPlayerHp = pScript->health;

			return;
		}

		if (pScript->health < oldPlayerHp)
		{
			oldPlayerHp = pScript->health;
			takeDamageProgressDuration = 0.f;
			takeDamageTriggerVisual = true;

			if (pScript->health <= 0)
			{
				oldPlayerHp = -1;
			}
		}
	}
}

void UpdateScoreUI()
{
	scoreTxt = L"Score : " + std::to_wstring(ScoreManager::GetScore());
	pScoreUI->UpdateDynamicText(scoreTxt);
}
void UpdateAmmosUI()
{
	gce::GameObject* pObj = GameManager::GetSceneManager().GetInventoryManager()->GetCurrentEquipedObject();

	if (pObj == nullptr)
		return;

	if (pObj->HasTags({ Tag::TWeapon }))
	{
		WeaponMagazineBehavior* pScript = pObj->GetScript<WeaponMagazineBehavior>();

		if (pScript)
		{
			ammoTxt = std::to_wstring(pScript->ammosLeft) + L"/" + std::to_wstring(pScript->maxCapacity);
		}
		else
		{
			ammoTxt = NOTHING;
		}
	}
	else
	{
		ammoTxt = NOTHING;
	}

	pAmmoUI->UpdateDynamicText(ammoTxt);
}
void UpdateTotalAmmoUI()
{
	InventoryManager* pInventory = GameManager::GetSceneManager().GetInventoryManager();

	gce::GameObject* pObj = pInventory->GetCurrentEquipedObject();

	if (pObj == nullptr)
		return;

	if (pObj->HasTags({ Tag::TWeapon }))
	{
		WeaponMagazineBehavior* pScript = pObj->GetScript<WeaponMagazineBehavior>();

		if (pScript)
		{
			Ammos* pAmmoToDisplay = pInventory->GetAmmos(pScript->typeOfAmmo);

			if (pAmmoToDisplay)
			{
				totalAmmoTxt = std::to_wstring(pAmmoToDisplay->GetAmount());
			}
			else
			{
				totalAmmoTxt = NOTHING;
			}
		}
		else
		{
			totalAmmoTxt = NOTHING;
		}
	}
	else
	{
		totalAmmoTxt = NOTHING;
	}

	pTotalAmmoUI->UpdateDynamicText(totalAmmoTxt);
}
void UpdateDashUI()
{
	gce::GameObject* pPlayer = GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer });

	if (pPlayer == nullptr)
		return;

	PlayerBehavior* script = pPlayer->GetScript<PlayerBehavior>();

	if (script)
	{
		int dashAmount = script->dashAmount;
		int dashReloadTimeHalf = script->dashTotalReloadTime / script->maxDashAmount;

		if (script->dashProgressReloadTime < dashReloadTimeHalf)
		{
			dashBar.SetFilledBar1ByRatio(script->dashProgressReloadTime, dashReloadTimeHalf);
			dashBar.SetFilledBar2ByRatio(0.f, 1.f);
		}
		else
		{
			dashBar.SetFilledBar1ByRatio(1.f, 1.f);
			dashBar.SetFilledBar2ByRatio(-dashReloadTimeHalf + script->dashProgressReloadTime, dashReloadTimeHalf);
		}
	}
}
void UpdateHpUI()
{
	gce::GameObject* pPlayer = GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer });

	if (pPlayer == nullptr)
		return;

	HealthBehavior* health = pPlayer->GetScript<HealthBehavior>();

	if (health)
	{
		hpBar.SetFilledBar1ByRatio(health->health, health->maxHealth);
	}
}
void UpdateWaveUI()
{
	auto wave = WaveManager::GetInstance();

	if (wave)
	{
		waveBar.SetFilledBar1ByRatio(wave->m_currentEnnemiesAlives + wave->m_currentEnnemiesToSpawn, wave->m_currentMaxEnnemiesToSpawn, true);
	}
}
void UpdateCrosshair()
{
	crosshair.Update();
}

void UpdateDragonBar()
{
	auto dragon = GameManager::GetSceneManager().GetFirstGameObject({ Tag::TEnemy, Tag::TDragon });

	if (dragon == nullptr)
		return;

	HealthBehavior* health = dragon->GetScript<HealthBehavior>();

	if (health)
	{
		dragonBar.SetFilledBar1ByRatio(health->health, health->maxHealth);
	}
}

void Start()
{
	pUIManager = GameManager::GetSceneManager().GetUIManager();

	pScoreUI = &EntityWrapper::Create();
	pScoreUI->AddDynamicTextRenderer(scoreTxt, { WINDOW_WIDTH - 500, 10, 500, 300 }, gce::Color::Black, {0.6, 0.6});

	pAmmoUI = &EntityWrapper::Create();
	gce::Vector3f32 ammoPos = { WINDOW_WIDTH - 575, WINDOW_HEIGHT - 120, 0.f };
	pAmmoUI->AddDynamicTextRenderer(ammoTxt, { ammoPos.x, ammoPos.y, 0, 0 }, gce::Color::White, { 0.6f, 0.6f });

	pTotalAmmoUI = &EntityWrapper::Create();
	gce::Vector3f32 totalAmmmoPos = { WINDOW_WIDTH - 425, WINDOW_HEIGHT - 120, 0.f };
	pTotalAmmoUI->AddDynamicTextRenderer(totalAmmoTxt, { totalAmmmoPos.x, totalAmmmoPos.y, 0, 0 }, gce::Color::White, {0.6f, 0.6f});

	pTakeDamageUI = &EntityWrapper::Create();
	gce::Vector2f32 takeDamagePos = { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };
	pTakeDamageUI->AddUIButton(takeDamagePos, { 0, 0 }, { WINDOW_WIDTH, WINDOW_HEIGHT }, "res/2D_Assets/Gameplay/takedamage_screen.png");
	pTakeDamageUI->SetActive(false);

	crosshair.Init();

	hpBar.InitFilledBar1("res/2D_Assets/Gameplay/hpBar.png", {405, 53}, { 190, 69}, { 0.5, 0.5 });
	hpBar.InitFrame("res/2D_Assets/Gameplay/hpBar_frame.png", { 569, 204 }, { 160, 60 }, { 0.5, 0.5 });

	float middleX = WINDOW_WIDTH / 2.f;

	waveBar.InitFilledBar1("res/2D_Assets/Gameplay/wave_bar.png", { 321, 23 }, { middleX + 2, 80 }, { 1.5, 1.5 });
	waveBar.InitFrame("res/2D_Assets/Gameplay/wave_frame.png", { 325, 32 }, { middleX, 80 }, { 1.5, 1.5 });

	pWaveUI = &EntityWrapper::Create();
	pWaveUI->AddDynamicTextRenderer(waveTxt, { WINDOW_WIDTH / 2.f - 420, 10, WINDOW_WIDTH, 0 }, gce::Color::Red, { 0.5f, 0.5f });

	dashBar.InitFilledBar1("res/2D_Assets/Gameplay/dash_filled1.png", { 607, 33 }, { middleX + 3, WINDOW_HEIGHT - 78 }, { 1, 1 });
	dashBar.InitFilledBar2("res/2D_Assets/Gameplay/dash_filled2.png", { 607, 33 }, { middleX + 3, WINDOW_HEIGHT - 78 }, { 1, 1 });
	dashBar.InitFrame("res/2D_Assets/Gameplay/dash_frame.png", { 639, 102 }, { middleX, WINDOW_HEIGHT - 100 }, { 1, 1 });

	uiWeapon.Init();

	dragonBar.InitFilledBar1("res/2D_Assets/Gameplay/dragon_bar.png", { 2402, 56 }, { middleX + 3, 190 }, { 0.35f, 0.35f });
	dragonBar.InitFrame("res/2D_Assets/Gameplay/dragon_frame.png", { 3505, 816 }, { middleX + 3, 110 }, { 0.35f, 0.35f });
}

void Update()
{
	bool display = pUIManager->IsSceneType(SceneType::GamePlayScene);

	pAmmoUI->SetActive(display);
	pTotalAmmoUI->SetActive(display);
	pScoreUI->SetActive(display);
	crosshair.SetActive(display);
	hpBar.SetActive(display);
	dashBar.SetActive(display);

	if (auto dragon = GameManager::GetSceneManager().GetFirstGameObject({ Tag::TEnemy, Tag::TDragon }))
	{
		dragonBar.SetActive(true);
		waveBar.SetActive(false);
		UpdateDragonBar();
		pWaveUI->SetActive(false);
	}
	else
	{
		dragonBar.SetActive(false);
		waveBar.SetActive(true);
		pWaveUI->SetActive(true);

		auto wave = WaveManager::GetInstance();

		waveTxt = L"Time : " + std::to_wstring((int)wave->m_waveTimer) + L" | Wave : " + std::to_wstring(wave->m_curentWave) + L"/" + std::to_wstring(wave->m_maxWave) + L"\n";
		pWaveUI->GetComponent<TextRenderer>()->text = waveTxt;
	}

	uiWeapon.SetActive(display);

	if (display == true)
	{
		UpdateScoreUI();
		UpdateAmmosUI();
		UpdateTotalAmmoUI();
		UpdateCrosshair();
		UpdateHpUI();
		UpdateDashUI();
		UpdateWaveUI();
		UpdateTakeDamageUI();
		uiWeapon.Update();

	}
	else
	{
		dragonBar.SetActive(false);
		pWaveUI->SetActive(false);
		pTakeDamageUI->SetActive(false);
		takeDamageTriggerVisual = false;
	}
}

END_SCRIPT
