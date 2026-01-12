#include "Agent.h"
#include "GameManager.h"
#include "Utils.h"
#include <queue>

Agent::Agent() : EntityWrapper()
{
}

Agent& Agent::Create()
{
	Agent* const pNew = new Agent();
	pNew->m_pScene = &gce::GameManager::GetScene();
	pNew->transform.m_pOwner = pNew;
	gce::GameManager::GetLifespanSystem().m_toCreate.gameObjects.Push(pNew);
	return *pNew;
}

void Agent::FollowPathToTarget()
{
	if (m_isRotating)
		RotateTowardDirection();

	if ((IsTargetInRange() && !IsTargetBehindObstacle()) || m_pTarget == nullptr )
		return;
	if (NeedCalculatePath())
		FindPath();

	if (m_isMoving)
		MoveToTarget();
	else if (m_currentLineNodes.empty())
		CalculateNextLine();
	else if (!m_currentLineNodes.empty())
		FollowCurrentLine();
}

void Agent::ReleaseAllNodes()
{
	std::unordered_set<Node<NavTile, Agent>*> copyOccupiedNode = m_nodesOccupied;
	for (Node<NavTile, Agent>* node : copyOccupiedNode)
	{
		m_nodesOccupied.erase(node);
		node->occupiedByAgent = nullptr;
	}
}

void Agent::SetTarget(GameObject* target)
{
	m_pTarget = target;
}

void Agent::SetSpeed(float speed)
{
	m_speed = speed;
}

void Agent::SetStopRange(float stopRange)
{
	m_stopRange = stopRange;
}

void Agent::SetCurrentNode(Node<NavTile, Agent>* node)
{
	m_pCurrentNode = node;
}

void Agent::ResetBlockedTime()
{
	m_blockedTime = 0.f;
}

bool Agent::IsTargetInRange()
{
	gce::Vector3f32 selfPos = transform.GetWorldPosition();
	gce::Vector3f32 targetPos = m_pTarget->transform.GetWorldPosition();
	return (selfPos - targetPos).Norm() < m_stopRange;
}

float Agent::GetStopRange()
{
	return m_stopRange;
}

float Agent::GetDistanceFromTarget()
{
	gce::Vector3f32 targetPos = m_pTarget->transform.GetWorldPosition();
	return (transform.GetWorldPosition() - targetPos).Norm();
}

gce::GameObject* Agent::GetTarget()
{
	return m_pTarget;
}


bool Agent::IsTargetBehindObstacle()
{
	gce::Vector3f32 myPos = transform.GetWorldPosition();
	myPos.y = 0.f;
	gce::Vector3f32 targetPos = m_pTarget->transform.GetWorldPosition();
	targetPos.y = 0.f;
	return NavMesh::Instance()->DoesSegmentGoThroughObstacles(myPos, targetPos, 0.4f);
}

void Agent::CalculateNextLine()
{
	if (m_path.size() < 2)
		return;

	if (m_currentPathIndex >= m_path.size() - 1) // should not happen
		return;

	gce::MeshRenderer* meshRenderer = GetComponent<gce::MeshRenderer>();

	if (meshRenderer == nullptr)
		return;

	gce::Vector3f32 pos = transform.GetWorldPosition();
	gce::Vector3f32 boxMin = meshRenderer->pGeometry->min;
	float radius = boxMin.Norm();

	size_t indexOffset = 1;

	gce::Vector3f32 lastNodePos;

	NavMesh* navMesh = NavMesh::Instance();

	while (m_currentPathIndex + indexOffset < m_path.size())
	{
		Node<NavTile, Agent>* pToNode = m_path[m_currentPathIndex + indexOffset];
		gce::Vector3f32 toNodePos = pToNode->data->GetPosition();
		if (navMesh->DoesSegmentGoThroughObstacles(pos,toNodePos, radius))
			break;
		
		lastNodePos = toNodePos;
		indexOffset++;
	}

	// temp fix happens when agent is to close to obstacles 
	// should probably modify Astar to adjust the path to agent geometry size 
	if (indexOffset == 1 && m_currentPathIndex + indexOffset < m_path.size())
	{
		Node<NavTile, Agent>* pToNode = m_path[m_currentPathIndex + indexOffset];
		gce::Vector3f32 toNodePos = pToNode->data->GetPosition();
		lastNodePos = toNodePos;
		m_currentLineNodes.push(pToNode);
		m_currentLineTargets.push(ClosestPointOnLine(pos, lastNodePos, toNodePos));
	}

	for (size_t i = 1; i < indexOffset; i++)
	{
		Node<NavTile, Agent>* pToNode = m_path[m_currentPathIndex + i];
		gce::Vector3f32 toNodePos = pToNode->data->GetPosition();
		m_currentLineNodes.push(pToNode);
		m_currentLineTargets.push(ClosestPointOnLine(pos, lastNodePos, toNodePos));
	}

	m_direction = (lastNodePos - transform.GetWorldPosition()).Normalize();
	m_targetForward = lastNodePos - transform.GetWorldPosition();
	m_targetForward.y = 0.f;

	if (m_targetForward.Norm() < 0.0001f)
		return;

	m_targetForward.SelfNormalize();
	m_isRotating = true;
}

