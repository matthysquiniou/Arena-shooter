#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"
#include "Agent.h"

using namespace gce;

DECLARE_SCRIPT(AgentBehavior, ScriptFlag::Update)

//Members
Agent* pAgent = nullptr;



void Init()
{
	pAgent = dynamic_cast<Agent*>(m_pOwner);
}

void Update()
{
	if (pAgent == nullptr)
	{
		Init();
		return;
	}

	pAgent->FollowPathToTarget();
}

END_SCRIPT
