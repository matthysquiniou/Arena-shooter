#include <Console.h>
#include "SceneManager.h"
#include <windows.h>
#include <Render.h>
#include <Engine.h>

#include "Prefabs/InventoryManager.h"

#include "Prefabs/ArenaCamera.h"
#include "Prefabs/Player.h"

#include "Prefabs/EntityWrapper.h"
#include "Prefabs/UIManager.h"

#include "Scripts/SceneManagerBehavior.hpp"

#include "Scripts/AgentBehavior.hpp"
#include "Scripts/WaveManagerBehavior.hpp"
#include "Scripts/HealSpawnerBehavior.hpp"
#include "Scripts/DragonBehavior.hpp"
#include "Prefabs/BonusManager.h"
#include "Prefabs/BulletPool.h"

#include "CollectibleMunitionManager.h"
#include "Prefabs/ScoreManager.h"

#include "Utils.h"
#include "Agent.h"
#include <ranges>

void SceneManager::InitGamePlay()
{
	if (m_IsGamePlayInit == true)
		return;
	
	m_IsGamePlayInit = true;

	for (gce::GameObject* go : m_Map)
	{
		go->SetActive(true);
	}

	if (m_pPlayer == nullptr)
	{
		m_pPlayer = new Player();
		m_pPlayer->Create();
	}
	else
	{
		m_pPlayer->Reset();
	}

	float camOffsetY = m_pPlayer->GetGameObject()->transform.GetWorldScale().y * 0.5f;
	GetCameraObject()->transform.SetLocalPosition({ 0, camOffsetY, 0 });

	m_pInventoryManager->InitAll();
	m_pInventoryManager->InitInventoryState();

	EntityWrapper& entityWrapper = EntityWrapper::Create();
	entityWrapper.AddScript<WaveManagerBehavior>();
	LinkObjectToScene(&entityWrapper, SceneType::GamePlayScene);

	ScoreManager::Set(FULLSCORE);
}

void SceneManager::UnInitGamePlay()
{
	m_IsGamePlayInit = false;

	for (gce::GameObject* go : m_pInventoryManager->GetCurrentInventory())
	{
		m_pArenaCam->GetGameObject()->RemoveChild(*go);
	}

	m_pInventoryManager->ResetAll();

	m_pPlayer->GetGameObject()->RemoveChild(*GetCameraObject());

	for (gce::GameObject* go : m_SceneObjectsList[SceneType::GamePlayScene])
	{
		if (go->HasTags({ Tag::TMapObject }) == false)
			go->Destroy();
	}	

	m_SceneObjectsList[SceneType::GamePlayScene].clear();

	m_pPlayer->GetGameObject()->RemoveChild(*m_pArenaCam->GetGameObject());

	m_pPlayer->GetGameObject()->SetActive(false);

	BulletPool::DesactivateAllBullets();
	WaveManager::GetInstance()->Reset();
	NavMesh::Instance()->ResetObstacles();

	for (gce::GameObject* go : m_Map)
	{
		if (go->HasTags({ Tag::TSpawner, Tag::THeal }))
			go->GetScript<HealSpawnerBehavior>()->DesactiveAll();

		go->SetActive(false);
	}
	CollectibleMunitionManager::Reset();
}

void SceneManager::Init()
{
	//Keys
	KeyBinds::InitDefaultKeyBinds();

	//sounds
	LoadSounds();

	//PSO
	m_pPso = new gce::D12PipelineObject(
		gce::SHADERS.VERTEX,
		gce::SHADERS.PIXEL,
		gce::SHADERS.HULL,
		gce::SHADERS.DOMAIN_,
		gce::SHADERS.ROOT_SIGNATURE
	);

	gce::Scene& scene = gce::Scene::Create();
	
	// LIGHT
	gce::LightManager::SetLightsProperties(8.0f, 100.0f, 2.0f, 32.0f, 1.f);
	gce::LightData directionalLight = gce::LightManager::CreateDirectionalLight(gce::Vector3f32(0.0f, -1.f, 0.f), gce::Vector4(1.0f, 1.0f, 1.0f, 1.0f), 3.f, 3.f);
	gce::LightManager::AddLight(directionalLight);

	//CAM
	m_pArenaCam = new ArenaCamera();
	m_pArenaCam->Create();
	m_pArenaCam->SetParams(gce::PI / 4, 0.1f, 100.f, 1000.0f / 800.0f);

	//Scene Manager Behavior
	m_pEmpty = &EntityWrapper::Create();
	m_pEmpty->SetProperties("SceneManager Object", { Tag::TMiscellaneous });
	m_pEmpty->AddScript<SceneManagerBehavior>();

	//MAP
	for (gce::GameObject* go : ImportBlenderScene(L"scene_v2.json"))
	{
		go->AddTags({ Tag::TMapObject });
		go->SetActive(false);
		m_Map.push_back(go);
	}

	//INVENTORY
	m_pInventoryManager = new InventoryManager();

	//BulletPool
	BulletPool::Init();

	//UI
	m_pUIManager = new UIManager();
	m_pUIManager->Init();
	
	ChangeScene(SceneType::MenuScene);
}

