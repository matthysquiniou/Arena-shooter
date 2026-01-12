#include "NavMesh.h"

#include <Components.h>
#include "GameManager.h"
#include "Utils.h"
#include <queue>

NavMesh* NavMesh::s_pInstance = nullptr;

void NavMesh::Create(gce::Vector<gce::Vertex> vertices, gce::Vector<uint32> indices, std::unordered_set<gce::GameObject*> obstacles)
{
	if (s_pInstance != nullptr)
	{
		delete s_pInstance;
	}
	s_pInstance = new NavMesh(vertices, indices, obstacles);
}

NavMesh* NavMesh::Instance()
{
	return s_pInstance;
}

void NavMesh::ResetVisited()
{
	if (++m_currentVersion == 0)
	{
		for (size_t i = 0; i < m_nodes.size(); i++)
		{
			Node<NavTile, Agent>* node = m_nodes[i];
			node->visitedVersion = 1;
		}
	}
}

Node<NavTile, Agent>* NavMesh::GetNearestNodeFromPosition(gce::Vector3f32 position)
{
	float bestDistance = std::numeric_limits<float>::infinity();
	NavTile* nearest;
	for (size_t i = 0; i < m_tiles.size(); i++)
	{
		NavTile* tile = m_tiles[i];
		float distance = (tile->GetPosition() - position).Norm();
		if (distance < bestDistance)
		{
			nearest = tile;
			bestDistance = distance;
		}
	}

	return m_mapTileNode[nearest];
}

bool NavMesh::DoesSegmentGoThroughObstacles(const gce::Vector3f32& A, const gce::Vector3f32& B, const float& radius)
{
	for (gce::GameObject* obstacle : m_obstacles)
	{
		gce::Vector3f32 obstaclePos = obstacle->transform.GetWorldPosition();
		gce::MeshRenderer* mr = obstacle->GetComponent<gce::MeshRenderer>();
		gce::Geometry* geo = mr->pGeometry;
		float minX = geo->min.x + obstaclePos.x;
		float maxX = geo->max.x + obstaclePos.x;
		float minZ = geo->min.z + obstaclePos.z;
		float maxZ = geo->max.z + obstaclePos.z;

		if (SegmentIntersectsRectXZ(A, B, minX, maxX, minZ, maxZ, radius))
			return true;
	}
	return false;
}

uint32 NavMesh::GetCurrentVisitedVersion()
{
	return m_currentVersion;
}

void NavMesh::UpdateNodesObstacle(float radius, Node<NavTile,Agent>* node)
{
	std::unordered_set<Node<NavTile, Agent>*> nodesToUpdate;
	ResetVisited();
	std::queue<Node<NavTile, Agent>*> queue;
	node->visitedVersion = m_currentVersion;
	queue.push(node);

	while (!queue.empty())
	{
		Node<NavTile, Agent>* fNode = queue.front();
		queue.pop();
		if (fNode->CalculateEuclidieanDistance(node) < radius)
			nodesToUpdate.insert(fNode);
		for (Node<NavTile, Agent>* nNode : fNode->neighbors)
		{
			if (nNode->visitedVersion != m_currentVersion)
			{
				nNode->visitedVersion = m_currentVersion;
				queue.push(nNode);
			}
		}
	}
	std::unordered_map<std::string, std::vector<Node<NavTile, Agent>*>> mapEdgesUpdateNodes;

	for (Node<NavTile, Agent>* uNode : nodesToUpdate)
	{
		uNode->data->CheckObstacles(m_obstacles);
		std::erase_if(uNode->neighbors, [&nodesToUpdate] (Node<NavTile, Agent>* testNode) { return nodesToUpdate.contains(testNode); });
		for (std::string edge : uNode->data->m_edges)
			mapEdgesUpdateNodes[edge].push_back(uNode);
	}

	for (auto& [edge, nodes] : mapEdgesUpdateNodes)
	{
		if (nodes.size() == 2)
		{
			Node<NavTile, Agent>* a = nodes[0];
			Node<NavTile, Agent>* b = nodes[1];

			if (a->IsWalkable() && b->IsWalkable())
			{
				a->neighbors.insert(b);
				b->neighbors.insert(a);
			}
		}
	}
}

