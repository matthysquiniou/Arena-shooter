#pragma once
#include "NavMesh.h"

class Agent;

static class PathFinder
{
public:
	PathFinder() = delete;
	static std::vector<Node<NavTile, Agent>*> GetPath(std::vector< Node<NavTile, Agent>*> pathToCalculate, Agent* allowedAgent = nullptr);

private:
	struct ComparatorAStar
	{
		bool operator()(Node<NavTile,Agent>* a, Node<NavTile, Agent>* b) {
			return (a->distanceToTarget + a->distanceToStart) > (b->distanceToTarget + b->distanceToStart);
		}
	};


private:
	static std::vector<Node<NavTile, Agent>*> AStar(Node<NavTile, Agent>* fromNode, Node<NavTile, Agent>* toNode, Agent* allowedAgent);
};

