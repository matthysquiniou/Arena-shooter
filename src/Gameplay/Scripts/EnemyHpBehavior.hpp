#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"
#include "Agent.h"
#include "HealthBehavior.hpp"
#include "WaveManager.h"

#include "Scripts/MogwaiBehavior.hpp"
#include "Scripts/GuhuoniaoBehavior.hpp"
#include "Scripts/JiangshiBehavior.hpp"

#include <CollectibleMunitionManager.h>
#include "../Prefabs/ScoreManager.h"

using namespace gce;

DECLARE_SCRIPT(EnemyHpBehavior, ScriptFlag::Start | ScriptFlag::Update)

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

	float value = (gce::PI / 2) * (GameManager::DeltaTime() / animDeathDuration);

	m_pOwner->transform.WorldRotate({ value, 0.f, 0.f });
}

void HandleDeath()
{
	CollectibleMunitionManager::CreateRandomMunition(m_pOwner->transform.GetWorldPosition() + gce::Vector3f32{0.f,0.5f,0.f});
	m_pOwner->RemoveComponent<BoxCollider>();

	if (auto mogwaiScript = m_pOwner->GetScript<MogwaiBehavior>())
		mogwaiScript->Reset();

	else if (auto guHuoNiaoScript = m_pOwner->GetScript<GuHuoNiaoBehavior>())
		guHuoNiaoScript->Reset();

	else if (auto jiangshi = m_pOwner->GetScript<JiangshiBehavior>())
		jiangshi->Reset();

	m_pOwner->transform.SetWorldRotation({ 0, 0, 0 });
	m_pOwner->SetActive(false);

	Agent* agent = dynamic_cast<Agent*>(m_pOwner);
	WaveManager::GetInstance()->EnnemyKilled(agent);
	agent->ReleaseAllNodes();

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
		if (animDeathProgress == 0.f)
		{
			switch (m_pOwner->GetUniqueTag({ Tag::TMogwai, Tag::TJiangshi, Tag::TGuHuoNiao }))
			{
			case Tag::TMogwai:
				AudioUse::Play("mogwai_death");
				break;
			case Tag::TJiangshi:
				AudioUse::Play("jiangshi_death");
				break;
			case Tag::TGuHuoNiao:
				AudioUse::Play("harpy_death");
				break;

			default:
				AudioUse::Play("vineboom");
			}
		}

		if (animDeathProgress < animDeathDuration)
		{
			animDeathProgress += GameManager::DeltaTime();
			HandleDeathAnimation();
		}
		else
		{
			animDeathTrigger = false;
			HandleDeath();
		}

		return;
	}

	if (animHitTrigger)
	{
		if (animHitProgress < animHitDuration)
		{
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
