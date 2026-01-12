#pragma once
#include <vector>
#include <unordered_map>
#include "Tags.h"
#include "KeyBinds.h"
#include <Audio.h>

class Player;
class ArenaCamera;
class EntityWrapper;
class InventoryManager;
class UIManager;

namespace gce
{
	class D12PipelineObject;
}

enum SceneType
{
	MenuScene,
	GamePlayScene,
	GameOverScene,
	WinScene,
	Count
};

namespace gce {
	class Scene;
	class GameObject;
}

constexpr int FULLSCORE = 1000;

class SceneManager
{
	std::vector<gce::GameObject*> m_Map; // All the objects imported by the json
	EntityWrapper* m_pEmpty = nullptr; // Need to handle the SceneManager Script
	UIManager* m_pUIManager = nullptr; // Handle all the 2D objects

	Player* m_pPlayer = nullptr;
	ArenaCamera* m_pArenaCam = nullptr;

	std::unordered_map<SceneType, std::vector<gce::GameObject*>> m_SceneObjectsList;
	SceneType m_currentSceneType = SceneType::Count;

	InventoryManager* m_pInventoryManager = nullptr;

	gce::D12PipelineObject* m_pPso = nullptr;

	bool m_fullScreen = true;
public:
	SceneManager() = default;
	~SceneManager() { delete m_pPso; }

	void Init();
	void InitGamePlay();
	void UnInitGamePlay();
	bool m_IsGamePlayInit = false;

	void ChangeScene(SceneType newType);
	void AddSound(const char* name, std::wstring path, gce::Category category, int volume);
	void LoadSounds();
	void LinkObjectToScene(gce::GameObject* obj, SceneType scene);

	gce::GameObject* GetFirstGameObject(std::vector<Tag> tags); // return the first GameObject with the all the tags
	std::vector<gce::GameObject*> GetAllGameObjects(std::vector<Tag> tags); // return all GameObjects with all the tags

	InventoryManager* GetInventoryManager() { return m_pInventoryManager; }
	gce::D12PipelineObject* GetPSO() { return m_pPso; }

	ArenaCamera* GetArenaCamera() { return m_pArenaCam; }
	gce::GameObject* GetCameraObject();
	const SceneType& GetSceneType() const { return m_currentSceneType; }
	UIManager* GetUIManager() { return m_pUIManager; }

	const bool& GetFullScreenMode() const { return m_fullScreen; }
	void SetFullScreenMode(bool state);
	void ToggleFullScreenMode();
};

