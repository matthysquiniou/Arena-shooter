#include "EnemyScriptHelper.h"
#include <Components.h>
#include "Scripts/EnemyProjectileBehavior.hpp"

EnemyScriptHelper::EnemyScriptHelper()
{
}

void EnemyScriptHelper::InitMogwai()
{
    if (m_pAgent == nullptr)
        return;

    m_baseProjectileReload = 5.f;
    m_maxProjectiles = 3;
    m_currentProjectiles = 3;
    m_rangeDistanceAttack = 8.f;
    m_projectileDamage = 3;
    m_projectileLifeTime = 3.f;
    m_distanceAttackCooldown = 1.f;
    m_projectileSpeed = 8.f;

    m_rangeLaunchMelee = 3.f;
    m_rangeHitMelee = 3.f;
    m_meleeAttackCooldown = 0.7f;
    m_meleeAttackHitFrame = 0.4f;
    m_inAttack = false;
    m_meleeDamage = 6;

    for (size_t i = 0; i < static_cast<int>(m_distanceAttackCooldown * m_projectileLifeTime) + 1; i++)
    {
        EntityWrapper& ew = EntityWrapper::Create();
        ew.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/feijan/fei_jian.obj"), "res/Assets/feijan/fei_jian_base_color.png");
        ew.transform.SetWorldScale({ 2, 2, 2 });
        EnemyProjectileBehavior* epb = ew.AddScript<EnemyProjectileBehavior>();
        epb->m_baseLifeTime = m_projectileLifeTime;
        epb->m_speed = m_projectileSpeed;
        epb->m_damage = m_projectileSpeed;
        epb->m_ySpawnOffset = 1.f;
        epb->m_animation = [&ew](float deltaTime, gce::Vector3f32 dir) {
            ew.transform.LocalRotate({ deltaTime * 1.f, 0.f, deltaTime * 1.f }); // tourne n'importe comment mais pas la force pour la trigo
        };
        m_projectiles.push_back(&ew);
    }

    InitStateMachine();
}

void EnemyScriptHelper::InitJiangshi()
{
    if (m_pAgent == nullptr)
        return;

    m_baseProjectileReload = 10.f;
    m_maxProjectiles = 0;
    m_currentProjectiles = 0;
    m_rangeDistanceAttack = 4.f;
    m_projectileDamage = 7;
    m_projectileLifeTime = 1.f;
    m_distanceAttackCooldown = 0.0f;
    m_projectileSpeed = 12.f;

    m_rangeLaunchMelee = 3.f;
    m_rangeHitMelee = 3.5f;
    m_meleeAttackCooldown = 1.1f;
    m_meleeAttackHitFrame = 0.6f;
    m_inAttack = false;
    m_meleeDamage = 10;

    InitStateMachine();
}

void EnemyScriptHelper::InitGuhuoniao()
{
    if (m_pAgent == nullptr)
        return;

    m_baseProjectileReload = 1.f;
    m_maxProjectiles = 5;
    m_currentProjectiles = 5;
    m_rangeDistanceAttack = 15.f;
    m_projectileDamage = 6;
    m_projectileLifeTime = 3.f;
    m_distanceAttackCooldown = 1.5f;
    m_projectileSpeed = 10.f;

    m_rangeLaunchMelee = 3.f;
    m_rangeHitMelee = 3.5f;
    m_meleeAttackCooldown = 1.1f;
    m_meleeAttackHitFrame = 0.6f;
    m_inAttack = false;
    m_meleeDamage = 10;


    for (size_t i = 0; i < static_cast<int>(m_distanceAttackCooldown * m_projectileLifeTime) + 1; i++)
    {
        EntityWrapper& ew = EntityWrapper::Create();
        ew.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/Harpy/Attack/harpie_attack.obj"),"res/Assets/Harpy/Attack/harpie_attack_base_color.png"); // TODO :: projectile guhuoniao
        EnemyProjectileBehavior* epb = ew.AddScript<EnemyProjectileBehavior>();
        epb->m_baseLifeTime = m_projectileLifeTime;
        epb->m_speed = m_projectileSpeed;
        epb->m_ySpawnOffset = 1.f;
        epb->m_damage = m_projectileSpeed;

        m_projectiles.push_back(&ew);
    }

    InitStateMachine();
}

void EnemyScriptHelper::Update()
{
    if (m_pAgent == nullptr)
        return;

    m_stateMachine.Update();
}

void EnemyScriptHelper::Reset()
{
    m_stateMachine.SetState(State::MOVE);
    m_currentProjectiles = m_maxProjectiles;
    m_pAgent->transform.SetWorldPosition({ 0.f, -100.f, 0.f });
}

void EnemyScriptHelper::InitStateMachine()
{
    m_reloadAmmo = ReloadAmmo(&m_currentProjectiles, &m_maxProjectiles, m_baseProjectileReload);
    m_changeRange = ChangeAgentRange(m_pAgent, &m_currentProjectiles, m_rangeLaunchMelee, m_rangeDistanceAttack);
    m_meleeAttack = MeleeAttack(m_pAgent, m_meleeDamage, m_rangeHitMelee, m_meleeAttackHitFrame, m_meleeAttackCooldown, &m_inAttack);
    m_distanceAttack = DistanceAttack(m_pAgent, m_distanceAttackCooldown, &m_currentProjectiles, &m_projectiles);

    m_canMeleeAttack = CanMeleeAttack(m_pAgent, m_rangeLaunchMelee);
    m_canDistanceAttack = CanDistanceAttack(m_pAgent, &m_currentProjectiles, m_rangeDistanceAttack);
    m_cantAttack = CantAttack(m_pAgent, &m_currentProjectiles, &m_inAttack, m_rangeDistanceAttack);

    m_stateMachine.AddStateAction(State::MOVE, m_reloadAmmo);
    m_stateMachine.AddStateAction(State::MOVE, m_changeRange);
    m_stateMachine.AddStateCondition(State::MOVE, State::MELEE_ATTACK, m_canMeleeAttack);
    m_stateMachine.AddStateCondition(State::MOVE, State::DISTANCE_ATTACK, m_canDistanceAttack);

    m_stateMachine.AddStateAction(State::MELEE_ATTACK, m_changeRange);
    m_stateMachine.AddStateAction(State::MELEE_ATTACK, m_meleeAttack);
    m_stateMachine.AddStateCondition(State::MELEE_ATTACK, State::MOVE, m_cantAttack);

    m_stateMachine.AddStateAction(State::DISTANCE_ATTACK, m_changeRange);
    m_stateMachine.AddStateAction(State::DISTANCE_ATTACK, m_distanceAttack);
    m_stateMachine.AddStateCondition(State::DISTANCE_ATTACK, State::MOVE, m_cantAttack);

    m_stateMachine.SetState(State::MOVE);
}
