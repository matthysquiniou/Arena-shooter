#pragma once

#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include <functional>
#include "Components.h"
#include "Scripts/HealthBehavior.hpp"

using namespace gce;

DECLARE_SCRIPT(EnemyProjectileBehavior, ScriptFlag::Start | ScriptFlag::Update)

Agent* pThrower = nullptr;

void SetThrower(Agent* pAgent) { pThrower = pAgent; }

float m_baseLifeTime = 3.f;
float m_lifeTime = 0.f;
float m_speed = 75.f;
int m_damage = 1;
float m_ySpawnOffset = 2.f;
std::function<void(float, gce::Vector3f32)> m_animation = [](float deltaTime, gce::Vector3f32 dir){};

gce::Vector3f32 m_dir = { 0, 0, 0 };

gce::GameObject* CheckCollision()
{
	auto& transform1 = m_pOwner->transform;

	gce::Vector3f32 pos1 = transform1.GetWorldPosition();
	gce::Vector3f32 scale1 = transform1.GetWorldScale();

	MeshRenderer* pMesh1 = m_pOwner->GetComponent<MeshRenderer>();
	if (!pMesh1)
		return nullptr;

	auto& geo1 = pMesh1->pGeometry;
	gce::Vector3f32 min1 = pos1 + geo1->min * scale1;
	gce::Vector3f32 max1 = pos1 + geo1->max * scale1;

	for (gce::GameObject* go : GameManager::GetSceneManager().GetAllGameObjects({ Tag::TPlayer }))
	{
		if (!go || !go->IsActive())
			continue;

		auto& transform2 = go->transform;

		gce::Vector3f32 pos2 = transform2.GetWorldPosition();
		gce::Vector3f32 scale2 = transform2.GetWorldScale();

		MeshRenderer* pMesh2 = go->GetComponent<MeshRenderer>();
		if (!pMesh2)
			continue;

		auto& geo2 = pMesh2->pGeometry;

		gce::Vector3f32 min2 = pos2 + geo2->min * scale2;
		gce::Vector3f32 max2 = pos2 + geo2->max * scale2;

		// Test AABB
		bool collision =
			(min1.x <= max2.x && max1.x >= min2.x) &&
			(min1.y <= max2.y && max1.y >= min2.y) &&
			(min1.z <= max2.z && max1.z >= min2.z);

		if (collision)
			return go;
	}

	return nullptr;
}

void Update()
{
	float dt = GameManager::DeltaTime();

	if (m_lifeTime < 0.f)
	{
		m_pOwner->SetActive(false);
	}
	else
	{
		m_lifeTime -= dt;

		m_pOwner->transform.WorldTranslate(m_dir * m_speed * dt);
		m_animation(dt, m_dir);
	}

	if (gce::GameObject* pCollided = CheckCollision())
	{
		if (pCollided->HasTags({ Tag::TPlayer }))
		{
			if (pThrower)
			{
				switch (pThrower->GetUniqueTag({ Tag::TMogwai, Tag::TJiangshi, Tag::TGuHuoNiao }))
				{
				case Tag::TMogwai:
					AudioUse::Play("knife_hit");
					break;
				case Tag::TJiangshi:
					break;
				case Tag::TGuHuoNiao:
					break;
				default:
					break;
				}
			}
		
			pCollided->GetScript<HealthBehavior>()->TakeDamage(m_damage);
		}
		
		m_pOwner->SetActive(false);
	}
}

void Shoot(gce::Vector3f32 from, gce::Vector3f32 toward)
{
	from.y += m_ySpawnOffset;
	m_pOwner->transform.SetWorldPosition(from);
	m_dir = (toward - from).Normalize();

	float yaw = atan2(m_dir.x, m_dir.z);
	float pitch = atan2(
		-m_dir.y,
		sqrt(m_dir.x * m_dir.x + m_dir.z * m_dir.z)
	);

	gce::Quaternion rot = {};
	rot.SetRotationEuler(pitch, yaw, 0.0f);

	m_pOwner->transform.SetWorldRotation(rot);

	m_lifeTime = m_baseLifeTime;
	m_pOwner->SetActive(true);
}

END_SCRIPT
