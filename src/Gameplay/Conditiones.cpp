#include "Conditiones.h"

CanMeleeAttack::CanMeleeAttack(Agent* agent, float meleeRange)
{
	m_agent = agent;
	m_meleeRange = meleeRange;
}

bool CanMeleeAttack::Get()
{
	return m_meleeRange == m_agent->GetStopRange() && m_agent->IsTargetInRange();
}

CanDistanceAttack::CanDistanceAttack(Agent* agent, int* ammo, float distanceRange)
{
	m_agent = agent;
	m_ammo = ammo;
	m_distanceRange = distanceRange;
}

bool CanDistanceAttack::Get()
{
	return m_distanceRange == m_agent->GetStopRange() && (*m_ammo) > 0 && m_agent->IsTargetInRange() && !m_agent->IsTargetBehindObstacle();
}

CantAttack::CantAttack(Agent* agent, int* ammo, bool* attacking, float distanceRange)
{
	m_agent = agent;
	m_ammo = ammo;
	m_distanceRange = distanceRange;
	m_attacking = attacking;
}

bool CantAttack::Get()
{
	return !m_agent->IsTargetInRange() || (m_agent->GetStopRange() == m_distanceRange && (*m_ammo) == 0);
}

IsPatternFinished::IsPatternFinished(bool* isPatternFinished)
{
	m_isPatternFinished = isPatternFinished;
}

bool IsPatternFinished::Get()
{
	return *m_isPatternFinished;
}

IsRotationFinished::IsRotationFinished(bool* isRotationFinished)
{
	m_isRotationFinished = isRotationFinished;
}

bool IsRotationFinished::Get()
{
	return *m_isRotationFinished;
}

IsAttackLoaded::IsAttackLoaded(bool* isAttackLoaded)
{
	m_isAttackLoaded = isAttackLoaded;
}

bool IsAttackLoaded::Get()
{
	return *m_isAttackLoaded;
}
