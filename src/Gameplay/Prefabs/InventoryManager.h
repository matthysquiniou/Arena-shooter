#pragma once
#include "GameObject.h"
#include <vector>

class Ammos;
class SceneManager;

class InventoryManager
{
	SceneManager* m_pSceneManager;

	int m_inventoryIndex = 0;

	std::vector<gce::GameObject*> m_allWeapons;
	std::vector<gce::GameObject*> m_allCollectibles;

	std::vector<gce::GameObject*> m_currentInventory;

	std::vector<Ammos*> m_ammoStock;

	gce::GameObject* m_pEquipedObject = nullptr;

	//Weapons Content
	gce::GameObject* CreateMusket();
	gce::GameObject* CreateBlunderBuss();
	gce::GameObject* CreateStarwheel();
	gce::GameObject* CreateMeleeWeapon();

public:
	~InventoryManager();

	void InitAll();
	void ResetAll();

	std::vector<gce::GameObject*> GetWeapons();
	gce::GameObject* GetWeapon(Tag tag);
	std::vector<gce::GameObject*> GetCollectibles();
	gce::GameObject* GetCollectible(Tag tag);

	void InitInventoryState();

	void SwapEquipedObject(bool forward); // Forward = true to go on the next item in the Inventory, false to go backward
	void SetEquipedObjectByIndex(int index); // Equip instantly the new object with based on its location in the Inventory
	gce::GameObject* GetCurrentEquipedObject() { return m_pEquipedObject; }

	std::vector<gce::GameObject*>& GetCurrentInventory() { return m_currentInventory; }
	std::vector<Ammos*>& GetAmmoStock() { return m_ammoStock; }

	Ammos* GetAmmos(Tag type);
};

