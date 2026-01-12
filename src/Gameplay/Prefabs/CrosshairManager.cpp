#include "CrosshairManager.h"
#include "GameManager.h"
#include "../SceneManager.h"
#include "../Prefabs/InventoryManager.h"

void CrosshairManager::Init()
{
	gce::Vector2f32 center = { (float)WINDOW_WIDTH / 2.f + 10, (float)WINDOW_HEIGHT / 2.f }; // TODO see if the UI accuracy depends of the screen resolution

	m_pDefaultCrosshair = &EntityWrapper::Create();
	m_pDefaultCrosshair->AddUIButton(center, { 0, 0 }, { 50, 50 }, "res/2D_Assets/Gameplay/hit_marker.png");

	m_pStarwheelCrosshair = &EntityWrapper::Create();
	m_pStarwheelCrosshair->AddUIButton(center, { 0, 0 }, { 100, 50 }, "res/2D_Assets/Gameplay/crosshair_pistolet.png");

	m_pMusketCrosshair = &EntityWrapper::Create();
	m_pMusketCrosshair->AddUIButton(center, { 0, 0 }, { 50, 50 }, "res/2D_Assets/Gameplay/crosshair_musket.png");

	m_pBlunderbussCrosshair = &EntityWrapper::Create();
	m_pBlunderbussCrosshair->AddUIButton(center, { 0, 0 }, { 75, 75 }, "res/2D_Assets/Gameplay/crosshair_blunderbuss.png");

	m_pThrowableWeaponCrosshair = &EntityWrapper::Create();
	m_pThrowableWeaponCrosshair->AddUIButton(center, { 0, 0 }, { 50, 50 }, "res/2D_Assets/Gameplay/crosshair_throwable.png");
}

void CrosshairManager::Update()
{
	SetActive(false);

	gce::GameObject* currentEquiped = gce::GameManager::GetSceneManager().GetInventoryManager()->GetCurrentEquipedObject();

	switch (currentEquiped->GetUniqueTag({ Tag::TStarwheel, Tag::TBlunderBuss, Tag::TMusket, Tag::TThrowableWeapon }))
	{
	case Tag::TStarwheel:
		m_pStarwheelCrosshair->SetActive(true);
		break;
	case Tag::TBlunderBuss:
		m_pBlunderbussCrosshair->SetActive(true);
		break;
	case Tag::TMusket:
		m_pMusketCrosshair->SetActive(true);
		break;
	case Tag::TThrowableWeapon:
		m_pThrowableWeaponCrosshair->SetActive(true);
		break;
	default:
		m_pDefaultCrosshair->SetActive(true);
		break;
	}
}

void CrosshairManager::SetActive(bool state)
{
	if (state == true)
	{
		return;
	}

	if (m_pMeleeCrosshair)
		m_pMeleeCrosshair->SetActive(false);

	if (m_pStarwheelCrosshair)
		m_pStarwheelCrosshair->SetActive(false);

	if (m_pMusketCrosshair)
		m_pMusketCrosshair->SetActive(false);

	if (m_pBlunderbussCrosshair)
		m_pBlunderbussCrosshair->SetActive(false);

	if (m_pThrowableWeaponCrosshair)
		m_pThrowableWeaponCrosshair->SetActive(false);

	if (m_pDefaultCrosshair)
		m_pDefaultCrosshair->SetActive(false);
}
