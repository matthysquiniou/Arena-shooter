#include "Actiones.h"
#include "Scripts/EnemyProjectileBehavior.hpp"
#include "Scripts/HealthBehavior.hpp"

ReloadAmmo::ReloadAmmo(int* ammo, int* maxAmmo, float baseReloadAmmo)
{
	m_maxAmmo = maxAmmo;
	m_ammo = ammo;
	m_baseReloadAmmo = baseReloadAmmo;
	m_reloadAmmo = 0.f;
}

void ReloadAmmo::Update(float deltaTime)
{
	if (*m_ammo == *m_maxAmmo)
		return;

	if (m_reloadAmmo >= m_baseReloadAmmo)
	{
		m_reloadAmmo = 0.f;
		(*m_ammo)++;
	}
	else
		m_reloadAmmo += deltaTime;
}

ChangeAgentRange::ChangeAgentRange(Agent* agent, int* ammo, float meleeRange, float distanceRange)
{
	m_ammo = ammo;
	m_agent = agent;
	m_meleeRange = meleeRange;
	m_distanceRange = distanceRange;
}

void ChangeAgentRange::Update(float deltaTime)
{
	if (*m_ammo == 0)
		m_agent->SetStopRange(m_meleeRange);
	else
		m_agent->SetStopRange(m_distanceRange);
}

MeleeAttack::MeleeAttack(Agent* agent, int damage, int hitRange, float attackHitFrame, float baseAttackCooldown, bool* attackLaunched)
{
	m_agent = agent;
	m_hitRange = hitRange;
	m_attackHitFrame = attackHitFrame;
	m_baseAttackCooldown = baseAttackCooldown;
	m_attackCooldown = 0.f;
	m_attackLaunched = attackLaunched;
	m_attackReady = true;
	m_currentAttackFrame = 0.f;
	m_damage = damage;
}


void MeleeAttack::Update(float deltaTime)
{
	if (m_attackReady)
	{
		(*m_attackLaunched) = true;
		m_attackReady = false;
	}

	if ((*m_attackLaunched))
	{
		m_currentAttackFrame += deltaTime;
	}

	if (m_currentAttackFrame >= m_attackHitFrame)
	{
		(*m_attackLaunched) = false;
		m_currentAttackFrame = 0.f;
		if (m_agent->GetDistanceFromTarget() <= m_hitRange)
		{
			switch (m_agent->GetUniqueTag({ Tag::TMogwai, Tag::TJiangshi, Tag::TGuHuoNiao }))
			{
			case Tag::TMogwai:
				AudioUse::Play("mogwai_hitplayer");
				break;
			case Tag::TJiangshi:
				AudioUse::Play("jiangshi_hitplayer");
				break;
			case Tag::TGuHuoNiao:
				AudioUse::Play("harpy_hitplayer");
				break;
			default:
				break;
			}

			GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer })->GetScript<HealthBehavior>()->TakeDamage(m_damage);
		}
	}

	if (!m_attackReady && !(*m_attackLaunched))
	{
		m_attackCooldown += deltaTime;
		if (m_attackCooldown >= m_baseAttackCooldown)
		{
			m_attackReady = true;
			m_attackCooldown = 0.f;
		}
	}
}

void MeleeAttack::Reset()
{
	m_attackReady = true;
	(*m_attackLaunched) = false;
	m_currentAttackFrame = 0.f;
	m_attackCooldown = 0.f;
}

DistanceAttack::DistanceAttack(Agent* agent, float baseAttackCooldown, int* ammo, std::vector<gce::GameObject*>* projectiles)
{
	m_agent = agent;
	m_baseAttackCooldown = baseAttackCooldown;
	m_attackCooldown = 0.f;
	m_ammo = ammo;
	m_attackReady = true;
	m_projectiles = projectiles;
}

void DistanceAttack::Update(float deltaTime)
{
	if (m_attackReady && (*m_ammo) > 0)
	{
		m_attackReady = false;
		if (m_projectiles->empty())
		{
			std::vector<gce::GameObject*> getLauched;
			for (gce::GameObject* proj : m_lauchedProjectiles)
			{
				if (!proj->IsActive())
					getLauched.push_back(proj);
			}

			for (size_t i = 0; i < getLauched.size(); i++)
			{
				gce::GameObject* proj = getLauched[i];
				m_lauchedProjectiles.erase(proj);
				m_projectiles->push_back(proj);
			}
		}

		if (m_projectiles->empty())
			return;

		gce::GameObject* proj = m_projectiles->back();
		EnemyProjectileBehavior* epb = proj->GetScript<EnemyProjectileBehavior>();
		epb->SetThrower(m_agent);
		epb->Shoot(m_agent->transform.GetWorldPosition(), m_agent->GetTarget()->transform.GetWorldPosition());

		m_projectiles->pop_back();
		m_lauchedProjectiles.insert(proj);

		(*m_ammo)--;

		switch (m_agent->GetUniqueTag({ Tag::TMogwai, Tag::TJiangshi, Tag::TGuHuoNiao }))
		{
		case Tag::TMogwai:
			AudioUse::Play("knife_sharpen");
			break;
		case Tag::TJiangshi:
			break;
		case Tag::TGuHuoNiao:
			AudioUse::Play("harpy_attack");
			break;
		default:
			break;
		}
	}

	if (!m_attackReady)
	{
		m_attackCooldown += deltaTime;
		if (m_attackCooldown >= m_baseAttackCooldown)
			Reset();
	}
}

