#include "pch.h"
#include "InventoryManager.h"
#include "EntityWrapper.h"

#include "Scripts/GunBehavior.hpp"
#include "Scripts/MeleeWeaponBehavior.hpp"
#include "Ammos.h"

InventoryManager::~InventoryManager()
{
	for (Ammos* a : m_ammoStock)
	{
		delete a;
	}

	m_ammoStock.clear();
}

//Weapons Content
gce::GameObject* InventoryManager::CreateMusket()
{
	EntityWrapper& musket = EntityWrapper::Create();

	musket.SetProperties("Musket", { Tag::TWeapon, Tag::TMusket }, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
	m_pSceneManager->GetCameraObject()->AddChild(musket);

	musket.transform.SetLocalPosition({ 0.25, -0.1f, 0.5f });

	musket.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/musket/musket.obj"), "res/Assets/musket/musket_base_color.png");
	EntityWrapper& hole = EntityWrapper::Create();
	gce::Vector3f32 holePos = {};
	holePos.z -= 0.4;
	holePos.y += 0.02;

	hole.SetChildProperties(musket, "Musket Hole", { Tag::TMiscellaneous }, { 0, 0, 0 }, { 0, 0, 0 }, { 0.05, 0.05, 0.05 });
	hole.transform.LocalTranslate(holePos);

	auto ammoManagerScript = musket.AddScript<WeaponMagazineBehavior>();
	ammoManagerScript->SetAmmoType(Tag::TNormalAmmo);
	ammoManagerScript->SetMaxCapacity(30);

	auto gunBehavior = musket.AddScript<GunBehavior>();
	gunBehavior->SetUnloadSpeed(0.25);
	gunBehavior->SetReloadTime(1.2f);
	gunBehavior->SetAmmoManagerScript(ammoManagerScript);

	m_pSceneManager->LinkObjectToScene(&musket, SceneType::GamePlayScene);
	m_pSceneManager->LinkObjectToScene(&hole, SceneType::GamePlayScene);

	return &musket;
}

gce::GameObject* InventoryManager::CreateBlunderBuss()
{
	EntityWrapper& blunderbuss = EntityWrapper::Create();

	blunderbuss.SetProperties("Blunderbuss", { Tag::TWeapon, Tag::TBlunderBuss }, { 0, 0, 0.f }, { 0, 0, 0 }, { 1, 1, 1 });
	m_pSceneManager->GetCameraObject()->AddChild(blunderbuss);

	blunderbuss.transform.SetLocalPosition({ 0.25, -0.1f, 0.5f });

	blunderbuss.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/blunderbuss/blunderbuss.obj"), "res/Assets/blunderbuss/blunderbuss_base_color.png");

	EntityWrapper& hole = EntityWrapper::Create();
	gce::Vector3f32 holePos = {};
	holePos.z -= 0.8;
	holePos.y += 0.f;

	hole.SetChildProperties(blunderbuss, "Blunderbuss Hole", { Tag::TMiscellaneous }, { 0, 0, 0 }, { 0, 0, 0 }, { 0.1, 0.1, 0.1 });
	hole.transform.LocalTranslate(holePos);

	auto ammoManagerScript = blunderbuss.AddScript<WeaponMagazineBehavior>();
	ammoManagerScript->SetAmmoType(Tag::THeavyAmmo);
	ammoManagerScript->SetMaxCapacity(8);

	auto gunBehavior = blunderbuss.AddScript<GunBehavior>();
	gunBehavior->SetUnloadSpeed(0.75);
	gunBehavior->SetReloadTime(1.5f);
	gunBehavior->SetAmmoManagerScript(ammoManagerScript);

	m_pSceneManager->LinkObjectToScene(&blunderbuss, SceneType::GamePlayScene);
	m_pSceneManager->LinkObjectToScene(&hole, SceneType::GamePlayScene);

	return &blunderbuss;
}

gce::GameObject* InventoryManager::CreateStarwheel()
{
	EntityWrapper& starwheel = EntityWrapper::Create();

	starwheel.SetProperties("Starwheel", { Tag::TWeapon, Tag::TStarwheel }, { 0, 0, 0 }, { 0, 0, 0 }, { 2.f, 2.f, 2.f });
	m_pSceneManager->GetCameraObject()->AddChild(starwheel);

	starwheel.transform.SetLocalPosition({ 0.3f, -0.15f, 0.5f });
	starwheel.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/starwheel/starwheel.obj"), "res/Assets/starwheel/starwheel_base_color.png");
	EntityWrapper& hole = EntityWrapper::Create();
	gce::Vector3f32 holePos = {};
	holePos.z -= 0.1f;
	holePos.y += 0.02;

	hole.SetChildProperties(starwheel, "Starwheel Hole", { Tag::TMiscellaneous }, { 0, 0, 0 }, { 0, 0, 0 }, { 0.02, 0.02, 0.02 });
	hole.transform.LocalTranslate(holePos);

	auto ammoManagerScript = starwheel.AddScript<WeaponMagazineBehavior>();
	ammoManagerScript->SetAmmoType(Tag::TLightAmmo);
	ammoManagerScript->SetMaxCapacity(20);

	auto gunBehavior = starwheel.AddScript<GunBehavior>();
	gunBehavior->SetUnloadSpeed(0.2);
	gunBehavior->SetReloadTime(1.5f);
	gunBehavior->SetAmmoManagerScript(ammoManagerScript);

	m_pSceneManager->LinkObjectToScene(&starwheel, SceneType::GamePlayScene);
	m_pSceneManager->LinkObjectToScene(&hole, SceneType::GamePlayScene);

	return &starwheel;
}

gce::GameObject* InventoryManager::CreateMeleeWeapon()
{
	EntityWrapper& melee = EntityWrapper::Create();

	melee.SetProperties("Spear", { Tag::TMeleeWeapon, Tag::TSpear }, { 0, 0, 0 }, { 0, 0, 0 }, { 1.f, 1.f, 1.f });
	m_pSceneManager->GetCameraObject()->AddChild(melee);

	EntityWrapper& hitbox = EntityWrapper::Create();
	gce::Vector3f32 holePos = {};
	holePos.x += 2.f;
	holePos.y += 0.3f;
	holePos.z += 1.f;

	hitbox.SetChildProperties(melee, "Melee Hitbox", { Tag::TMiscellaneous }, { 0, 0, 0 }, { 0, 0, 0 }, { 1.5, 1.5, 1.5 });
	hitbox.transform.LocalTranslate(holePos);
	hitbox.AddMeshRenderer(SHAPES.SPHERE, "")->SetActive(false);

	Quaternion yQ = {};
	yQ.SetRotationEuler({ 0, -gce::PI / 3, 0 });

	Quaternion xQ = {};
	xQ.SetRotationEuler({ -gce::PI / 8, 0, 0 });

	melee.transform.SetLocalRotation(xQ * yQ);
	melee.transform.SetLocalPosition({ 0.025f, -0.2f, 0.5f });
	melee.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/spear/spear.obj"), "res/Assets/spear/spear_base_color.png")->SetActive(true);
	auto meleeBehavior = melee.AddScript<MeleeWeaponBehavior>();
	meleeBehavior->SetMeleeWeaponProperties(25, 0.5f);
	meleeBehavior->SetDefaultRotation(xQ * yQ);

	m_pSceneManager->LinkObjectToScene(&melee, SceneType::GamePlayScene);
	m_pSceneManager->LinkObjectToScene(&hitbox, SceneType::GamePlayScene);

	return &melee;
}



void InventoryManager::InitAll()
{
	m_pSceneManager = &GameManager::GetSceneManager();

	m_allWeapons.push_back(CreateMeleeWeapon());
	m_allWeapons.push_back(CreateMusket());
	m_allWeapons.push_back(CreateBlunderBuss());
	m_allWeapons.push_back(CreateStarwheel());

	m_currentInventory.push_back(GetWeapon(Tag::TSpear));
	m_currentInventory.push_back(GetWeapon(Tag::TMusket));
	m_currentInventory.push_back(GetWeapon(Tag::TBlunderBuss));
	m_currentInventory.push_back(GetWeapon(Tag::TStarwheel));

	// AMMMOS
	m_ammoStock.push_back(new Ammos(Tag::THeavyAmmo, 30));
	m_ammoStock.push_back(new Ammos(Tag::TNormalAmmo, 100));
	m_ammoStock.push_back(new Ammos(Tag::TLightAmmo, 200));
}

void InventoryManager::ResetAll()
{
	m_inventoryIndex = 0;
	m_pEquipedObject = nullptr;

	m_currentInventory.clear();

	for (Ammos* ammos : m_ammoStock)
	{
		delete ammos;
	}

	m_allWeapons.clear();
	m_allCollectibles.clear();

	m_ammoStock.clear();
}

std::vector<gce::GameObject*> InventoryManager::GetWeapons()
{
	if (m_allWeapons.empty() == false)
	{
		std::vector<gce::GameObject*> weapons = m_allWeapons;

		return weapons;
	}

	return gce::GameManager::GetSceneManager().GetAllGameObjects({ Tag::TWeapon });
}

gce::GameObject* InventoryManager::GetWeapon(Tag tag)
{
	if(m_allWeapons.empty() == false)
	{
		for (gce::GameObject* go : m_allWeapons)
		{
			if (go->HasTags({ tag }))
				return go;
		}
	}

	return gce::GameManager::GetSceneManager().GetFirstGameObject({ Tag::TWeapon, tag });
}

std::vector<gce::GameObject*> InventoryManager::GetCollectibles()
{
	if (m_allCollectibles.empty() == false)
	{
		std::vector<gce::GameObject*> collectibles = m_allCollectibles;

		return collectibles;
	}

	return gce::GameManager::GetSceneManager().GetAllGameObjects({ Tag::TThrowableWeapon });
}

gce::GameObject* InventoryManager::GetCollectible(Tag tag)
{
	if (m_allCollectibles.empty() == false)
	{
		for (gce::GameObject* go : m_allCollectibles)
		{
			if (go->HasTags({ tag }))
				return go;
		}
	}

	return gce::GameManager::GetSceneManager().GetFirstGameObject({ Tag::TThrowableWeapon, tag });
}

void InventoryManager::InitInventoryState()
{
	for (gce::GameObject* pWeapon : m_allWeapons)
	{
		pWeapon->SetActive(false);
	}

	for (gce::GameObject* pCollectible : m_allCollectibles)
	{
		pCollectible->SetActive(false);
	}


	if (gce::GameObject* first = m_currentInventory[0])
	{
		first->SetActive(true);
		m_pEquipedObject = first;
	}
}

void InventoryManager::SwapEquipedObject(bool forward)
{
	int sens = forward ? 1 : -1;

	if (m_inventoryIndex + sens < 0)
		return;

	if (m_inventoryIndex + sens > m_currentInventory.size() - 1)
		return;

	if (auto current = m_currentInventory[m_inventoryIndex])
	{
		if (current->HasTags({ Tag::TWeapon }))
			current->GetScript<GunBehavior>()->OnLeaveWeapon(); // The old weapon

		if (current->HasTags({ Tag::TMeleeWeapon }))
			current->GetScript<MeleeWeaponBehavior>()->OnLeaveWeapon(); // The old weapon
	}

	m_inventoryIndex += sens;

	gce::GameObject* newEquipedObject = m_currentInventory[m_inventoryIndex];

	for (gce::GameObject* pObject : m_currentInventory)
	{
		//Only the new Equiped Object is active

		if (pObject == newEquipedObject) // if the current checked Object among the inventory is the new equiped
		{
			pObject->SetActive(true);
			m_pEquipedObject = pObject;

			if (pObject->HasTags({ Tag::TWeapon }))
				pObject->GetScript<GunBehavior>()->OnReceiveWeapon();

			if (pObject->HasTags({ Tag::TMeleeWeapon }))
				pObject->GetScript<MeleeWeaponBehavior>()->OnReceiveWeapon();
		}
		else
		{
			pObject->SetActive(false);
		}
	}
}

void InventoryManager::SetEquipedObjectByIndex(int index)
{
	if (index == m_inventoryIndex) // already equiped
		return;

	if (index < 0) // outrange
		return;

	if (index > m_currentInventory.size() - 1) // outrange
		return;

	if (auto current = m_currentInventory[m_inventoryIndex])
	{
		if (current->HasTags({ Tag::TWeapon }))
			current->GetScript<GunBehavior>()->OnLeaveWeapon(); // The old weapon

		if (current->HasTags({ Tag::TMeleeWeapon }))
			current->GetScript<MeleeWeaponBehavior>()->OnLeaveWeapon();
	}

	m_inventoryIndex = index;

	gce::GameObject* newEquipedObject = m_currentInventory[m_inventoryIndex];

	for (gce::GameObject* pObject : m_currentInventory)
	{
		//Only the new Equiped Object is active

		if (pObject == newEquipedObject) // if the current checked Object among the inventory is the new equiped
		{
			pObject->SetActive(true);
			m_pEquipedObject = pObject;

			if (pObject->HasTags({ Tag::TWeapon }))
				pObject->GetScript<GunBehavior>()->OnReceiveWeapon();

			if (pObject->HasTags({ Tag::TMeleeWeapon }))
				pObject->GetScript<MeleeWeaponBehavior>()->OnReceiveWeapon();
		}
		else
		{
			pObject->SetActive(false);
		}
	}
}

Ammos* InventoryManager::GetAmmos(Tag type)
{
	for (Ammos* a : m_ammoStock)
	{
		if (a->IsType(type))
			return a;
	}

	return nullptr;
}