void NavMesh::ResetObstacles()
{
	for (gce::GameObject* obstacle : m_disabledObstacles)
	{
		m_obstacles.insert(obstacle);
		obstacle->SetActive(true);
	}
	m_disabledObstacles.clear();

	std::unordered_map<std::string, std::vector<Node<NavTile, Agent>*>> mapEdgesUpdateNodes;

	for (Node<NavTile, Agent>* uNode : m_nodes)
	{
		uNode->data->CheckObstacles(m_obstacles);
		uNode->neighbors.clear();
		for (std::string edge : uNode->data->m_edges)
			mapEdgesUpdateNodes[edge].push_back(uNode);
	}

	for (auto& [edge, nodes] : mapEdgesUpdateNodes)
	{
		if (nodes.size() == 2)
		{
			Node<NavTile, Agent>* a = nodes[0];
			Node<NavTile, Agent>* b = nodes[1];

			if (a->IsWalkable() && b->IsWalkable())
			{
				a->neighbors.insert(b);
				b->neighbors.insert(a);
			}
		}
	}
}

void NavMesh::DisableObstacle(gce::GameObject* obstacle)
{
	if (m_obstacles.contains(obstacle))
	{
		m_obstacles.erase(obstacle);
		m_disabledObstacles.insert(obstacle);
		gce::MeshRenderer* mr = obstacle->GetComponent<gce::MeshRenderer>();
		if (mr == nullptr)
			return;
		obstacle->SetActive(false);
		UpdateNodesObstacle(max(mr->pGeometry->max.x,max(mr->pGeometry->max.y, mr->pGeometry->max.z)), GetNearestNodeFromPosition(obstacle->transform.GetWorldPosition()));
	}
}

NavMesh::NavMesh(gce::Vector<gce::Vertex> vertices, gce::Vector<uint32> indices, std::unordered_set<gce::GameObject*> obstacles)
{
	m_obstacles = obstacles;
	size_t triangleNumber = indices.Size() / 3;
	std::unordered_map<std::string, std::vector<Node<NavTile, Agent>*>> mapIndicesNodes;

	for (size_t i = 0; i < triangleNumber; i++)
	{
		size_t indice1 = indices[i * 3];
		size_t indice2 = indices[i * 3 + 1];
		size_t indice3 = indices[i * 3 + 2];

		NavTile* navTile = new NavTile(vertices[indice1], vertices[indice2], vertices[indice3]);
		m_tiles.push_back(navTile);
		navTile->CheckObstacles(obstacles);

		Node<NavTile, Agent>* node = new Node<NavTile, Agent>{ navTile };
		m_nodes.push_back(node);

		m_mapTileNode[navTile] = node;

		std::string edge1 = std::to_string(min(indice1, indice2)) + std::to_string(max(indice1, indice2));
		std::string edge2 = std::to_string(min(indice2, indice3)) + std::to_string(max(indice2, indice3));
		std::string edge3 = std::to_string(min(indice1, indice3)) + std::to_string(max(indice1, indice3));
		navTile->m_edges = { edge1, edge2, edge3 };

		auto it1 = mapIndicesNodes.find(edge1);
		if (it1 != mapIndicesNodes.end())
			it1->second.push_back(node);
		else
			mapIndicesNodes[edge1] = { node };

		auto it2 = mapIndicesNodes.find(edge2);
		if (it2 != mapIndicesNodes.end())
			it2->second.push_back(node);
		else
			mapIndicesNodes[edge2] = { node };

		auto it3 = mapIndicesNodes.find(edge3);
		if (it3 != mapIndicesNodes.end())
			it3->second.push_back(node);
		else
			mapIndicesNodes[edge3] = { node };
	}
	
	for (auto& [edge, nodes] : mapIndicesNodes)
	{
		if (nodes.size() == 2)
		{
			Node<NavTile, Agent>* a = nodes[0];
			Node<NavTile, Agent>* b = nodes[1];

			if (a->IsWalkable() && b->IsWalkable())
			{
				a->neighbors.insert(b);
				b->neighbors.insert(a);
			}
		}
	}
}

NavMesh::~NavMesh()
{
	for (auto& pair : m_mapTileNode)
	{
		delete pair.first;
		delete pair.second;
	}
}
