
#pragma once
#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "../Prefabs/BonusManager.h"

using namespace gce;

DECLARE_SCRIPT(HealSpawnerBehavior, ScriptFlag::Start | ScriptFlag::Update)

GameObject* pNoodle = nullptr;
GameObject* pNem = nullptr;
GameObject* pRiceBowl = nullptr;
float CDTime = 0.f;

bool init = false;

void DesactiveAll()
{
	pNoodle->SetActive(false);
	pNem->SetActive(false);
	pRiceBowl->SetActive(false);
}

void GenerateHeal()
{
	DesactiveAll();

	int random = rand() & 11;
	if (random == 10)
	{
		pNoodle->SetActive(true);
	}
	else if (random >= 7)
	{
		pNem->SetActive(true);
	}
	else
	{
		pRiceBowl->SetActive(true);
	}
	CDTime = 60.f;
}

void Start()
{

}

void Update()
{
	if (init == false)
	{
		gce::Vector3f32 pos = m_pOwner->transform.GetWorldPosition();
		pos.y += 1;
		pNoodle = BonusManager::CreateNoodles(pos);
		pNem = BonusManager::CreateNem(pos);
		pRiceBowl = BonusManager::CreateRiceBowl(pos);
		GenerateHeal();

		init = true;
		return;
	}

	if (!pNoodle->IsActive() && !pNem->IsActive() && !pRiceBowl->IsActive())
		CDTime -= GameManager::DeltaTime();

	if (CDTime <= 0)
		GenerateHeal();
}
END_SCRIPT