void Agent::FollowCurrentLine()
{
	if (!m_hasReleasedTraveled)
		ReleaseTraveledNodes();

	Node<NavTile, Agent>* pToNode = m_currentLineNodes.front();

	if (!AcquireTravelingToNodes(pToNode))
	{
		m_blockedTime += gce::GameManager::DeltaTime();
		return;
	}
	m_blockedTime = 0.f;
	m_pCurrentNode = pToNode;
	m_currentLineNodes.pop();
	m_movingTo = m_currentLineTargets.front();
	m_movingTo.y = transform.GetWorldPosition().y;
	m_distanceToMove = (transform.GetWorldPosition() - m_movingTo).Norm();
	m_currentLineTargets.pop();
	m_isMoving = true;
	m_currentPathIndex++;
	m_hasReleasedTraveled = false;
}

void Agent::MoveToTarget()
{
	gce::Vector3f32 translate = m_direction * m_speed * gce::GameManager::DeltaTime();
	float distanceTraveled = translate.Norm();
	m_distanceToMove -= distanceTraveled;
	if (m_distanceToMove < 0.f)
	{
		transform.SetWorldPosition(m_movingTo);
		m_isMoving = false;
		return;
	}
	transform.WorldTranslate(translate);
}

void Agent::ReleaseTraveledNodes()
{
	m_hasReleasedTraveled = true;
	gce::MeshRenderer* meshRenderer = GetComponent<gce::MeshRenderer>();
	
	if (meshRenderer == nullptr)
		return;

	gce::Vector3f32 pos = m_pCurrentNode->data->GetPosition();
	gce::Vector3f32 min = meshRenderer->pGeometry->min + pos;
	gce::Vector3f32 max = meshRenderer->pGeometry->max + pos;

	std::vector< Node<NavTile, Agent>*> nodesToRelease;

	for (Node<NavTile, Agent>* node : m_nodesOccupied)
	{
		if (!IsNodeInBounds(node,min,max))
			nodesToRelease.push_back(node);
	}

	for (size_t i = 0; i < nodesToRelease.size(); i++)
	{
		Node<NavTile, Agent>* node = nodesToRelease[i];
		node->occupiedByAgent = nullptr;
		m_nodesOccupied.erase(node);
	}
}

