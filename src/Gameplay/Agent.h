#pragma once
#include "Prefabs/EntityWrapper.h"
#include "PathFinder.h"
#include <queue>

class Agent : public EntityWrapper
{

public:
	Agent();
	static Agent& Create();

	void FollowPathToTarget();
	void ReleaseAllNodes();
	void SetTarget(GameObject* target);
	void SetSpeed(float speed);
	void SetStopRange(float stopRange);
	void SetCurrentNode(Node<NavTile, Agent>* node);
	void ResetBlockedTime();
	bool IsTargetInRange();
	float GetStopRange();
	float GetDistanceFromTarget();
	gce::GameObject* GetTarget();
	bool IsTargetBehindObstacle();

private:
	void CalculateNextLine();
	void FollowCurrentLine();
	void MoveToTarget();
	void ReleaseTraveledNodes();
	bool AcquireTravelingToNodes(Node<NavTile, Agent>* goToNode);
	bool IsNodeInBounds(Node<NavTile,Agent>* node, gce::Vector3f32 min, gce::Vector3f32 max);
	void FindPath();
	bool HasTargetMovedTooMuch();
	bool NeedCalculatePath();
	void RotateTowardDirection();
private:
	gce::Vector3f32 m_targetForward;
	float m_rotationSpeed = 3.f;
	bool m_isRotating = false;

	float m_speed = 3.f;
	float m_stopRange = 2.f;

	float m_baseBlockedTime = 0.2f;
	float m_blockedTime = 0.f;
	bool m_blocked = false;
	Agent* m_pBlockedBy;

	bool m_hasReleasedTraveled = false;

	float m_baseNoPathTime = 0.5f;
	float m_noPathTime = 0.f;

	bool m_isMoving = false;
	std::queue<Node<NavTile, Agent>*> m_currentLineNodes;
	std::queue<gce::Vector3f32> m_currentLineTargets;

	float m_distanceToMove;
	gce::Vector3f32 m_movingTo;
	gce::Vector3f32 m_direction;

	size_t m_currentPathIndex;
	Node<NavTile, Agent>* m_pCurrentNode;
	std::vector<Node<NavTile, Agent>*> m_path;
	std::unordered_set<Node<NavTile, Agent>*> m_nodesOccupied;
	std::vector<Node<NavTile, Agent>*> m_needToAcquire;

	GameObject* m_pTarget;
	gce::Vector3f32 m_lastTargetCalculatedPathCoordinates;
};