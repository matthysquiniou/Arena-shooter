#pragma once
#include "Agent.h"
#include "BossPattern.h"

class Actione
{
public:
	virtual void Update(float deltaTime) {}
	virtual void Reset() {}
};

class ReloadAmmo : public Actione
{
public:
	ReloadAmmo() {}
	ReloadAmmo(int* ammo, int* maxAmmo, float baseReloadAmmo);
	void Update(float deltaTime) override;
private:
	int* m_ammo;
	int* m_maxAmmo;
	float m_baseReloadAmmo;
	float m_reloadAmmo;
};

class ChangeAgentRange : public Actione
{
public:
	ChangeAgentRange() {}
	ChangeAgentRange(Agent* agent, int* ammo, float meleeRange, float distanceRange);
	void Update(float deltaTime) override;
private:
	Agent* m_agent;
	int* m_ammo;
	float m_meleeRange;
	float m_distanceRange;
};

class MeleeAttack : public Actione
{
public:
	MeleeAttack() {}
	MeleeAttack(Agent* agent, int damage, int hitRange, float attackHitFrame, float baseAttackCooldown, bool* attackLaunched);
	void Update(float deltaTime) override;
	void Reset() override;
private:
	Agent* m_agent;
	int m_hitRange;
	float m_attackHitFrame;
	float m_baseAttackCooldown;
	float m_attackCooldown;
	float m_currentAttackFrame;
	bool* m_attackLaunched;
	bool m_attackReady;
	int m_damage;
};

class DistanceAttack : public Actione
{
public:
	DistanceAttack() {}
	DistanceAttack(Agent* agent, float baseAttackCooldown, int* ammo, std::vector<gce::GameObject*>* projectiles);
	void Update(float deltaTime) override;
	void Reset() override;
private:
	Agent* m_agent;
	int* m_ammo;
	float m_baseAttackCooldown;
	float m_attackCooldown;
	bool m_attackReady;
	std::vector<gce::GameObject*>* m_projectiles;
	std::unordered_set<gce::GameObject*> m_lauchedProjectiles;
};

class RotateTowardTarget : public Actione
{
public:
	RotateTowardTarget() {}
	RotateTowardTarget(gce::GameObject* owner, gce::Vector3f32* target, bool* rotationFinised, float rotationSpeed);

	void Update(float deltaTime) override;
	void Reset() override;
private:
	gce::GameObject* m_owner;
	gce::Vector3f32* m_target;
	bool* m_rotationFinised;
	float m_rotationSpeed;
};

class RotateAround : public Actione
{
public:
	RotateAround() {}
	RotateAround(gce::GameObject* owner, gce::GameObject* target, gce::Vector3f32* translateTarget, bool* isRotationFinished, int circlePoints, float speed, float radius);

	void Update(float deltaTime) override;
	void Reset() override;
private:
	void ChangeTarget();
private:
	gce::GameObject* m_owner;
	gce::GameObject* m_target;
	gce::Vector3f32* m_translateTarget;
	bool* m_isRotationFinished;
	float m_stepRadian;
	float m_speed;
	float m_radius;
	bool m_isTranslateTargetSet;
	gce::Vector3f32 m_direction;
	float32 m_distanceToTravel;
};

class BossLoadAttack : public Actione
{
public:
	BossLoadAttack() {}
	BossLoadAttack(bool* isAttackLoaded, float attackLoadTime, gce::GameObject* player, gce::Vector3f32* target);

	void Update(float deltaTime) override;
	void Reset() override;
private:
	bool* m_isAttackLoaded;
	float m_baseAttackLoadTime;
	float m_attackLoadTime;
	gce::GameObject* m_player;
	gce::Vector3f32* m_target;
};

class BossAttack : public Actione
{
public:
	BossAttack() {};
	BossAttack(BossPattern* pPattern, bool* isAttackFinished);

	void Update(float deltaTime) override;
	void Reset() override;

private:
	BossPattern* m_bossPattern;
	bool m_patternPlaned;
	bool* m_isAttackFinished;
};