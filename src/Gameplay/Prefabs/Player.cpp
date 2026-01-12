#include "Player.h"
#include "../SceneManager.h"
#include "Scripts/PlayerBehavior.hpp"
#include "EntityWrapper.h"
#include "../Scripts/HealthBehavior.hpp"
#include "ScoreManager.h"

void Player::Create()
{
	m_pGameObject = &EntityWrapper::Create();
	m_pGameObject->SetProperties("Player", { Tag::TPlayer }, { 10, 2, 0 });

	m_pGameObject->AddPhysics(10, 1, 0);
	m_pGameObject->AddMeshRenderer(gce::SHAPES.CYLINDER, "");

	m_pGameObject->AddComponent<BoxCollider>();
	m_pGameObject->AddScript<PlayerBehavior>();

	auto health = m_pGameObject->AddScript<HealthBehavior>();
	health->SetMaxHP(100);

	m_pGameObject->AddChild(*GameManager::GetSceneManager().GetCameraObject());
}

void Player::Reset()
{
	m_pGameObject->SetActive(true);
	m_pGameObject->transform.SetWorldPosition({ 10, 2, 0 });
	m_pGameObject->transform.SetWorldRotation({ 0, 0, 0 });
	auto playerScript = m_pGameObject->GetScript<PlayerBehavior>();
	playerScript->ResetScript();
	playerScript->SetLookingAround(true);

	auto health = m_pGameObject->GetScript<HealthBehavior>();
	health->FillHP();

	m_pGameObject->AddChild(*GameManager::GetSceneManager().GetCameraObject());
}