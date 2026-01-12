#include "BossPattern.h"
#include "Scripts/HealthBehavior.hpp"
#include "Prefabs/EntityWrapper.h"

BossPattern::BossPattern(gce::GameObject* boss, gce::GameObject* player)
{
	m_boss = boss;
	m_player = player;
	std::random_device rand = std::random_device();
	m_rng = std::mt19937(rand());

	int maxFireball = 10;
	//int maxLaser = 3;

	for (size_t i = 0; i < maxFireball * 2; i++)
		CreateFireBall();

	/*
	for (size_t i = 0; i < maxLaser * 2; i++)
		CreateLaser();
	*/
	m_offSetPositionDistribution = std::uniform_real_distribution<float>(-5.f, 5.f);
	m_patternTimeDistribution = std::uniform_real_distribution<float>(5.f, 12.f);
	m_numberOfFireballDistribution = std::uniform_int_distribution<int>(1, maxFireball);
	// m_numberOfLaserDistribution = std::uniform_int_distribution<int>(0, maxLaser);

	m_fireBallDamage = 15;
	m_fireBallSpeed = 15.f;

	//m_laserDamage = 5.f;
	//m_damageTickFrequency = 0.3f;
	//m_laserRotationSpeed = 0.7f;
}

BossPattern::~BossPattern()
{
	for (size_t i = 0; i < m_fireballPool.size(); i++)
	{
		m_fireballPool[i]->Destroy();
	}

	m_fireballPool.clear();

	for (gce::GameObject* go : m_launchedFireball)
	{
		go->Destroy();
	}

	m_launchedFireball.clear();
}

void BossPattern::Update(float deltaTime)
{
	m_timeSincePatternStart += deltaTime;

	while (!m_attackToLaunchQueue.empty())
	{
		const auto& [launchTime, attackType] = m_attackToLaunchQueue.top();

		if (launchTime > m_timeSincePatternStart)
			break;

		switch (attackType)
		{
		case BossTypeAttack::FIREBALL:
			LaunchFireball();
			break;
			/*
		case BossTypeAttack::LASER:
			LaunchLaser();
			break;
			*/
		}

		m_attackToLaunchQueue.pop();
	}
	MoveAttacks(deltaTime);
	CheckAttackCollision();
	CheckAttackEnd();
}

void BossPattern::GeneratePattern()
{
	AudioUse::Play("dragon_wander");

	m_attackToLaunchQueue = std::priority_queue<std::pair<float, BossTypeAttack>, std::vector<std::pair<float, BossTypeAttack>>, AttackTimeComparator>();

	m_timeSincePatternStart = 0.0f;
	const float patternDuration = m_patternTimeDistribution(m_rng);

	HealthBehavior* hb = m_boss->GetScript<HealthBehavior>();
	const float healthValue = gce::Clamp(hb->health / hb->maxHealth, 0.0f, 1.0f);
	const float aggressivity = 0.7f + (1.0f - healthValue);

	const int fireballCount = static_cast<int>(m_numberOfFireballDistribution(m_rng) * aggressivity);
	// const int laserCount = static_cast<int>(m_numberOfLaserDistribution(m_rng) * aggressivity);

	for (int i = 0; i < fireballCount; ++i)
	{
		const float launchTime = std::uniform_real_distribution<float>(0.0f, patternDuration)(m_rng);

		m_attackToLaunchQueue.push({ launchTime, BossTypeAttack::FIREBALL });
	}

	/*
	for (int i = 0; i < laserCount; ++i)
	{
		const float launchTime = std::uniform_real_distribution<float>(0.0f, patternDuration)(m_rng);

		m_attackToLaunchQueue.push({ launchTime, BossTypeAttack::LASER });
	}
	*/
}

bool BossPattern::IsPatternFinished()
{
	return m_launchedFireball.empty() && m_attackToLaunchQueue.empty();
	// return m_launchedFireball.empty() && m_launchedLaser.empty() && m_attackToLaunchQueue.empty();
}

void BossPattern::LaunchFireball()
{
	if (m_fireballPool.empty())
		CreateFireBall();

	gce::GameObject* fireball = m_fireballPool.back();
	m_fireballPool.pop_back();
	fireball->SetActive(true);

	MeshRenderer* mr = fireball->GetComponent<MeshRenderer>();
	float centerOffset = mr->pGeometry->max.z;

	gce::Vector3f32 forward = m_boss->transform.GetWorldForward();
	gce::Vector3f32 bossPos = m_boss->transform.GetWorldPosition();

	float theta = std::atan2(forward.x, forward.z);

	gce::Vector3f32 mouthPosition = { bossPos.x + std::cos(theta) * centerOffset, bossPos.y , bossPos.z + std::sin(theta) * centerOffset};
	fireball->transform.SetWorldPosition(mouthPosition);

	gce::Vector3f32 playerPos = m_player->transform.GetWorldPosition();
	playerPos.x += m_offSetPositionDistribution(m_rng);
	playerPos.z += m_offSetPositionDistribution(m_rng);

	gce::Vector3f32 dir = (playerPos - mouthPosition).Normalize();

	m_launchedFireball.insert(fireball);
	m_mapFireballDir[fireball] = dir;

	AudioUse::Play("fireball");
}

