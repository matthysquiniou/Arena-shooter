#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"
#include "Prefabs/InventoryManager.h"
#include "Prefabs/Ammos.h"
#include "CollectibleMunitionManager.h"

using namespace gce;

DECLARE_SCRIPT(CollectibleMunitionBehavior, ScriptFlag::Update | ScriptFlag::Start | ScriptFlag::CollisionEnter)

void Start()
{
	m_pOwner->transform.SetWorldRotation({ gce::PI / 16, 0, 0 });
}

void Update()
{
	float dt = GameManager::DeltaTime();
	float val = m_rotationSpeed * dt;

	m_pOwner->transform.WorldRotate({ 0, val, 0 });
	CheckCollision();
}


void CheckCollision()
{
	auto& transform1 = m_pOwner->transform;

	gce::Vector3f32 pos1 = transform1.GetWorldPosition();
	gce::Vector3f32 scale1 = transform1.GetWorldScale();

	MeshRenderer* pMesh1 = m_pOwner->GetComponent<MeshRenderer>();
	if (!pMesh1)
		return;

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
		{
			InventoryManager* im = gce::GameManager::s_pInstance->GetSceneManager().GetInventoryManager();
			Ammos* ammo = im->GetAmmos(m_ammoTag);
			ammo->EarnAmmos(m_ammoEarn);
			CollectibleMunitionManager::MunitionTook(m_pOwner);

			AudioUse::Play("ammo_pickup");
		}
	}
}

Tag m_ammoTag;
int m_ammoEarn = 10;
float m_rotationSpeed = 2.0f;

END_SCRIPT
