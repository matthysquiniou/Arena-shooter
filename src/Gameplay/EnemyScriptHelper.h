#pragma once
#include <vector>
#include "Agent.h"
#include "StateMachinee.h"

class EnemyScriptHelper
{
public:
    EnemyScriptHelper();

    void InitMogwai();
    void InitJiangshi();
    void InitGuhuoniao();
    void Update();
    void Reset();

private:
    void InitStateMachine();

public:
    Agent* m_pAgent;

private:
    float m_baseProjectileReload;
    int m_maxProjectiles;
    int m_currentProjectiles;
    float m_rangeDistanceAttack;
    int m_projectileDamage;
    float m_projectileLifeTime;
    float m_distanceAttackCooldown;
    float m_projectileSpeed;
    std::vector<gce::GameObject*> m_projectiles;

    float m_rangeLaunchMelee;
    float m_rangeHitMelee;
    float m_meleeAttackCooldown;
    float m_meleeAttackHitFrame;
    bool m_inAttack;
    int m_meleeDamage;

    StateMachinee m_stateMachine;
    ReloadAmmo m_reloadAmmo;
    ChangeAgentRange m_changeRange;
    MeleeAttack m_meleeAttack;
    DistanceAttack m_distanceAttack;

    CanMeleeAttack m_canMeleeAttack;
    CanDistanceAttack m_canDistanceAttack;
    CantAttack m_cantAttack;
};
