#pragma once
#include "EntityWrapper.h"

class CrosshairManager
{
	EntityWrapper* m_pMeleeCrosshair = nullptr;
	EntityWrapper* m_pStarwheelCrosshair = nullptr;
	EntityWrapper* m_pMusketCrosshair = nullptr;
	EntityWrapper* m_pBlunderbussCrosshair = nullptr;

	EntityWrapper* m_pThrowableWeaponCrosshair = nullptr;
	EntityWrapper* m_pDefaultCrosshair = nullptr; // Used for the not defined Tags

public:
	void Init();
	void Update();
	void SetActive(bool state);
};

