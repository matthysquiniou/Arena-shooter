#pragma once
#include "Agent.h"

class Enemy
{
private:
	static void CreateMogwai(Agent& agent);
	static void CreateJiangshi(Agent& agent);
	static void CreateGuHuoNiao(Agent& agent);
	static void CreateDragon(Agent& dragon);
public:
	static Agent& CreateEnemy(gce::GameObject* target, Tag enemyType);
};

