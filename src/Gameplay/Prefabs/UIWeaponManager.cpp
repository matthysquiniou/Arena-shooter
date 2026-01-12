#include "UIWeaponManager.h"
#include "../SceneManager.h"
#include "InventoryManager.h"

void UIWeaponManager::Init()
{
	gce::Vector2f32 middle = { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };

	gce::Vector2f32 starwheelUIPos = { WINDOW_WIDTH - 200, WINDOW_HEIGHT - 300 };
	m_pStarwheelUI.first = &EntityWrapper::Create();
	m_pStarwheelUI.first->AddUIButton(starwheelUIPos, {0, 0}, {221, 78}, "res/2D_Assets/Gameplay/starwheel_OFF.png");
	m_pStarwheelUI.second = &EntityWrapper::Create();
	m_pStarwheelUI.second->AddUIButton(starwheelUIPos, { 0, 0 }, { 231, 87 }, "res/2D_Assets/Gameplay/starwheel_ON.png");

	gce::Vector2f32 blunderbussUIPos = { WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200 };
	m_pBlunderbussUI.first = &EntityWrapper::Create();
	m_pBlunderbussUI.first->AddUIButton(blunderbussUIPos, { 0, 0 }, { 221, 64 }, "res/2D_Assets/Gameplay/blunderbuss_OFF.png");
	m_pBlunderbussUI.second = &EntityWrapper::Create();
	m_pBlunderbussUI.second->AddUIButton(blunderbussUIPos, { 0, 0 }, { 231, 74 }, "res/2D_Assets/Gameplay/blunderbuss_ON.png");

	gce::Vector2f32 musketUIPos = { WINDOW_WIDTH - 200, WINDOW_HEIGHT - 100 };
	m_pMusketUI.first = &EntityWrapper::Create();
	m_pMusketUI.first->AddUIButton(musketUIPos, { 0, 0 }, { 224, 74 }, "res/2D_Assets/Gameplay/musket_OFF.png");
	m_pMusketUI.second = &EntityWrapper::Create();
	m_pMusketUI.second->AddUIButton(musketUIPos, { 0, 0 }, { 233, 84 }, "res/2D_Assets/Gameplay/musket_ON.png");

	gce::Vector2f32 ammoUIPos = { WINDOW_WIDTH - 450, WINDOW_HEIGHT - 100 };
	m_pLightAmmoUI = &EntityWrapper::Create();
	m_pLightAmmoUI->AddUIButton(ammoUIPos, { 0, 0 }, { 62 * 1.5f, 61 * 1.5f }, "res/2D_Assets/Gameplay/light_ammo.png");

	m_pNormalAmmoUI = &EntityWrapper::Create();
	m_pNormalAmmoUI->AddUIButton(ammoUIPos, { 0, 0 }, { 54 * 1.5f, 63 * 1.5f }, "res/2D_Assets/Gameplay/normal_ammo.png");

	m_pHeavyAmmoUI = &EntityWrapper::Create();
	m_pHeavyAmmoUI->AddUIButton(ammoUIPos, { 0, 0 }, { 58 * 1.5f, 62 * 1.5f }, "res/2D_Assets/Gameplay/heavy_ammo.png");
}

void UIWeaponManager::Update()
{
	gce::GameObject* pEquiped = gce::GameManager::GetSceneManager().GetInventoryManager()->GetCurrentEquipedObject();

	if (pEquiped == nullptr)
		return;

	m_pStarwheelUI.first->SetActive(true);
	m_pBlunderbussUI.first->SetActive(true);
	m_pMusketUI.first->SetActive(true);

	m_pStarwheelUI.second->SetActive(false);
	m_pBlunderbussUI.second->SetActive(false);
	m_pMusketUI.second->SetActive(false);

	m_pLightAmmoUI->SetActive(false);
	m_pNormalAmmoUI->SetActive(false);
	m_pHeavyAmmoUI->SetActive(false);

	switch (pEquiped->GetUniqueTag({ Tag::TStarwheel, Tag::TBlunderBuss, Tag::TMusket }))
	{
	case Tag::TStarwheel:
		m_pStarwheelUI.second->SetActive(true);
		m_pLightAmmoUI->SetActive(true);
		break;

	case Tag::TBlunderBuss:
		m_pBlunderbussUI.second->SetActive(true);
		m_pHeavyAmmoUI->SetActive(true);
		break;

	case Tag::TMusket:
		m_pMusketUI.second->SetActive(true);
		m_pNormalAmmoUI->SetActive(true);
		break;

	default:
		break;
	}
}

void UIWeaponManager::SetActive(bool state)
{
	if (state == true)
		return;

	m_pStarwheelUI.first->SetActive(false);
	m_pStarwheelUI.second->SetActive(false);

	m_pBlunderbussUI.first->SetActive(false);
	m_pBlunderbussUI.second->SetActive(false);

	m_pMusketUI.first->SetActive(false);
	m_pMusketUI.second->SetActive(false);

	m_pLightAmmoUI->SetActive(false);
	m_pNormalAmmoUI->SetActive(false);
	m_pNormalAmmoUI->SetActive(false);
}