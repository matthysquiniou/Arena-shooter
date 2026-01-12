#include "WaveManager.h"
#include "Scripts/AgentBehavior.hpp"
#include <time.h>
#include <Enemy.h>
#include "Scripts/HealthBehavior.hpp"


WaveManager* WaveManager::s_instance = nullptr;

WaveManager* WaveManager::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new WaveManager();

	return s_instance;
}

void WaveManager::Destroy()
{
	delete s_instance;
}

void WaveManager::Update()
{
	if (!m_isTargetSet)
		SetEnemyTarget();
	TryNextWave();
	TrySpawn();
}

void WaveManager::EnnemyKilled(Agent* ennemy)
{
	if (ennemy == nullptr)
		return;

	Tag tag = ennemy->GetUniqueTag({ Tag::TMogwai, Tag::TGuHuoNiao, Tag::TJiangshi });
	ennemy->SetActive(false);
	auto it = m_ennemiesFreePool.find(tag);
	if (it != m_ennemiesFreePool.end())
	{
		m_ennemiesSpawnedPool.erase(ennemy);
		it->second.push_back(ennemy);
		m_currentEnnemiesAlives--;
	}
}

void WaveManager::Reset()
{
	std::unordered_set<Agent*> swapEnemies = m_ennemiesSpawnedPool;
	for (Agent* enemy : swapEnemies)
		EnnemyKilled(enemy);
	
	m_currentEnnemiesToSpawn = 0;
	m_currentEnnemiesAlives = 0;
	m_spawnTimer = 0.f;
	m_waveTimer = 0.f;
	m_curentWave = 0;
	m_isTargetSet = false;
}

void WaveManager::SetEnemyTarget()
{
	m_isTargetSet = true;
	gce::GameObject* player = gce::GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer });

	for (auto& [tag, enemies] : m_ennemiesFreePool)
	{
		for (Agent* enemy : enemies)
			enemy->SetTarget(player);
	}
}

WaveManager::WaveManager()
{
	
	std::random_device rand = std::random_device();
	m_rng = std::mt19937(rand());
	m_ennemyTag = { Tag::TMogwai, Tag::TJiangshi, Tag::TGuHuoNiao };
	m_ennemyTagDistrubution = std::uniform_int_distribution<int>(0, m_ennemyTag.size() - 1);

	gce::GameObject* player = gce::GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer });

	for (size_t i = 0; i < m_ennemyTag.size(); i++)
	{
		Tag ennemyTag = m_ennemyTag[i];
		for (size_t i = 0; i < m_maxSimultanateEnnemies; i++)
		{
			CreateEnnemy(ennemyTag, player);
		}
	}

	std::vector<GameObject*> spawners = gce::GameManager::GetSceneManager().GetAllGameObjects({ Tag::TSpawner });
	for (size_t i = 0; i < spawners.size(); i++)
	{
		gce::GameObject* spawner = spawners[i];
		for (size_t j = 0; j < m_ennemyTag.size(); j++)
		{
			Tag ennemyTag = m_ennemyTag[j];
			if (spawner->HasTags({ ennemyTag }))
				m_spawnerPosition[ennemyTag].push_back(spawner->transform.GetWorldPosition());
		}
	}

	for (size_t i = 0; i < m_ennemyTag.size(); i++)
	{
		Tag ennemyTag = m_ennemyTag[i];
		if (m_spawnerPosition[ennemyTag].empty())
			continue;
		m_spawnerPositionDistribution[ennemyTag] = std::uniform_int_distribution<int>(0, m_spawnerPosition[ennemyTag].size() - 1);
	}
}

void WaveManager::TryNextWave()
{
	if (GameManager::GetSceneManager().GetSceneType() != SceneType::GamePlayScene)
		return;

	if (m_curentWave == m_maxWave)
		return;

	if (m_currentEnnemiesAlives == 0 && m_currentEnnemiesToSpawn == 0)
		m_waveTimer = 0;

	if (m_waveTimer > 0)
	{
		m_waveTimer -= gce::GameManager::DeltaTime();
		return;
	}

	m_waveTimer = m_baseWaveTimer;
	m_curentWave++;
	m_currentEnnemiesToSpawn += GetEnnemiesNumberForWave(m_curentWave);
	m_currentMaxEnnemiesToSpawn = m_currentEnnemiesToSpawn;
	m_baseSpawnTimer = (m_baseWaveTimer * 0.25) / m_currentEnnemiesToSpawn;
}

void WaveManager::TrySpawn()
{
	if (GameManager::GetSceneManager().GetSceneType() != SceneType::GamePlayScene)
		return;


	if (m_currentEnnemiesToSpawn == 0 || m_currentEnnemiesAlives >= m_maxSimultanateEnnemies)
		return; 
	if (m_curentWave == m_maxWave)
	{
		gce::GameObject* player = gce::GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer });
		CreateEnnemy(Tag::TDragon, player);
		m_currentEnnemiesToSpawn = 0;
		return;
	}

	if (m_spawnTimer < m_baseSpawnTimer)
	{
		m_spawnTimer += gce::GameManager::DeltaTime();
		return;
	}

	m_spawnTimer = 0.f;
	
	int tagIndex = m_ennemyTagDistrubution(m_rng);
	Tag tag = m_ennemyTag[tagIndex];

	if (m_ennemiesFreePool[tag].empty())
		return;

	Agent* ennemy = m_ennemiesFreePool[tag].back();

	m_ennemiesSpawnedPool.insert(ennemy);
	m_ennemiesFreePool[tag].pop_back();
	ennemy->SetActive(true);
	ennemy->GetScript<HealthBehavior>()->FillHP();

	if (ennemy->HasComponent<BoxCollider>() == false)
	{
		ennemy->AddComponent<BoxCollider>()->SetActive(true);
	}

	int spawnPosIndex = m_spawnerPositionDistribution[tag](m_rng);
	gce::Vector3f32 spawnPos = m_spawnerPosition[tag][spawnPosIndex];
	ennemy->transform.SetWorldPosition(spawnPos);
	ennemy->SetCurrentNode(NavMesh::Instance()->GetNearestNodeFromPosition(spawnPos));
	m_currentEnnemiesAlives++;
	m_currentEnnemiesToSpawn--;
}

int WaveManager::GetEnnemiesNumberForWave(int wave)
{
	return 8 + wave * 4;
}

void WaveManager::CreateEnnemy(Tag tag, gce::GameObject* player) 
{
	Agent& enemy = Enemy::CreateEnemy(player, tag);
	enemy.AddTags({ Tag::TEnemy });

	if(tag != Tag::TDragon)
		enemy.SetActive(false);

	m_ennemiesFreePool[tag].push_back(&enemy);
}
