#pragma once
#include "EntityWrapper.h"

class UIWeaponManager
{
	std::pair<EntityWrapper*, EntityWrapper*> m_pStarwheelUI = { nullptr, nullptr };
	std::pair<EntityWrapper*, EntityWrapper*> m_pBlunderbussUI = { nullptr, nullptr };
	std::pair<EntityWrapper*, EntityWrapper*> m_pMusketUI= { nullptr, nullptr };

	EntityWrapper* m_pLightAmmoUI = nullptr;
	EntityWrapper* m_pNormalAmmoUI = nullptr;
	EntityWrapper* m_pHeavyAmmoUI = nullptr;
public:
	void Init();
	void Update();
	void SetActive(bool state);
};

