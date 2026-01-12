#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include "Prefabs/ArenaCamera.h"
#include "Components.h"
#include "../SceneManager.h"
#include "../Prefabs/InventoryManager.h"
#include "KeyBinds.h"

#include "GunBehavior.hpp"
#include "MeleeWeaponBehavior.hpp"
#include "HealthBehavior.hpp"


using namespace gce;

DECLARE_SCRIPT(PlayerBehavior, ScriptFlag::Start | ScriptFlag::Update | ScriptFlag::CollisionEnter | ScriptFlag::CollisionStay | ScriptFlag::CollisionExit | ScriptFlag::Destroy)

//Members /////////////////////////
PhysicComponent* pPhysic = nullptr;
GameObject* pWeapon = nullptr;

float playerSpeed = 5.f;
gce::Vector3f32 finalDir = {};

//Jump
bool isJumping = false;
int jumpsAmount = 0;
int maxJumpsAmount = 1;

//Dash
bool isDashing = false;
float maxDashAmount = 2;
float dashAmount = 0.f;
float dashTotalReloadTime = 6.f;
float dashProgressReloadTime = 0.f;

float dashDuration = 0.1f;
float dashProgressDuration = 0.f;

//Camera
float sensitivity = 0.002f;
gce::Vector2i32 middleScreen = { (int)((float)(WINDOW_WIDTH) * 0.5f), (int)((float)(WINDOW_HEIGHT) * 0.5f) };
float totalPitchRotation = 0.f;
bool stopLookAround = false;

bool isDead = false;
float dyingAnimationDuration = 1.5f;
float dyingAnimationProgressDuration = 0.f;

Quaternion currentRotation = {};

void ResetScript()
{
	isDashing = false;
	dashProgressReloadTime = 0.f;
	dashProgressDuration = 0.f;
	totalPitchRotation = 0.f;
	dyingAnimationProgressDuration = 0.f;
	isDead = false;
	isDashing = false;
}

void SetLookingAround(bool state)
{
	if (isDead)
		return;

	stopLookAround = !state;
}

//Functions
void HandleHealth()
{
	if (HealthBehavior* pScript = m_pOwner->GetScript<HealthBehavior>())
	{
		isDead = !pScript->IsAlive();
	}

	return;
}

void DeathAnimation()
{
	if (dyingAnimationProgressDuration < dyingAnimationDuration)
	{
		pWeapon->SetActive(false);

		float ratio = dyingAnimationProgressDuration / dyingAnimationDuration;
		ratio = std::clamp(ratio, 0.f, 1.f);
		float easing = pow(ratio, 3.f); // Acceleration animation

		Quaternion rotation = {};
		rotation.SetRotationAxis(m_pOwner->transform.GetWorldRight(), gce::PI / 2 * easing);

		m_pOwner->transform.SetWorldRotation(currentRotation * rotation);

		dyingAnimationProgressDuration += GameManager::DeltaTime();
	}
	else
	{
		isDead = false;
		dyingAnimationProgressDuration = 0.f;
		GameManager::GetSceneManager().ChangeScene(SceneType::GameOverScene);
	}
}

void LookAround()
{
	if (m_pOwner->GetChildren().Empty() || stopLookAround == true)
	{
		ShowMouseCursor();
		return;
	}

	gce::GameObject* pCamera = m_pOwner->GetChildren()[0];
	// Delta Mouse Calcul
	HideMouseCursor();

	POINT currentMousePos;
	GetCursorPos(&currentMousePos);
	gce::Vector2i32 mouseDelta = { (int)(currentMousePos.x - middleScreen.x), (int)(currentMousePos.y - middleScreen.y) };

	float yaw = mouseDelta.x * sensitivity;
	float pitch = mouseDelta.y * sensitivity;

	//Don't allow to look the world upside down (e.g more than 90 degrees toward up)
	totalPitchRotation += pitch;
	totalPitchRotation = std::clamp(totalPitchRotation, -gce::PI / 2, gce::PI / 2);

	// Set Rotation for Player
	m_pOwner->transform.WorldRotate({ 0.f, yaw, 0.f });

	Quaternion pitchQ = Quaternion::RotationEuler({ totalPitchRotation, 0.f, 0.f });

	pCamera->transform.SetLocalRotation(pitchQ);

	SetCursorPos(middleScreen.x, middleScreen.y);

	currentRotation = m_pOwner->transform.GetWorldRotation();
}

