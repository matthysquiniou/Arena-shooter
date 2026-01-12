#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"
#include "../Prefabs/InventoryManager.h"
#include "../Prefabs/BulletPool.h"
#include "../Scripts/HealthBehavior.hpp"
#include "../Scripts/EnemyHpBehavior.hpp"
#include "../Scripts/DragonHpBehavior.hpp"

#include <limits>
#include "Scripts/DestructibleBehavior.hpp"

using namespace gce;

DECLARE_SCRIPT(BulletBehavior, ScriptFlag::Start | ScriptFlag::Update | ScriptFlag::Destroy)

//Members
float maxLifeTime = 3.f;
float lifeTime = maxLifeTime;
float speed = 0.f;
int damage = 1;
bool headseeker = false;

gce::Vector3f32 defaultDir = {};
gce::Vector3f32 dir = {};

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

	for (gce::GameObject* go : GameManager::GetSceneManager().GetAllGameObjects({ Tag::TEnemy }))
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

	for (gce::GameObject* go : GameManager::GetSceneManager().GetAllGameObjects({ Tag::TMapObject }))
	{
		if (!go || !go->IsActive() || go->HasTags({Tag::TDestructible}))
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

	for (gce::GameObject* go : GameManager::GetSceneManager().GetAllGameObjects({ Tag::TDestructible }))
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

void ActiveHeadSeeker()
{
	headseeker = true;
}

void Start()
{
}

void Reset()
{
	speed = 0.f;
	damage = 1;
	lifeTime = maxLifeTime;
	defaultDir = {};
	dir = {};
	headseeker = false;

	m_pOwner->GetComponent<MeshRenderer>()->SetActive(false);
	BulletPool::Desactive(m_pOwner);
}

gce::GameObject* GetNearestEnemy()
{
	auto enemies = GameManager::GetSceneManager().GetAllGameObjects({ Tag::TEnemy });
	float minSqrDistance = std::numeric_limits<float>::infinity();

	gce::GameObject* pNearestEnemy = nullptr;
	gce::Vector3f32 bulletPos = m_pOwner->transform.GetWorldPosition();

	float maxDistanceDetection = 10.f;

	gce::GameObject* pPlayer = GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer });

	if (pPlayer->IsActive() == false)
		return nullptr;

	gce::Vector3f32 playerPos = pPlayer->transform.GetWorldPosition();
	gce::Vector3f32 playerForward = pPlayer->transform.GetWorldForward();

	for (gce::GameObject* pCurrent : enemies)
	{
		if (pCurrent->IsActive() == false)
			continue;

		gce::Vector3f32 enemyPos = pCurrent->transform.GetWorldPosition();

		gce::Vector3f32 toEnemy = enemyPos - playerPos;
		toEnemy.SelfNormalize();
		float dot = playerForward.DotProduct(toEnemy);

		if (dot <= 0.0f)
			continue; // Enemy behind

		float sqrDist = (enemyPos.x - bulletPos.x) * (enemyPos.x - bulletPos.x) + (enemyPos.y - bulletPos.y) * (enemyPos.y - bulletPos.y) + (enemyPos.z - bulletPos.z) * (enemyPos.z - bulletPos.z);

		if (sqrDist < minSqrDistance && sqrDist < maxDistanceDetection * maxDistanceDetection)
		{
			minSqrDistance = sqrDist;
			pNearestEnemy = pCurrent;
		}
	}

	return pNearestEnemy;
}

void SeekEnemy(gce::GameObject* pEnemy) // Change the direction
{
	if (pEnemy == nullptr)
	{
		dir = defaultDir;
		return;
	}

	gce::Vector3f32 bulletPos = m_pOwner->transform.GetWorldPosition();
	gce::Vector3f32 enemyPos = pEnemy->transform.GetWorldPosition();

	MeshRenderer* pMesh = pEnemy->GetComponent<MeshRenderer>();

	if (pMesh == nullptr)
		return;

	enemyPos.y += (pMesh->pGeometry->min.y + pMesh->pGeometry->max.y) * 0.5f;

	dir = enemyPos - bulletPos;
	dir.SelfNormalize();
}

void Update()
{
	float dt = GameManager::DeltaTime();

	if (lifeTime < 0)
	{
		Reset();
	}
	else
	{
		if (headseeker == true)
		{
			SeekEnemy(GetNearestEnemy());
			headseeker = false;
		}

		lifeTime -= dt;

		float yaw = atan2(dir.x, dir.z);
		float pitch = atan2(-dir.y, sqrt(dir.x * dir.x + dir.z * dir.z));

		gce::Quaternion rot = {};
		rot.SetRotationEuler(pitch, yaw, 0.0f);

		Quaternion flip = {};
		flip.SetRotationEuler({ 0, gce::PI, 0 });

		m_pOwner->transform.SetWorldRotation(flip * rot);
		m_pOwner->transform.WorldTranslate(dir * speed * dt);

		m_pOwner->GetComponent<MeshRenderer>()->SetActive(true);
	}

	if (gce::GameObject* pCollided = CheckCollision())
	{
		if (pCollided->HasTags({ Tag::TEnemy }))
		{
			if (auto animation = pCollided->GetScript<EnemyHpBehavior>())
			{
				animation->TriggerHitAnim();
			}

			if (auto animation = pCollided->GetScript<DragonHpBehavior>())
			{
				animation->TriggerHitAnim();
			}

			if (auto health = pCollided->GetScript<HealthBehavior>())
			{
				health->TakeDamage(damage);
			}
		}

		if (pCollided->HasTags({ Tag::TDestructible }))
		{
			DestructibleBehavior* db = pCollided->GetScript<DestructibleBehavior>();
			db->GetHit();
		}

		Reset();
	}
}

void Destroy()
{
}

END_SCRIPT
