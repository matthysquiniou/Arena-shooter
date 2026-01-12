#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"

#include "../Prefabs/BulletPool.h"
#include "BulletBehavior.hpp"
#include "../Prefabs/EntityWrapper.h"
#include "WeaponMagazineBehavior.hpp"
#include "../Prefabs/Ammos.h"
#include "../KeyBinds.h"

using namespace gce;

DECLARE_SCRIPT(GunBehavior, ScriptFlag::Start | ScriptFlag::Update)

float unloadSpeed = 0.1f;
float unloadProgress = 0.f;

float reloadTime = 1.f;
float reloadProgressTime = 0.f;

bool isReloading = false;

float totalRecoil = 0.f;
float recoilRecoverFactor = 1.f; // Speed to cancel recoil
float recoilFactor = 0.f; // For the animation

float gettingWeaponTime = 0.5f; // To prevent switching very fast weapon to shoot instantly
float gettingWeaponProgressTime = 0.f;

WeaponMagazineBehavior* pMagazineBehavior = nullptr;
Quaternion defaultRotation;

const bool& IsReadyToUse() const
{
	return gettingWeaponProgressTime >= gettingWeaponTime;
}
void OnLeaveWeapon() // When this weapon will be changed
{
	isReloading = false;
	reloadProgressTime = 0.f;
	m_pOwner->transform.SetLocalRotation(defaultRotation);
	unloadProgress = unloadSpeed;
	totalRecoil = 0.f;
}
void OnReceiveWeapon() // When this weapon will be the new current one
{
	gettingWeaponProgressTime = 0.f;
}

// IN CLASS METHODS, DO NOT USE SOMEWHERE ELSE
void HandleGettingWeaponAnimation(float dt, MeshRenderer* pMesh)
{
	float ratio = gettingWeaponProgressTime / gettingWeaponTime;
	float valueRatio = (gce::PI / 4) * (1 - ratio);

	Quaternion rotationAnim = {};
	rotationAnim.SetRotationEuler({ valueRatio, 0, 0 });

	m_pOwner->transform.SetLocalRotation(defaultRotation * rotationAnim);
	pMesh->pMaterial->useTextureAlbedo = 0;

	gettingWeaponProgressTime += dt;
}
void HandleReloadingAnimation(float dt)
{
	int turns = 2;

	float value = (2.f * gce::PI / reloadTime) * turns;

	if (reloadProgressTime == 0.f)
	{
		
	}

	m_pOwner->transform.LocalRotate({ value * dt, 0, 0 });
}
void HandleRecoilAnimation(float dt)
{
	if (isReloading)
		return;

	if (totalRecoil > 0)
	{
		float recoverStep = recoilRecoverFactor * recoilFactor * dt;

		if (recoverStep > totalRecoil)
			recoverStep = totalRecoil;

		m_pOwner->transform.LocalRotate({ -recoverStep, 0, 0 });

		totalRecoil -= recoverStep;
	}
	else
	{
		m_pOwner->transform.SetLocalRotation(defaultRotation);
	}
}
void HandleEmptyAnimation(MeshRenderer* pMesh)
{
	if (pMagazineBehavior->IsWeaponHasAmmo() == false)
	{
		if (GetButtonDown(Mouse::LEFT))
		{
			AudioUse::Play("out_of_ammo");
		}

		unloadSpeed = unloadSpeed;
		pMesh->pMaterial->useTextureAlbedo = 0;
	}
	else
	{
		pMesh->pMaterial->useTextureAlbedo = 1;
	}
}
///////////////////////////////////////////////

void SetWeaponProperties(BulletBehavior* script, EntityWrapper& bullet, float bulletSpeed, float bulletLifeTime, gce::Vector3f32 bulletScale, int bulletDamage, float _recoilFactor, float _recoilRecoverFactor)
{
	script->speed = bulletSpeed;
	script->lifeTime = bulletLifeTime;
	script->damage = bulletDamage;
	bullet.transform.WorldScale(bulletScale);
	recoilFactor = _recoilFactor;
	recoilRecoverFactor = _recoilRecoverFactor;
}

void SetAmmoManagerScript(WeaponMagazineBehavior* script) { pMagazineBehavior = script; }
void SetUnloadSpeed(float speed)
{
	unloadSpeed = std::abs(speed);
	unloadProgress = unloadSpeed;
}
void SetReloadTime(float newTime)
{
	reloadTime = std::abs(newTime);
}

