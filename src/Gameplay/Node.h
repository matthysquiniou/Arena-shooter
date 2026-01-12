#pragma once
#include <vector>
#include <unordered_set>
#include <Maths/Vector3.h>

template <typename T, typename Agent>
struct Node
{
	T* data;

	Node<T,Agent>* cameFrom;
	std::unordered_set<Node<T, Agent>*> neighbors;

	uint32 visitedVersion;

	float distanceToStart;
	float distanceToTarget;

	Agent* occupiedByAgent;

	const bool IsWalkable(Agent* allowedAgent = nullptr) const;

	float CalculateEuclidieanDistance(Node<T, Agent>* toNode);
};

#include "Node.inl"