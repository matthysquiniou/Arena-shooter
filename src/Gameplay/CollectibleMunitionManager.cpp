#include "CollectibleMunitionManager.h"
#include "Prefabs/EntityWrapper.h"
#include "Scripts/CollectibleMunitionBehavior.hpp"

std::mt19937 CollectibleMunitionManager::s_rng = std::mt19937(std::random_device()());
std::unordered_map<Tag, std::vector<gce::GameObject*>> CollectibleMunitionManager::s_freeMunitionsPool;
std::vector<Tag> CollectibleMunitionManager::s_munitionsTags = { Tag::TLightAmmo, Tag::TNormalAmmo, Tag::THeavyAmmo };
std::uniform_int_distribution<int> CollectibleMunitionManager::s_tagIndexDistribution = std::uniform_int_distribution<int>(0, s_munitionsTags.size() - 1);

void CollectibleMunitionManager::CreateRandomMunition(gce::Vector3f32 position)
{
	int tagIndex = s_tagIndexDistribution(s_rng);
	Tag ammoTag = s_munitionsTags[tagIndex];

	std::vector<gce::GameObject*> freeAmmoType = s_freeMunitionsPool[ammoTag];
	if (freeAmmoType.size() > 0)
	{
		gce::GameObject* ammo = freeAmmoType.back();
		freeAmmoType.pop_back();
		ammo->transform.SetWorldPosition(position);
		ammo->SetActive(true);
		return;
	}

	EntityWrapper& ew = EntityWrapper::Create();
	ew.transform.SetWorldPosition(position);
	CollectibleMunitionBehavior* cmb = ew.AddScript<CollectibleMunitionBehavior>();
	cmb->m_ammoTag = ammoTag;
	ew.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/ammo/ammo.obj"), "res/Assets/ammo/ammo_base_color.png");
	ew.AddTags({ ammoTag, Tag::TCollectibleAmmo });
	switch (ammoTag)
	{
	case Tag::THeavyAmmo:
		ew.transform.SetWorldScale({ 7.f, 7.f, 3.5f });
		cmb->m_ammoEarn = 10;
		break;
	case Tag::TNormalAmmo:
		ew.transform.SetWorldScale({ 5.f, 5.f, 5.f });
		cmb->m_ammoEarn = 40;
		break;
	case Tag::TLightAmmo:
		ew.transform.SetWorldScale({ 3.f, 3.f, 6.f });
		cmb->m_ammoEarn = 70;
		break;
	default:
		break;
	}
}

void CollectibleMunitionManager::MunitionTook(gce::GameObject* munition)
{
	munition->SetActive(false);
	Tag tagAmmo = munition->GetUniqueTag({ Tag::THeavyAmmo, Tag::TNormalAmmo, Tag::TLightAmmo });
	s_freeMunitionsPool[tagAmmo].push_back(munition);
}

void CollectibleMunitionManager::Reset()
{
	std::vector<gce::GameObject*> ammos = gce::GameManager::GetSceneManager().GetAllGameObjects({ Tag::TCollectibleAmmo });
	for (size_t i = 0; i < ammos.size(); i++)
	{
		gce::GameObject* ammo = ammos[i];
		if (!ammo->IsActive())
			continue;
		MunitionTook(ammo);
	}
}