#include "NavTile.h"
#include <Geometry.h>
#include <GameObject.h>

NavTile::NavTile(gce::Vertex& vertex1, gce::Vertex& vertex2, gce::Vertex& vertex3)
{
	m_p1 = vertex1.pos;
	m_p2 = vertex2.pos;
	m_p3 = vertex3.pos;

	m_center = (m_p1 + m_p2 + m_p3) / 3;

	m_max = {
		gce::Max(m_p1.x,gce::Max(m_p2.x,m_p3.x)),
		gce::Max(m_p1.y,gce::Max(m_p2.y,m_p3.y)),
		gce::Max(m_p1.z,gce::Max(m_p2.z,m_p3.z))
	};
	m_min = {
		gce::Min(m_p1.x,gce::Min(m_p2.x,m_p3.x)),
		gce::Min(m_p1.y,gce::Min(m_p2.y,m_p3.y)),
		gce::Min(m_p1.z,gce::Min(m_p2.z,m_p3.z))
	};
}

void NavTile::CheckObstacles(std::unordered_set<gce::GameObject*> obstacles)
{
	m_isWalkable = true;
	for (gce::GameObject* obstacle : obstacles)
	{
		gce::Vector3f32 pos = obstacle->transform.GetWorldPosition();
		gce::Geometry* obstacleGeo = obstacle->GetComponent<gce::MeshRenderer>()->pGeometry;
		const float32& maxX = obstacleGeo->max.x + pos.x;
		const float32& maxY = obstacleGeo->max.y + pos.y;
		const float32& maxZ = obstacleGeo->max.z + pos.z;
		const float32& minX = obstacleGeo->min.x + pos.x;
		const float32& minY = obstacleGeo->min.y + pos.y;
		const float32& minZ = obstacleGeo->min.z + pos.z;
		if (IsInBounds({minX,minY,minZ}, {maxX,maxY,maxZ}))
		{
			m_isWalkable = false;
			return;
		}
	}	
	
	/*
		gce::GameObject& go = gce::GameObject::Create(gce::GameManager::GetScene());
		gce::MeshRenderer* mr = go.AddComponent<gce::MeshRenderer>();
		mr->pGeometry = gce::SHAPES.CUBE;
		mr->pPso = gce::GameManager::GetSceneManager().GetPSO();
		go.transform.SetWorldPosition(m_center);
		go.transform.SetWorldScale({ 0.1f,0.5f,0.1f });
	*/
}

const gce::Vector3f32& NavTile::GetPosition() const
{
	return m_center;
}

const bool NavTile::IsInBounds(gce::Vector3f32 min, gce::Vector3f32 max) const // only check on x and z axis
{
	return m_max.x <= max.x && m_max.x >= min.x && m_max.z <= max.z && m_max.z >= min.z ||
		m_min.x <= max.x && m_min.x >= min.x && m_max.z <= max.z && m_max.z >= min.z ||
		m_max.x <= max.x && m_max.x >= min.x && m_min.z <= max.z && m_min.z >= min.z ||
		m_min.x <= max.x && m_min.x >= min.x && m_min.z <= max.z && m_min.z >= min.z;
}

const float NavTile::CalculateEuclidieanDistance(const NavTile* otherNavTile) const
{
	float x = otherNavTile->GetPosition().x - m_center.x;
	float y = 0.f; // may be origine of not optinal path on plane navmesh otherNavTile->GetPosition().y - m_center.y;
	float z = otherNavTile->GetPosition().z - m_center.z;
	return std::sqrtf(x * x + y * y + z * z);
}

const bool NavTile::IsWalkable() const
{
	return m_isWalkable;
}
