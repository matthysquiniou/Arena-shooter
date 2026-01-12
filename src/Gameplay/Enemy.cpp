#include "Enemy.h"
#include "Scripts/AgentBehavior.hpp"
#include "Scripts/GuhuoniaoBehavior.hpp"
#include "Scripts/JiangshiBehavior.hpp"
#include "Scripts/MogwaiBehavior.hpp"
#include "Scripts/HealthBehavior.hpp"
#include "Scripts/EnemyHpBehavior.hpp"
#include "Scripts/DragonHpBehavior.hpp"
#include "Scripts/DragonBehavior.hpp"

void Enemy::CreateMogwai(Agent& agent)
{
	Geometry* customGeo = gce::GeometryFactory::GetCustomGeometry("res/Assets/mogwai_lowcost/mogwai_lowcost.obj");
	agent.AddMeshRenderer(customGeo, "res/Assets/mogwai_lowcost/mogwai_lowcost_base_color.png");
	agent.transform.WorldTranslate({ 0.f, abs(customGeo->max.y) + abs(customGeo->min.y), 0.f });
	agent.AddScript<AgentBehavior>();
	agent.AddScript<HealthBehavior>()->SetMaxHP(80);

	agent.SetSpeed(4.5f);
	agent.AddScript<MogwaiBehavior>();
	agent.AddScript<EnemyHpBehavior>();
}

void Enemy::CreateJiangshi(Agent& agent)
{
	Geometry* customGeo = gce::GeometryFactory::GetCustomGeometry("res/Assets/jiangshi/jiangshi.obj");
	agent.AddMeshRenderer(customGeo, "res/Assets/jiangshi/jiangshi_base_color.png");
	agent.transform.WorldTranslate({ 0.f, abs(customGeo->max.y) + abs(customGeo->min.y), 0.f });
	agent.AddScript<AgentBehavior>();
	agent.AddScript<HealthBehavior>()->SetMaxHP(200);

	agent.SetSpeed(1.5f);
	agent.AddScript<JiangshiBehavior>();
	agent.AddScript<EnemyHpBehavior>();
}

void Enemy::CreateGuHuoNiao(Agent& agent)
{

	Geometry* customGeo = gce::GeometryFactory::GetCustomGeometry("res/Assets/Harpy/harpy.obj");
	agent.AddMeshRenderer(customGeo, "res/Assets/Harpy/harpy_base_color.png");
	agent.transform.WorldTranslate({ 0.f, abs(customGeo->max.y) + abs(customGeo->min.y), 0.f });
	agent.AddScript<AgentBehavior>();
	agent.AddScript<HealthBehavior>()->SetMaxHP(120);
	agent.SetSpeed(2.5f);
	agent.AddScript<GuHuoNiaoBehavior>();
	agent.AddScript<EnemyHpBehavior>();
}

void Enemy::CreateDragon(Agent& dragon)
{
	dragon.transform.SetWorldPosition({ 0, 15, 0 });
	dragon.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/dragon/dragon.obj"), "res/Assets/dragon/dragon_base_color.png");
	auto health = dragon.AddScript<HealthBehavior>();
	health->SetMaxHP(1000);
	dragon.AddScript<DragonHpBehavior>();
	dragon.AddScript<DragonBehavior>();
	GameManager::GetSceneManager().LinkObjectToScene(&dragon, SceneType::GamePlayScene);
	return;
}

Agent& Enemy::CreateEnemy(gce::GameObject* target, Tag enemyType)
{
	Agent& agent = Agent::Create();
	agent.AddTags({ enemyType });
	agent.AddComponent<gce::BoxCollider>();
	agent.SetTarget(target);

	switch (enemyType)
	{
	case Tag::TMogwai:
		CreateMogwai(agent);
		break;
	case Tag::TJiangshi:
		CreateJiangshi(agent);
		break;
	case Tag::TGuHuoNiao:
		CreateGuHuoNiao(agent);
		break;
	case Tag::TDragon:
		CreateDragon(agent);
		break;
	default:
		break;
	}


	return agent;
}
