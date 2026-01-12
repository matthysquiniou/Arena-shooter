#include "Node.h"

template<typename T, typename Agent>
const bool Node<T, Agent>::IsWalkable(Agent* allowedAgent) const {
	return data->IsWalkable() && (occupiedByAgent == nullptr || allowedAgent == nullptr || occupiedByAgent == allowedAgent);
}

template<typename T, typename Agent>
float Node<T, Agent>::CalculateEuclidieanDistance(Node<T, Agent>* toNode)
{
	distanceToTarget = data->CalculateEuclidieanDistance(toNode->data);
	return distanceToTarget;
}