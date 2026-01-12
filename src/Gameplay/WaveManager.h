#pragma once
#include <vector>
#include <Agent.h>
#include "Tags.h"
#include <random>

class WaveManager
{
public:
	static WaveManager* GetInstance();
	static void Destroy();
	void Update();
	void EnnemyKilled(Agent* ennemy);
	void Reset();
private:
	void SetEnemyTarget();
	WaveManager();
	void TryNextWave();
	void TrySpawn();
	int GetEnnemiesNumberForWave(int wave);

	void CreateEnnemy(Tag tag, gce::GameObject* player);

private:
	static WaveManager* s_instance;

public:
	int m_maxSimultanateEnnemies = 10;
	bool m_isTargetSet = false;

	int m_currentMaxEnnemiesToSpawn = 0;
	int m_currentEnnemiesToSpawn = 0;
	int m_currentEnnemiesAlives = 0;
	
	float m_baseSpawnTimer = 1.f;
	float m_spawnTimer = 0.f;

	float m_baseWaveTimer = 120.f;
	float m_waveTimer = 0.f;

	int m_maxWave = 2;
	int m_curentWave = 0;

	std::unordered_map<Tag,std::vector<Agent*>> m_ennemiesFreePool;
	std::unordered_set<Agent*> m_ennemiesSpawnedPool;
	std::unordered_map<Tag,std::vector<gce::Vector3f32>> m_spawnerPosition;
	std::vector<Tag> m_ennemyTag;

	std::mt19937 m_rng;
	std::uniform_int_distribution<int> m_ennemyTagDistrubution;
	std::unordered_map<Tag, std::uniform_int_distribution<int>> m_spawnerPositionDistribution;
};