void BasicControls() // Move + Jump
{
	if (pPhysic == nullptr)
		return;

	if (isDashing == false)
	{
		gce::Vector3f32 velocity = pPhysic->GetVelocity();
		pPhysic->SetVelocity({ 0, velocity.y, 0 });
	}

	float dt = GameManager::DeltaTime();

	gce::Vector3f32 dir = {};

	if (GetKey(KeyBinds::GetKeyBind(KeyAction::MoveLeft)))
		dir.x -= 1;
	if (GetKey(KeyBinds::GetKeyBind(KeyAction::MoveRight)))
		dir.x += 1;
	if (GetKey(KeyBinds::GetKeyBind(KeyAction::MoveForward)))
		dir.z += 1;
	if (GetKey(KeyBinds::GetKeyBind(KeyAction::MoveBackward)))
		dir.z -= 1;

	dir.SelfNormalize();

	finalDir = m_pOwner->transform.GetWorldForward() * dir.z + m_pOwner->transform.GetWorldRight() * dir.x; // Redirect Direction By Rotation

	if (GetKeyDown(KeyBinds::GetKeyBind(KeyAction::Jump)))
	{
		if (jumpsAmount > 0)
		{
			isJumping = true;
			jumpsAmount--;

			gce::Force f;

			f.direction = { 0, 1, 0 };
			f.norm = 3000;
			f.useApplicationPoint = true;
			f.relativeApplicationPoint = { 0, 0, 0 };

			pPhysic->AddForce(f);
		}
	}

	m_pOwner->transform.WorldTranslate((finalDir * playerSpeed * dt));
}
void HandleDash()
{
	float dt = GameManager::DeltaTime();

	if (isDashing)
	{
		if (dashProgressDuration < dashDuration)
		{
			dashProgressDuration += dt;

			gce::Force f;
			f.direction = finalDir;
			f.norm = 22750;
			f.useApplicationPoint = true;
			f.relativeApplicationPoint = { 0, 0, 0 };

			pPhysic->AddForce(f);
		}
		else
		{
			dashProgressDuration = 0.f;
			isDashing = false;
		}

		return;
	}

	if (dashProgressReloadTime < dashTotalReloadTime)
	{
		dashProgressReloadTime += dt;
	}

	dashProgressReloadTime = std::clamp(dashProgressReloadTime, 0.f, dashTotalReloadTime);
	dashAmount = (int)(maxDashAmount * dashProgressReloadTime / dashTotalReloadTime);

	if (dashAmount > 0)
	{
		if (GetKeyDown(KeyBinds::GetKeyBind(KeyAction::Dash)))
		{
			pPhysic->SetVelocity({ 0, 0, 0 });

			dashProgressReloadTime -= dashTotalReloadTime / (float)maxDashAmount;
			isDashing = true;

			AudioUse::Play("dash");
		}
	}
}
void HandleWeapon()
{
	if (pWeapon != nullptr)
	{
		if (auto gunScript = pWeapon->GetScript<GunBehavior>())
		{
			if (gunScript->IsReadyToUse())
			{
				if (GetButton(Mouse::LEFT))
					gunScript->Shoot();

				if (GetKeyDown(KeyBinds::GetKeyBind(KeyAction::Reload)))
					gunScript->TriggerReload();
			}
		}

		if (auto meleeScript = pWeapon->GetScript<MeleeWeaponBehavior>())
		{
			if (meleeScript->IsReadyToUse())
			{
				if (GetButton(Mouse::LEFT))
					meleeScript->Hit();
			}
		}
	}

	if (GetKeyDown(KeyBinds::GetKeyBind(KeyAction::InventorySlot1)))
	{
		GameManager::GetSceneManager().GetInventoryManager()->SetEquipedObjectByIndex(0);
	}
	else if (GetKeyDown(KeyBinds::GetKeyBind(KeyAction::InventorySlot2)))
	{
		GameManager::GetSceneManager().GetInventoryManager()->SetEquipedObjectByIndex(1);
	}
	else if (GetKeyDown(KeyBinds::GetKeyBind(KeyAction::InventorySlot3)))
	{
		GameManager::GetSceneManager().GetInventoryManager()->SetEquipedObjectByIndex(2);
	}
	else if (GetKeyDown(KeyBinds::GetKeyBind(KeyAction::InventorySlot4)))
	{
		GameManager::GetSceneManager().GetInventoryManager()->SetEquipedObjectByIndex(3);
	}
}

void HandleInput()
{
	if (pPhysic == nullptr)
		return;

	BasicControls();
	HandleDash();
	HandleWeapon();
}

void SetCurrentWeapon(GameObject* go) { pWeapon = go; }

void Start()
{
	pPhysic = m_pOwner->GetComponent<PhysicComponent>();
}

void Update()
{
	if (isDead)
	{
		DeathAnimation();
		return;
	}

	SetCurrentWeapon(GameManager::GetSceneManager().GetInventoryManager()->GetCurrentEquipedObject());
	HandleInput();
	HandleHealth();
	LookAround();
}

void Destroy()
{
}

void CollisionStay(GameObject* other)
{
	if (true) // TO DO Set Tag for the loaded Scene
	{ 
		if (isJumping == false)
		{
			jumpsAmount = maxJumpsAmount;
		}
	}
}

void CollisionEnter(GameObject* other)
{
	if (true) // TO DO Set Tag for the loaded Scene
	{
		if (isJumping)
		{
			isJumping = false;
		}
		else
		{
			jumpsAmount = maxJumpsAmount;
		}
	}
}

void CollisionExit(GameObject* other) override
{
	if (isJumping == false)
		jumpsAmount = 0;
}

END_SCRIPT
