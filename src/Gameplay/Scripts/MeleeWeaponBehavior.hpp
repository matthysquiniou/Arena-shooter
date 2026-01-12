#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"
#include "../Prefabs/EntityWrapper.h"
#include "../KeyBinds.h"

using namespace gce;

DECLARE_SCRIPT(MeleeWeaponBehavior, ScriptFlag::Start | ScriptFlag::Update)

bool isHitting = false;
float unloadSpeed = 0.5f;
float unloadProgress = unloadSpeed;

float gettingWeaponTime = 0.5f; // To prevent switching very fast weapon to shoot instantly
float gettingWeaponProgressTime = 0.f;

Quaternion defaultRotation;
int damage = 0;

bool hasCollided = false;

void SetDefaultRotation(Quaternion rot) { defaultRotation = rot; }

gce::GameObject* CheckCollision()
{
	if (hasCollided == true)
		return nullptr;

	auto hitbox = m_pOwner->GetChildren()[0];

	if (hitbox == nullptr)
		return nullptr;

	auto& transform1 = hitbox->transform;

	gce::Vector3f32 pos1 = transform1.GetWorldPosition();
	gce::Vector3f32 scale1 = transform1.GetWorldScale();

	MeshRenderer* pMesh1 = hitbox->GetComponent<MeshRenderer>();
	if (!pMesh1)
		return nullptr;

	auto& geo1 = pMesh1->pGeometry;
	gce::Vector3f32 min1 = pos1 + geo1->min * scale1;
	gce::Vector3f32 max1 = pos1 + geo1->max * scale1;

	for (gce::GameObject* go : GameManager::GetSceneManager().GetAllGameObjects({ Tag::TEnemy}))
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
			hasCollided = true;
			return go;
		}
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

const bool& IsReadyToUse() const
{
	return gettingWeaponProgressTime >= gettingWeaponTime;
}
void OnLeaveWeapon() // When this weapon will be changed
{
	m_pOwner->transform.SetLocalRotation(defaultRotation);
	unloadProgress = unloadSpeed;
}
void OnReceiveWeapon() // When this weapon will be the new current one
{
	gettingWeaponProgressTime = 0.f;
}

// IN CLASS METHODS, DO NOT USE SOMEWHERE ELSE
void HandleGettingWeaponAnimation(float dt, MeshRenderer* pMesh)
{
	gce::Vector3f32 dir = m_pOwner->transform.GetLocalForward();

	float ratio = gettingWeaponProgressTime / gettingWeaponTime;
	float valueRatio = (gce::PI / 4) * (1 - ratio);

	Quaternion rotation = {};
	rotation.SetRotationAxis(dir, valueRatio);

	m_pOwner->transform.SetLocalRotation(rotation * defaultRotation);

	gettingWeaponProgressTime += dt;
}

void HandleHitAnimation(float dt)
{
	gce::Vector3f32 dir = m_pOwner->transform.GetLocalForward();

	Quaternion rotation = {};

	float ratio = unloadProgress / unloadSpeed;
	float swingRatio = std::sin(ratio * gce::PI);
	swingRatio *= swingRatio;

	rotation.SetRotationAxis(dir, -gce::PI / 2 * swingRatio);

	m_pOwner->transform.SetLocalRotation(rotation * defaultRotation);
}
///////////////////////////////////////////////

void SetMeleeWeaponProperties(int _damage, float hittingDuration)
{
	damage = _damage;
	unloadSpeed = hittingDuration;
	unloadProgress = unloadSpeed;
}

void SetUnloadSpeed(float speed)
{
	unloadSpeed = std::abs(speed);
	unloadProgress = unloadSpeed;
}

void Hit()
{
	if (isHitting || unloadProgress < unloadSpeed)
		return;

	isHitting = true;
	unloadProgress = 0.f;
}

void Start()
{
}

void Update()
{
	float dt = GameManager::DeltaTime();
	MeshRenderer* pMesh = m_pOwner->GetComponent<MeshRenderer>();

	if (pMesh == nullptr)
		return;

	if (IsReadyToUse() == false)
	{
		HandleGettingWeaponAnimation(dt, pMesh);
		pMesh->pMaterial->useTextureAlbedo = 0;
		return;
	}

	pMesh->pMaterial->useTextureAlbedo = 1;

	if (isHitting)
	{
		if(unloadProgress == 0.f)
			AudioUse::Play("spear_attack");

		if (unloadProgress < unloadSpeed)
		{
			unloadProgress += dt;
			HandleHitAnimation(dt);
			
			if (gce::GameObject* pCollided = CheckCollision())
			{
				if (auto animation = pCollided->GetScript<EnemyHpBehavior>())
				{
					animation->TriggerHitAnim();
				}

				if (auto health = pCollided->GetScript<HealthBehavior>())
				{
					AudioUse::Play("spear_hit");
					health->TakeDamage(damage);
				}
			}
			return;
		}
		else
		{
			hasCollided = false;
			isHitting = false;
		}
	}
	else
	{
		m_pOwner->transform.SetLocalRotation(defaultRotation);
	}
}

END_SCRIPT