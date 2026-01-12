#include <Console.h>
#include "Exemple.h"

// INCLUDES NECESSAIRES DE BASE !!!

#include <windows.h>
#include <Render.h>
#include <Engine.h>

// Exemple Initialisation de Base

// le namespace est utilisé dans tout le GC-Engine
// si vous n'utilisez pas using il faudra faire gce:: devant tous les élements du GC-Engine
// vous avez le choix 
using namespace gce;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	//// Initialisation du Moteur : 
	GameManager::Create();

	// Console utilisée pour le debug
	Console::Init();

	// Creation d'un scene
	Scene& scene1 = Scene::Create();

	//// Creation d'un objet pour la caméra
	GameObject& cameraObject = GameObject::Create(scene1);
	cameraObject.transform.LocalTranslate({ 0, 0, -10 });
	// ajout du component Camera sur l'objet puis initialisation des propriétés de la camera
	Light* pLight = cameraObject.AddComponent<Light>();
	pLight->DefaultDirectionLight();
	Camera* pCamera = cameraObject.AddComponent<Camera>();


	pCamera->SetMainCamera();
	pCamera->SetType(PERSPECTIVE);
	// Fov de base ici pi/4
	pCamera->perspective.fov = XM_PIDIV4;
	// point le plus proche que la camera verra
	pCamera->perspective.nearPlane = 0.001f; 
	// point le plus éloigné que la camera verra
	pCamera->perspective.farPlane = 500.0f; 
	// aspect ratio set en fonction de la largeur et hauteur de la fenêtre
	pCamera->perspective.aspectRatio = 1000.0f/800.0f;
	// ici on définis que l'axe Y va gérer la hauteur 
	pCamera->perspective.up = { 0.0f, 1.0f, 0.0f };

	//// Creation du pso de base pour un objet SANS TEXTURE !!!!
	D12PipelineObject defaultPso(
		SHADERS.VERTEX,
		SHADERS.PIXEL,
		SHADERS.HULL,
		SHADERS.DOMAIN_,
		SHADERS.ROOT_SIGNATURE
	);


	Texture* pNewTexture = new Texture("res/Texture/Plane Base Color.png");

	// creation d'un objet avec geometry custom
	GameObject& shapeCustom = GameObject::Create(scene1);
	shapeCustom.transform.SetWorldPosition({ 0.0f,3.0f,0.0f });
	MeshRenderer* pMeshRenderer3 = shapeCustom.AddComponent<MeshRenderer>();
	// Pensez à inverser l'axe Y des uv quand vous loadez des OBJ (sinon ça casse vos textures)
	pMeshRenderer3->pGeometry = GeometryFactory::GetCustomGeometry("res/Exemple/testplane.obj");
	pMeshRenderer3->pPso = &defaultPso;
	pMeshRenderer3->pMaterial->albedoTextureID = pNewTexture->GetTextureID();
	pMeshRenderer3->pMaterial->useTextureAlbedo = 1;




	// Propeiétés de la fenêtre
	WindowParam windowParam;
	windowParam.title = L"Window Name";
	windowParam.width = 1000;
	windowParam.height = 800;

	GameManager::Run(windowParam);


	// Pour pas surcharger le main je vais ajouter plusieurs exemples ici :

	// Destruction du GameManager
	GameManager::Destroy();

	return 0;
}

