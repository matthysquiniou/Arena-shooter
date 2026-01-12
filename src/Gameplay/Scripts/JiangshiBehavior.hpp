#pragma once
#include <Render.h>
#include <Engine.h>
#include <Script.h>
#include <algorithm>
#include "Components.h"
#include "Agent.h"
#include "EnemyScriptHelper.h"

using namespace gce;

DECLARE_SCRIPT(JiangshiBehavior, ScriptFlag::Update)

void Init()
{
	Agent* pAgent = dynamic_cast<Agent*>(m_pOwner);
	m_initTried = true;
	if (pAgent == nullptr)
		return;

	m_esh.m_pAgent = pAgent;
	m_esh.InitJiangshi();
}


void Reset()
{
	m_esh.Reset();
}


void Update()
{
	if (!m_initTried)
		Init();
	m_esh.Update();
}

EnemyScriptHelper m_esh;
bool m_initTried = false;

END_SCRIPT