void DistanceAttack::Reset()
{
	m_attackCooldown = 0.f;
	m_attackReady = true;
}

RotateTowardTarget::RotateTowardTarget(gce::GameObject* owner, gce::Vector3f32* target, bool* rotationFinised, float rotationSpeed)
{
	m_owner = owner;
	m_target = target;
	m_rotationFinised = rotationFinised;
	m_rotationSpeed = rotationSpeed;
}

void RotateTowardTarget::Update(float deltaTime)
{
	if (*m_rotationFinised)
		return;

	const gce::Vector3f32 ownerPos = m_owner->transform.GetWorldPosition();

	gce::Vector3f32 forward = m_owner->transform.GetWorldForward();
	forward = -forward;
	forward.y = 0.f;
	if (forward.Norm() < 0.0001f) return;
	forward.SelfNormalize();

	gce::Vector3f32 toTarget = *m_target - ownerPos;
	toTarget.y = 0.f;
	if (toTarget.Norm() < 0.0001f) return;
	toTarget.SelfNormalize();

	float dot = gce::Clamp(forward.DotProduct(toTarget), -1.f, 1.f);
	float angle = acosf(dot);

	float sign = (forward.CrossProduct(toTarget).y >= 0.f) ? 1.f : -1.f;

	float deltaYaw = angle * sign;

	float maxStep = m_rotationSpeed * deltaTime;

	if (fabs(angle) <= maxStep)
	{
		m_owner->transform.WorldRotate({ 0.f, deltaYaw, 0.f });
		*m_rotationFinised = true;
		return;
	}

	m_owner->transform.WorldRotate({ 0.f, sign * maxStep, 0.f });
}


void RotateTowardTarget::Reset()
{
	*m_rotationFinised = false;
}

RotateAround::RotateAround(gce::GameObject* owner, gce::GameObject* target, gce::Vector3f32* translateTarget, bool* isRotationFinished, int circlePoints, float speed, float radius)
{
	m_owner = owner;
	m_target = target;
	m_translateTarget = translateTarget;
	m_isRotationFinished = isRotationFinished;
	m_stepRadian = (2.f * gce::PI)/circlePoints;
	m_speed = speed;
	m_radius = radius;
	m_isTranslateTargetSet = false;
}

void RotateAround::Update(float deltaTime)
{
	if (!m_isTranslateTargetSet)
		ChangeTarget();

	gce::Vector3f32 translate = m_direction * m_speed * deltaTime;
	float32 distance = translate.Norm();
	if (m_distanceToTravel <= distance)
	{
		m_owner->transform.SetWorldPosition(*m_translateTarget);
		m_isTranslateTargetSet = false;
		return;
	}
	m_owner->transform.WorldTranslate(translate);
	m_distanceToTravel -= distance;
}

void RotateAround::Reset()
{
	*m_isRotationFinished = false;
	m_isTranslateTargetSet = false;
}

void RotateAround::ChangeTarget()
{
	m_isTranslateTargetSet = true;
	*m_isRotationFinished = false;

	gce::Vector3f32 currentPos = m_owner->transform.GetWorldPosition();
	gce::Vector3f32 targetPos = m_target->transform.GetWorldPosition();

	float currentRadian = atan2f(currentPos.z - targetPos.z, currentPos.x - targetPos.x);
	float nextRadian = currentRadian + m_stepRadian;

	m_translateTarget->x = targetPos.x + m_radius * cos(nextRadian);
	m_translateTarget->y = currentPos.y;
	m_translateTarget->z = targetPos.z + m_radius * sin(nextRadian);
	gce::Vector3f32 dir = (*m_translateTarget - currentPos);

	m_distanceToTravel = dir.Norm();
	m_direction = dir.Normalize();
}

BossLoadAttack::BossLoadAttack(bool* isAttackLoaded, float attackLoadTime, gce::GameObject* player, gce::Vector3f32* target)
{
	m_isAttackLoaded = isAttackLoaded;
	m_baseAttackLoadTime = attackLoadTime;
	m_attackLoadTime = 0.f;
	m_player = player;
	m_target = target;
}

void BossLoadAttack::Update(float deltaTime)
{
	if (*m_isAttackLoaded)
		return;

	m_attackLoadTime += deltaTime;
	if (m_attackLoadTime >= m_baseAttackLoadTime)
	{
		*m_isAttackLoaded = true;
		*m_target = m_player->transform.GetWorldPosition();
	}
}

void BossLoadAttack::Reset()
{
	*m_isAttackLoaded = false;
	m_attackLoadTime = 0.f;
}

BossAttack::BossAttack(BossPattern* pPattern, bool* isAttackFinished)
{
	m_bossPattern = pPattern;
	m_isAttackFinished = isAttackFinished;
	m_patternPlaned = false;
}

void BossAttack::Update(float deltaTime)
{
	if (!m_patternPlaned)
	{
		m_bossPattern->GeneratePattern();
		m_patternPlaned = true;
	}

	if (*m_isAttackFinished) 
		return;

	m_bossPattern->Update(deltaTime);
	if (m_bossPattern->IsPatternFinished())
		*m_isAttackFinished = true;
}

void BossAttack::Reset()
{
	*m_isAttackFinished = false;
	m_patternPlaned = false;
}