void SceneManager::ChangeScene(SceneType newType)
{
	if (m_currentSceneType == newType)
		return;

	switch (m_currentSceneType)
	{
	case SceneType::GamePlayScene:
		UnInitGamePlay();
		break;
	default:
		for (gce::GameObject* go : m_SceneObjectsList[m_currentSceneType])
		{
			go->SetActive(false);
		}
	}

	switch (newType)
	{
	case SceneType::GamePlayScene:
		m_IsGamePlayInit = false;
		InitGamePlay();
		break;
	default:
		for (gce::GameObject* go : m_SceneObjectsList[newType])
		{
			go->SetActive(true);
		}
	}

	m_currentSceneType = newType;
}

void SceneManager::AddSound(const char* name, std::wstring path, gce::Category category, int volume)
{
	std::wstring soundPath = WRES_PATH + path;
	AudioUse::LoadSound(name, soundPath.c_str());
	AudioUse::SetVolumeOfSound(name, volume);
	AudioUse::SetAudioCategory(name, category);
}

void SceneManager::LoadSounds()
{
	AudioUse::SetMasterVolume(100); // Volume g�n�ral : maximum 2^24

	AddSound("game_theme", L"res/Audio/game_theme.wav", gce::Category::MUSIC, 100);
	AddSound("boss_theme", L"res/Audio/boss_theme.wav", gce::Category::MUSIC, 100);

	AddSound("woosh", L"res/Audio/woosh.mp3", gce::Category::SFX, 50);
	AddSound("reloaded", L"res/Audio/reloaded.mp3", gce::Category::SFX, 50);
	AddSound("gun", L"res/Audio/gun.wav", gce::Category::SFX, 15);
	AddSound("riffle", L"res/Audio/riffle.wav", gce::Category::SFX, 15);
	AddSound("shotgun", L"res/Audio/shotgun.wav", gce::Category::SFX, 100);
	AddSound("spear_attack", L"res/Audio/spear_attack.mp3", gce::Category::SFX, 50);
	AddSound("spear_hit", L"res/Audio/spear_hit.mp3", gce::Category::SFX, 100);

	AddSound("ammo_pickup", L"res/Audio/ammo_pickup.wav", gce::Category::SFX, 50);
	AddSound("out_of_ammo", L"res/Audio/out_of_ammo.wav", gce::Category::SFX, 100);

	AddSound("dash", L"res/Audio/dash.wav", gce::Category::SFX, 400);

	AddSound("ux_button", L"res/Audio/ux_button.wav", gce::Category::SFX, 100);

	AddSound("mogwai_hitplayer", L"res/Audio/mogwai_hitplayer.wav", gce::Category::SFX, 50);
	AddSound("mogwai_death", L"res/Audio/mogwai_death.wav", gce::Category::SFX, 50);
	AddSound("knife_sharpen", L"res/Audio/knife_sharpen.wav", gce::Category::SFX, 100);
	AddSound("knife_hit", L"res/Audio/knife_hit.wav", gce::Category::SFX, 100);

	AddSound("jiangshi_hitplayer", L"res/Audio/jiangshi_hitplayer.wav", gce::Category::SFX, 50);
	AddSound("jiangshi_death", L"res/Audio/jiangshi_death.wav", gce::Category::SFX, 50);

	AddSound("harpy_attack", L"res/Audio/harpy_attack.wav", gce::Category::SFX, 20);
	AddSound("harpy_hitplayer", L"res/Audio/harpy_hitplayer.wav", gce::Category::SFX, 50);
	AddSound("harpy_death", L"res/Audio/harpy_death.wav", gce::Category::SFX, 50);

	AddSound("dragon_roar", L"res/Audio/dragon_roar.wav", gce::Category::SFX, 35);
	AddSound("dragon_wander", L"res/Audio/dragon_wander.wav", gce::Category::SFX, 75);
	AddSound("fireball", L"res/Audio/fireball.wav", gce::Category::SFX, 40);
}

void SceneManager::LinkObjectToScene(gce::GameObject* obj, SceneType scene)
{
	m_SceneObjectsList[scene].push_back(obj);
}

gce::GameObject* SceneManager::GetFirstGameObject(std::vector<Tag> tags)
{
	auto& gameObjects = GameManager::GetScene().m_gameObjects;

	for (GameObject* pGameObject : gameObjects | std::views::values)
	{
		if (pGameObject->HasTags(tags))
			return pGameObject;
	}

	return nullptr;
}

std::vector<gce::GameObject*> SceneManager::GetAllGameObjects(std::vector<Tag> tags)
{
	auto& gameObjects = GameManager::GetScene().m_gameObjects;

	std::vector<gce::GameObject*> finalTab;

	for (GameObject* pGameObject : gameObjects | std::views::values)
	{
		if (pGameObject->HasTags(tags))
			finalTab.push_back(pGameObject);
	}

	return finalTab;
}

gce::GameObject* SceneManager::GetCameraObject()
{
	return m_pArenaCam->GetGameObject();
}

void SceneManager::SetFullScreenMode(bool state)
{
	if (m_fullScreen == state)
		return;

	m_fullScreen = state;

	auto win = GameManager::GetWindow();

	if (m_fullScreen == false)
	{
		win->SetFullScreen(gce::FullScreenMode::WINDOWED);
	}
	else
	{
		win->SetFullScreen(gce::FullScreenMode::BORDERLESS);
	}
}

void SceneManager::ToggleFullScreenMode()
{
	m_fullScreen = !m_fullScreen;

	auto win = GameManager::GetWindow();

	if (m_fullScreen == false)
	{
		win->SetFullScreen(gce::FullScreenMode::WINDOWED);
	}
	else
	{
		win->SetFullScreen(gce::FullScreenMode::BORDERLESS);
	}
}