bool Agent::AcquireTravelingToNodes(Node<NavTile, Agent>* goToNode)
{
	//return true;
	gce::MeshRenderer* meshRenderer = GetComponent<gce::MeshRenderer>();
	if (meshRenderer == nullptr)
		return true;

	if (m_needToAcquire.empty())
	{
		NavMesh::Instance()->ResetVisited();

		std::queue<Node<NavTile, Agent>*> queue;

		gce::Vector3f32 currentPos = transform.GetWorldPosition();
		gce::Vector3f32 ToNodePos = goToNode->data->GetPosition();
		gce::Vector3f32 minPos = { min(currentPos.x,ToNodePos.x), min(currentPos.y,ToNodePos.y), min(currentPos.z,ToNodePos.z) };
		gce::Vector3f32 maxPos = { max(currentPos.x,ToNodePos.x), max(currentPos.y,ToNodePos.y), max(currentPos.z,ToNodePos.z) };
		gce::Vector3f32 min = minPos + meshRenderer->pGeometry->min;
		gce::Vector3f32 max = maxPos + meshRenderer->pGeometry->max;

		queue.push(goToNode);
		uint32 visitedVersion = NavMesh::Instance()->GetCurrentVisitedVersion();

		while (!queue.empty())
		{
			Node<NavTile, Agent>* testAcquire = queue.front();
			queue.pop();
			if (IsNodeInBounds(testAcquire, min, max))
			{
				m_needToAcquire.push_back(testAcquire);
				for (Node<NavTile, Agent>* neighbor : testAcquire->neighbors)
				{
					if (!(neighbor->visitedVersion == visitedVersion)) {
						neighbor->visitedVersion = visitedVersion;
						queue.push(neighbor);
					}
				}
			}
		}
	}

	for (size_t i = 0; i < m_needToAcquire.size(); i++)
	{
		Node<NavTile, Agent>* node = m_needToAcquire[i];
		if (node->occupiedByAgent != nullptr && node->occupiedByAgent != this)
		{
			m_pBlockedBy = node->occupiedByAgent;
			return false;
		}
	}

	for (size_t i = 0; i < m_needToAcquire.size(); i++)
	{
		Node<NavTile, Agent>* node = m_needToAcquire[i];
		node->occupiedByAgent = this;
		m_nodesOccupied.insert(node);
	}
	m_needToAcquire.clear();
	return true;
}

bool Agent::IsNodeInBounds(Node<NavTile, Agent>* node, gce::Vector3f32 min, gce::Vector3f32 max) // don't check Y axis
{
	return node->data->IsInBounds(min, max);
}

void Agent::FindPath()
{
	m_currentLineNodes = std::queue<Node<NavTile, Agent>*>();
	m_currentLineTargets = std::queue<gce::Vector3f32>();
	m_lastTargetCalculatedPathCoordinates = m_pTarget->transform.GetWorldPosition();
	Node<NavTile, Agent>* pTargetNode = NavMesh::Instance()->GetNearestNodeFromPosition(m_lastTargetCalculatedPathCoordinates);

	if (m_blocked)
		m_path = PathFinder::GetPath({ m_pCurrentNode,pTargetNode }, this);
	else
		m_path = PathFinder::GetPath({ m_pCurrentNode,pTargetNode });

	m_blocked = false;
	m_currentPathIndex = 0;
	m_noPathTime = 0.f;
	m_blocked = 0.f;
	m_needToAcquire.clear();
}

bool Agent::HasTargetMovedTooMuch()
{
	gce::Vector3f32 targetPos = m_pTarget->transform.GetWorldPosition();
	return (m_lastTargetCalculatedPathCoordinates - targetPos).Norm() > m_stopRange;
}

bool Agent::NeedCalculatePath()
{
	if (m_blockedTime > m_baseBlockedTime)
	{
		m_blocked = true;
		m_blockedTime = 0.f;
		m_pBlockedBy->ResetBlockedTime();
		m_pBlockedBy = nullptr;
		return true;
	}
	
	if (m_path.size() < 2)
		m_noPathTime += gce::GameManager::DeltaTime();

	if (m_noPathTime > m_baseNoPathTime)
	{
		m_noPathTime = 0.f;
		return true;
	}

	return HasTargetMovedTooMuch();
}

void Agent::RotateTowardDirection()
{
	gce::Vector3f32 currentForward = transform.GetWorldForward();
	currentForward.y = 0.f;

	if (currentForward.Norm() < 0.0001f)
		return;

	currentForward.SelfNormalize();

	gce::Vector3f32 targetForward = m_targetForward;
	targetForward.y = 0.f;
	targetForward.SelfNormalize();

	float dot = gce::Clamp(currentForward.DotProduct(targetForward), -1.f, 1.f);
	float angle = acosf(dot);

	gce::Vector3f32 cross = currentForward.CrossProduct(targetForward);
	float sign = (cross.y >= 0.f) ? 1.f : -1.f;

	float deltaYaw = angle * sign;

	float maxStep = m_rotationSpeed * gce::GameManager::DeltaTime();
	float step = gce::Clamp(deltaYaw, -maxStep, maxStep);

	transform.WorldRotate({ 0.f, step, 0.f });

	if (fabs(deltaYaw) < 0.001f)
		m_isRotating = false;
}
