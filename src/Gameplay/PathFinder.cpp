#include "PathFinder.h"
#include <queue>

std::vector<Node<NavTile, Agent>*> PathFinder::GetPath(std::vector<Node<NavTile, Agent>*> pathToCalculate, Agent* allowedAgent)
{
	if (pathToCalculate.size() < 2)
		return {};

	std::vector<Node<NavTile, Agent>*> path;

	path.push_back(pathToCalculate[0]);
	Node<NavTile, Agent>* fromNode = pathToCalculate[0];

	for (size_t i = 1; i < pathToCalculate.size(); i++)
	{
		Node<NavTile, Agent>* toNode = pathToCalculate[i];

		std::vector<Node<NavTile, Agent>*> partialPath;

		NavMesh::Instance()->ResetVisited();
		toNode->cameFrom = nullptr;
		partialPath = AStar(fromNode, toNode, allowedAgent);

		if (partialPath.empty())
			continue;
		else
			fromNode = toNode;


		path.insert(path.end(), partialPath.begin(), partialPath.end());
	}

	return path;
}

std::vector<Node<NavTile, Agent>*> PathFinder::AStar(Node<NavTile, Agent>* fromNode, Node<NavTile, Agent>* toNode, Agent* allowedAgent)
{
	std::priority_queue<Node<NavTile, Agent>*, std::vector<Node<NavTile, Agent>*>, ComparatorAStar> queue;

	if (!toNode->IsWalkable(allowedAgent))
		return {};

	queue.push(fromNode);

	fromNode->distanceToStart = 0;
	fromNode->CalculateEuclidieanDistance(toNode);

	uint32 visitedVersion = NavMesh::Instance()->GetCurrentVisitedVersion();

	while (!queue.empty() && toNode->cameFrom == nullptr)
	{
		Node<NavTile, Agent>* node = queue.top();
		queue.pop();

		for (Node<NavTile, Agent>* neighbor : node->neighbors)
		{

			if (allowedAgent != nullptr && !neighbor->IsWalkable(allowedAgent))
				continue;

			if (neighbor == toNode) {
				neighbor->cameFrom = node;
				break;
			}

			if (neighbor->visitedVersion == visitedVersion)
				continue;

			neighbor->visitedVersion = visitedVersion;
			neighbor->cameFrom = node;

			neighbor->distanceToStart = node->distanceToStart + neighbor->CalculateEuclidieanDistance(node);
			neighbor->CalculateEuclidieanDistance(toNode);

			queue.push(neighbor);
		}
	}

	std::vector<Node<NavTile, Agent>*> path;

	while (toNode->cameFrom != nullptr && toNode != fromNode)
	{
		path.push_back(toNode);
		toNode = toNode->cameFrom;
	}

	std::reverse(path.begin(), path.end());

	return path;
}
