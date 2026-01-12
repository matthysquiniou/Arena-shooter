#include "BulletPool.h"
#include "../Scripts/BulletBehavior.hpp"

int BulletPool::m_maxAmount = 10;
std::queue<EntityWrapper*> BulletPool::m_bulletsFreePool = {};

void BulletPool::SetActive(gce::GameObject* e, bool state)
{
	e->SetActive(state);
}

void BulletPool::Init()
{
	for (size_t i = 0; i < m_maxAmount; i++)
	{
		EntityWrapper& bullet = EntityWrapper::Create();
		bullet.SetProperties("Bullet", { Tag::TProjectile, Tag::TBullet }, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
		bullet.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/ammo/ammo.obj"), "res/Assets/ammo/ammo_base_color.png")->SetActive(false);
		bullet.AddScript<BulletBehavior>();

		SetActive(&bullet, false);
		m_bulletsFreePool.push(&bullet);
	}
}

EntityWrapper* BulletPool::Generate()
{
	if (m_bulletsFreePool.empty())
		return nullptr;

	EntityWrapper* pCurrent = m_bulletsFreePool.front();
	m_bulletsFreePool.pop();

	pCurrent->transform.SetWorldPosition({ 0, 0, 0 });
	pCurrent->transform.SetWorldRotation({ 0, 0, 0 });
	pCurrent->transform.SetWorldScale({ 1, 1, 1 });

	m_bulletsFreePool.push(pCurrent);

	SetActive(pCurrent, true);

	return pCurrent;
}

void BulletPool::Desactive(gce::GameObject* e)
{
	SetActive(e, false);
}

void BulletPool::DesactivateAllBullets()
{
	for (gce::GameObject* go : GameManager::GetSceneManager().GetAllGameObjects({ Tag::TBullet }))
	{
		SetActive(go, false);
	}
}
