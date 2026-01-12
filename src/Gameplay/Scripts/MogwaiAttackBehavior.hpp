#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include "Prefabs/ArenaCamera.h"
#include "Components.h"

using namespace gce;

DECLARE_SCRIPT(MogwaiAttackBehavior, ScriptFlag::Start | ScriptFlag::Update | ScriptFlag::CollisionEnter)

void Start()
{
	gce::Vector3f32 pos = m_pOwner->GetParent()->transform.GetWorldPosition();
	pos.x += 7.3f;
	pos.y += 0.4f;
	m_pOwner->transform.SetWorldPosition(pos);
	m_pOwner->transform.SetWorldScale({ 0.07f,0.50f ,0.07f });
}
void Update()
{
	float dt = GameManager::DeltaTime();
	m_pOwner->transform.WorldTranslate({ -1 * dt, 0, 0 });
}
void CollisionEnter()
{

}
END_SCRIPT