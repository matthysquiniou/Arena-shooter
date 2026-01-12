#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"
#include "Agent.h"
#include "HealthBehavior.hpp"
#include "WaveManager.h"


#include <CollectibleMunitionManager.h>

using namespace gce;

DECLARE_SCRIPT(DragonHpBehavior, ScriptFlag::Start | ScriptFlag::Update)

bool animHitTrigger = false;
float animHitDuration = 0.1f;
float animHitProgress = 0.f;

bool animDeathTrigger = false;
float animDeathDuration = 0.5f;
float animDeathProgress = 0.f;

void ResetTexture()
{
	m_pOwner->GetComponent<MeshRenderer>()->pMaterial->albedoTextureID = m_pOwner->GetDefaultTexture()->GetTextureID();
}

void TriggerHitAnim() { ResetTexture(); animHitTrigger = true; animHitProgress = 0.f; }

void HandleDeathAnimation()
{
	float dt = GameManager::DeltaTime();
	 
	float value = (gce::PI / 2) *50 *(GameManager::DeltaTime() / animDeathDuration);

	m_pOwner->transform.WorldRotate({ value, 0.f, 0.f });
}

void HandleDeath()
{
	//DragonDeath 

	m_pOwner->transform.SetWorldRotation({ 0, 0, 0 });
	m_pOwner->SetActive(false);

	WaveManager::GetInstance()->EnnemyKilled(dynamic_cast<Agent*>(m_pOwner));

	animDeathProgress = 0.f;
	animDeathTrigger = false;

	animHitProgress = 0.f;
	animHitTrigger = false;
	ResetTexture();
}

void Start()
{
}

void Update()
{
	if (animDeathTrigger)
	{
		if (animDeathProgress < animDeathDuration)
		{
			animDeathProgress += GameManager::DeltaTime();
			HandleDeathAnimation();
		}
		else
		{
			animDeathTrigger = false;
			HandleDeath();
			GameManager::GetSceneManager().ChangeScene(SceneType::WinScene);
		}

		return;
	}

	if (animHitTrigger)
	{
		if (animHitProgress < animHitDuration)
		{
			if (animHitProgress == 0.f)
			{
				AudioUse::Play("dragon_roar");
			}

			animHitProgress += GameManager::DeltaTime();
			m_pOwner->GetComponent<MeshRenderer>()->pMaterial->albedoTextureID = GameManager::s_pInstance->m_pRedTexture->GetTextureID();
		}
		else
		{
			animHitTrigger = false;
			ResetTexture();
		}
	}

	if (auto health = m_pOwner->GetScript<HealthBehavior>())
	{
		if (health->IsAlive() == false)
		{
			animDeathTrigger = true;
		}
	}
}

END_SCRIPT
