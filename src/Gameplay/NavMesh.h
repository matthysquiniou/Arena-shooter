#pragma once
#include <vector>
#include <Structs.h>
#include <NavTile.h>
#include <Node.h>
#include <Containers/Vector.hpp>

class Agent;

namespace gce
{
	class GameObject;
}

class NavMesh
{
public:
	static void Create(gce::Vector<gce::Vertex> vertices, gce::Vector<uint32> indices, std::unordered_set<gce::GameObject*> obstacles);
	static NavMesh* Instance();
	void ResetVisited();
	Node<NavTile, Agent>* GetNearestNodeFromPosition(gce::Vector3f32 position); // clearly not optimal
	bool DoesSegmentGoThroughObstacles(const gce::Vector3f32& A, const gce::Vector3f32& B,const float& radius);
	uint32 GetCurrentVisitedVersion();
	void ResetObstacles();
	void DisableObstacle(gce::GameObject* obstacle);

private:
	NavMesh(gce::Vector<gce::Vertex> vertices, gce::Vector<uint32> indices, std::unordered_set<gce::GameObject*> obstacles);
	~NavMesh();
	void UpdateNodesObstacle(float radius, Node<NavTile, Agent>* node);

private:
	static NavMesh* s_pInstance;
	uint32 m_currentVersion;
	std::vector<Node<NavTile, Agent>*> m_nodes;
	std::vector<NavTile*> m_tiles;
	std::unordered_set<gce::GameObject*> m_obstacles;
	std::unordered_set<gce::GameObject*> m_disabledObstacles;
	std::unordered_map<NavTile*, Node<NavTile, Agent>*> m_mapTileNode;
};

