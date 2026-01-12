#pragma once
#include <Structs.h>
#include <unordered_set>

namespace gce
{
	class GameObject;
}

class NavTile
{
public:
	NavTile(gce::Vertex& vertex1, gce::Vertex& vertex2, gce::Vertex& vertex3);
	void CheckObstacles(std::unordered_set<gce::GameObject*> obstacles);
	const gce::Vector3f32& GetPosition() const;
	const bool IsInBounds(gce::Vector3f32 min, gce::Vector3f32 max) const;
	const float CalculateEuclidieanDistance(const NavTile* otherNavTile) const;
	const bool IsWalkable() const;

public:
	std::vector<std::string> m_edges;

private:
	bool m_isWalkable = true;

	gce::Vector3f32 m_p1;
	gce::Vector3f32 m_p2;
	gce::Vector3f32 m_p3;

	gce::Vector3f32 m_center;

	gce::Vector3f32 m_max;
	gce::Vector3f32 m_min;

};