void TriggerReload()
{
	if (pMagazineBehavior == nullptr)
		return;

	if (isReloading == false && pMagazineBehavior->CanReload())
	{
		m_pOwner->transform.SetLocalRotation(defaultRotation);

		totalRecoil = 0.f;
		isReloading = true; 
		reloadProgressTime = 0.f;
	}
}
void Reload(float dt) // After the reloading animation
{
	if (reloadProgressTime < reloadTime)
	{
		reloadProgressTime += dt;
	}
	else
	{
		isReloading = false;
		reloadProgressTime = 0.f;

		Ammos* ammoToDecrease = GameManager::GetSceneManager().GetInventoryManager()->GetAmmos(pMagazineBehavior->typeOfAmmo);

		int amount = pMagazineBehavior->maxCapacity - pMagazineBehavior->ammosLeft;
		int ammoInStock = ammoToDecrease->GetAmount();

		if (ammoInStock > amount)
		{
			pMagazineBehavior->FillWeaponAmmos();
		}
		else
		{
			pMagazineBehavior->ammosLeft += ammoInStock; // Reload the rest
		}

		ammoToDecrease->UseAmmos(amount);
	}
}

void Shoot()
{
	if (pMagazineBehavior == nullptr)
		return;

	//Conditions to shoot
	if (isReloading || pMagazineBehavior->IsWeaponHasAmmo() == false)
		return;

	//Cap the shooting speed
	if (unloadProgress < unloadSpeed)
		return;

	gce::Scene& scene = GameManager::GetScene();

	if (m_pOwner->GetChildren().Empty()) // No Spawn point ?
	{
		return;
	}

	gce::Vector3f32 spawnPoint = m_pOwner->GetChildren()[0]->transform.GetWorldPosition();

	EntityWrapper* pCurrent = BulletPool::Generate();
	
	if (pCurrent == nullptr)
		return;

	pCurrent->transform.SetWorldPosition(spawnPoint);

	auto bulletScript = pCurrent->GetScript<BulletBehavior>();

	switch (m_pOwner->GetUniqueTag({ Tag::TMusket, Tag::TBlunderBuss, Tag::TStarwheel }))
	{
	case Tag::TMusket:
		SetWeaponProperties(bulletScript, *pCurrent, 35.f, 4.f, { 6.f, 6.f, 6.f }, 20, 0.075f, 3.f);
		AudioUse::Play("riffle");
		break;
	case Tag::TBlunderBuss:
		SetWeaponProperties(bulletScript, *pCurrent, 35.f, 1.f, { 8.f, 8.f, 4.f }, 50, 0.30f, 1.5f);
		AudioUse::Play("shotgun");
		break;
	case Tag::TStarwheel:
		SetWeaponProperties(bulletScript, *pCurrent, 35.f, 4.f, { 4.f, 4.f, 6.f }, 15, 0.04f, 4.f);
		AudioUse::Play("gun");
		break;
	}

	gce::GameObject* pCam = GameManager::GetSceneManager().GetCameraObject();

	gce::Vector3f32 camPos = pCam->transform.GetWorldPosition();
	gce::Vector3f32 camForward = pCam->transform.GetWorldForward();

	gce::Vector3f32 targetPoint = camPos + camForward * 1000.0f;
	gce::Vector3f32 holePos = m_pOwner->GetChildren()[0]->transform.GetWorldPosition();
	gce::Vector3f32 bulletDir = (targetPoint - holePos).SelfNormalize();

	bulletDir.y += totalRecoil;

	bulletScript->dir = bulletDir;
	bulletScript->defaultDir = bulletDir;

	if (totalRecoil < gce::PI / 8)
	{
		totalRecoil += recoilFactor;
		m_pOwner->transform.LocalRotate({ recoilFactor, 0, 0 }); //ANIM
	}

	unloadProgress = 0.f;
	pMagazineBehavior->UseWeaponAmmo();
}

void Start()
{
	m_pOwner->transform.SetLocalRotation(defaultRotation);
}

void Update()
{
	defaultRotation.SetIdentity();
	defaultRotation.SetRotationEuler({ 0, gce::PI, 0 });

	float dt = GameManager::DeltaTime();
	MeshRenderer* pMesh = m_pOwner->GetComponent<MeshRenderer>();

	if (pMesh == nullptr)
		return;

	if (IsReadyToUse() == false)
	{
		HandleGettingWeaponAnimation(dt, pMesh);
		return;
	}

	if (isReloading == false)
	{
		HandleEmptyAnimation(pMesh);

		if (unloadProgress < unloadSpeed)
			unloadProgress += dt;

		HandleRecoilAnimation(dt);
	}
	else
	{
		HandleReloadingAnimation(dt);
		Reload(dt);
	}
}

END_SCRIPT