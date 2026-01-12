#pragma once
#include "Structs.h"
#include "GameObject.h"
#include <random>

class CollectibleMunitionManager
{
public:
	static void CreateRandomMunition(gce::Vector3f32 position);
	static void Reset();
	static void MunitionTook(gce::GameObject* munition);

private:
	static std::mt19937 s_rng;
	static std::unordered_map<Tag, std::vector<gce::GameObject*>> s_freeMunitionsPool;
	static std::vector<Tag> s_munitionsTags;
	static std::uniform_int_distribution<int> s_tagIndexDistribution;
};

