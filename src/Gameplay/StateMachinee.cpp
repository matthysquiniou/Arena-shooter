#include "StateMachinee.h"
#include "GameManager.h"

void StateMachinee::Update()
{
	auto itConditions = m_stateConditionMap.find(m_currentState);
	if (itConditions != m_stateConditionMap.end())
	{
		std::vector<std::pair<Conditione*, State>> conditiones = itConditions->second;
		for (std::pair<Conditione*, State> condition : conditiones)
		{
			if (condition.first->Get())
			{
				m_currentState = condition.second;
				auto itAction = m_stateActionMap.find(m_currentState);
				if (itAction != m_stateActionMap.end())
				{
					for (Actione* action : itAction->second)
						action->Reset();
				}
				break;
			}
		}
	}

	float deltaTime = gce::GameManager::DeltaTime();

	auto itAction = m_stateActionMap.find(m_currentState);
	if (itAction != m_stateActionMap.end())
	{
		for (Actione* action : itAction->second)
			action->Update(deltaTime);
	}
}

void StateMachinee::SetState(State state)
{
	m_currentState = state;
}

void StateMachinee::AddStateCondition(State beginState, State endState, Conditione& condition)
{
	m_stateConditionMap[beginState].push_back({ &condition, endState });
}

void StateMachinee::AddStateAction(State state, Actione& action)
{
	m_stateActionMap[state].push_back(&action);
}
