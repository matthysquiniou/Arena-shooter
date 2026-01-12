#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include "Prefabs/ArenaCamera.h"
#include "Components.h"
#include "EnemyScriptHelper.h"
#include "Scripts/EnemyProjectileBehavior.hpp"
using namespace gce;

DECLARE_SCRIPT(DragonBehavior, ScriptFlag::Start | ScriptFlag::Update | ScriptFlag::CollisionEnter)

bool m_isRotationFinished;
bool m_isAttackLoaded;
bool m_isAttackFinished;

float m_speed;
float m_rotationSpeed;

float m_idleRange;
int m_circlePoints;
float m_timeBetweenPaterns;

std::vector<gce::GameObject*> m_fireBalls;
std::unordered_set<gce::GameObject*> m_launchedFireBalls;
std::vector<gce::GameObject*> m_laser;

gce::GameObject* m_player;
gce::Vector3f32 m_targetPos;

StateMachinee m_stateMachine;

IsPatternFinished m_isPatternFinishedC;
IsRotationFinished m_isRotationFinishedC;
IsAttackLoaded m_isAttackLoadedC;

RotateTowardTarget m_rotateTowardTarget;
RotateAround m_rotateAround;
BossLoadAttack m_bossLoadAttack;
BossAttack m_bossAttack;

BossPattern m_bossPattern = BossPattern(m_pOwner, m_player);

void Start()
{
	m_isRotationFinished = false;
	m_isAttackLoaded = false;
	m_isAttackFinished = false;

	m_player = GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer });

	m_speed = 10.f;
	m_rotationSpeed = 3.f;

	m_idleRange = 20.f;
	m_circlePoints = 360;
	m_timeBetweenPaterns = 8.f;

	m_isPatternFinishedC = IsPatternFinished(&m_isAttackFinished);
	m_isRotationFinishedC = IsRotationFinished(&m_isRotationFinished);
	m_isAttackLoadedC = IsAttackLoaded(&m_isAttackLoaded);

	m_rotateTowardTarget = RotateTowardTarget(m_pOwner, &m_targetPos, &m_isRotationFinished, m_rotationSpeed);
	m_rotateAround = RotateAround(m_pOwner, m_player, &m_targetPos, &m_isRotationFinished, m_circlePoints, m_speed, m_idleRange);
	m_bossLoadAttack = BossLoadAttack(&m_isAttackLoaded, m_timeBetweenPaterns, m_player, &m_targetPos);

	m_bossPattern.m_boss = m_pOwner;
	m_bossPattern.m_player = m_player;
	m_bossAttack = BossAttack(&m_bossPattern, &m_isAttackFinished); 

	m_stateMachine.AddStateAction(State::BOSS_IDLE, m_rotateAround);
	m_stateMachine.AddStateAction(State::BOSS_IDLE, m_rotateTowardTarget);
	m_stateMachine.AddStateAction(State::BOSS_IDLE, m_bossLoadAttack);
	m_stateMachine.AddStateCondition(State::BOSS_IDLE, State::BOSS_PREPARE_TO_ATTACK, m_isAttackLoadedC);

	m_stateMachine.AddStateAction(State::BOSS_PREPARE_TO_ATTACK, m_rotateTowardTarget);
	m_stateMachine.AddStateCondition(State::BOSS_PREPARE_TO_ATTACK, State::BOSS_ATTACK, m_isRotationFinishedC);

	m_stateMachine.AddStateAction(State::BOSS_ATTACK, m_bossAttack);
	m_stateMachine.AddStateCondition(State::BOSS_ATTACK, State::BOSS_IDLE, m_isPatternFinishedC);

	m_stateMachine.SetState(State::BOSS_IDLE);
}

void Update()
{
	m_stateMachine.Update();
}
END_SCRIPT