/*
void BossPattern::LaunchLaser()
{
	if (m_laserPool.empty())
		CreateLaser();

	gce::GameObject* laser = m_laserPool.back();
	m_laserPool.pop_back();
	laser->SetActive(true);
	laser->transform.SetWorldRotation({ 0.f,0.f,0.f });
	MeshRenderer* mr = laser->GetComponent<MeshRenderer>();
	float centerOffset = mr->pGeometry->max.z;

	gce::Vector3f32 forward = m_boss->transform.GetWorldForward();
	gce::Vector3f32 bossPos = m_boss->transform.GetWorldPosition();

	float theta = std::atan2(forward.x, forward.z);

	gce::Vector3f32 mouthPosition = { bossPos.x + std::cos(theta) * centerOffset, bossPos.y , bossPos.z + std::sin(theta) * centerOffset };
	laser->transform.SetWorldPosition(mouthPosition);

	gce::Vector3f32 playerPos = m_player->transform.GetWorldPosition();
	playerPos.x += m_offSetPositionDistribution(m_rng);
	playerPos.z += m_offSetPositionDistribution(m_rng);

	gce::Vector3f32 delta = playerPos - mouthPosition;
	delta.y = 0;
	float thetaLaser = std::atan2(delta.x, delta.z);

	laser->transform.WorldRotate({ gce::PI * 0.5f * std::cos(thetaLaser),0.f, gce::PI * 0.5f * std::sin(thetaLaser) });

	m_launchedLaser.insert(laser);
	m_mapLaserTetha[laser] = thetaLaser;
	m_mapLaserRotationDone[laser] = 0.f;
}
*/

void BossPattern::CreateFireBall()
{
	EntityWrapper& ew = EntityWrapper::Create();
	ew.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/FireBall/fireball.obj"), "res/Assets/FireBall/fireball_base_color.png");
	ew.SetActive(false);
	m_fireballPool.push_back(&ew);
}

/*
void BossPattern::CreateLaser()
{
	EntityWrapper& ew = EntityWrapper::Create();
	ew.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/laser/Laser.obj"), "res/Assets/laser/laser_base_color.png");
	ew.SetActive(false);
	m_laserPool.push_back(&ew);
}
*/

void BossPattern::CheckAttackEnd() 
{
	std::unordered_set<gce::GameObject*> copyLaunchedFireball = m_launchedFireball;
	for (gce::GameObject* fireball : copyLaunchedFireball)
	{
		if (fireball->transform.GetWorldPosition().y < 0.f)
		{
			fireball->SetActive(false);
			m_launchedFireball.erase(fireball);
			m_fireballPool.push_back(fireball);
		}
	}

	/*
	std::unordered_set<gce::GameObject*> copyLaunchedLaser = m_launchedLaser;
	for (gce::GameObject* laser : copyLaunchedLaser)
	{
		const float threshold = gce::PI * 0.5; 
		if (m_mapLaserRotationDone[laser] > threshold)
		{
			laser->SetActive(false);
			m_launchedLaser.erase(laser);
			m_laserPool.push_back(laser);
		}
	}
	*/

}

void BossPattern::CheckAttackCollision()
{	
	auto& transform1 = m_player->transform;

	gce::Vector3f32 pos1 = transform1.GetWorldPosition();
	gce::Vector3f32 scale1 = transform1.GetWorldScale();

	MeshRenderer* pMesh1 = m_player->GetComponent<MeshRenderer>();
	if (!pMesh1)
		return;

	auto& geo1 = pMesh1->pGeometry;
	gce::Vector3f32 min1 = pos1 + geo1->min * scale1;
	gce::Vector3f32 max1 = pos1 + geo1->max * scale1;

	std::unordered_set<gce::GameObject*> copyLaunchedFireball = m_launchedFireball;
	for (gce::GameObject* fireball : copyLaunchedFireball)
	{
		if (!fireball || !fireball->IsActive())
			continue;

		auto& transform2 = fireball->transform;

		gce::Vector3f32 pos2 = transform2.GetWorldPosition();
		gce::Vector3f32 scale2 = transform2.GetWorldScale();

		MeshRenderer* pMesh2 = fireball->GetComponent<MeshRenderer>();
		if (!pMesh2)
			continue;

		auto& geo2 = pMesh2->pGeometry;

		gce::Vector3f32 min2 = pos2 + geo2->min * scale2;
		gce::Vector3f32 max2 = pos2 + geo2->max * scale2;

		bool collision =
			(min1.x <= max2.x && max1.x >= min2.x) &&
			(min1.y <= max2.y && max1.y >= min2.y) &&
			(min1.z <= max2.z && max1.z >= min2.z);

		if (collision)
		{
			fireball->SetActive(false);
			m_launchedFireball.erase(fireball);
			m_fireballPool.push_back(fireball);
			GameManager::GetSceneManager().GetFirstGameObject({ Tag::TPlayer })->GetScript<HealthBehavior>()->TakeDamage(m_fireBallDamage);
		}
	}
}

void BossPattern::MoveAttacks(float deltaTime)
{
	for (gce::GameObject* fireball : m_launchedFireball)
	{
		gce::Vector3f32 dir = m_mapFireballDir[fireball];
		fireball->transform.WorldTranslate(dir * m_fireBallSpeed * deltaTime);
	}
	/*
	for (gce::GameObject* laser : m_launchedLaser)
	{
		float theta = m_mapLaserTetha[laser];
		float speed = m_laserRotationSpeed * deltaTime;
		gce::Vector3f32 rotation = { std::cos(theta) * speed,0.f, std::sin(theta) * speed };
		float deltaTheta = std::sqrt(rotation.x * rotation.x + rotation.z * rotation.z);
		m_mapLaserRotationDone[laser] += deltaTheta;
		laser->transform.WorldRotate(rotation);
	}
	*/
}
