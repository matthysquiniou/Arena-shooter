#pragma once
#include "GameObject.h"

class ArenaCamera
{
	gce::Camera* m_pCamera = nullptr;
	gce::GameObject* m_pGameObject = nullptr;
public:
	gce::GameObject* Create();

	void SetParams(float32 fov, float32 nearplane, float32 farplane, float32 aspectRatio);

	gce::GameObject* GetGameObject() { return m_pGameObject; }
	
	template<typename TScript>
	TScript* GetScript();
};

template<typename TScript>
inline TScript* ArenaCamera::GetScript()
{
	return m_pGameObject->GetScript<TScript>();
}

