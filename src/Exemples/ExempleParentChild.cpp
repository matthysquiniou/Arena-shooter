#include "Exemple.h"

#include <Render.h>
#include <Engine.h>

#include <Script.h>
using namespace gce;

void Exemple::ParentChild(Scene* pScene)
{
	D12PipelineObject* defaultPso = new D12PipelineObject(
		SHADERS.VERTEX,
		SHADERS.PIXEL,
		SHADERS.HULL,
		SHADERS.DOMAIN_,
		SHADERS.ROOT_SIGNATURE
	);

	GameObject& parentObject = GameObject::Create(*pScene);
	MeshRenderer* pMeshRenderer = parentObject.AddComponent<MeshRenderer>();
	pMeshRenderer->pGeometry = SHAPES.CYLINDER;
	pMeshRenderer->pPso = defaultPso;

	GameObject& childObject = GameObject::Create(*pScene);
	MeshRenderer* pMeshRendererChild = childObject.AddComponent<MeshRenderer>();
	pMeshRendererChild->pGeometry = SHAPES.CAPSULE;
	pMeshRendererChild->pPso = defaultPso;

	GameObject& childObject2 = GameObject::Create(*pScene);
	MeshRenderer* pMeshRendererChild2 = childObject2.AddComponent<MeshRenderer>();
	pMeshRendererChild2->pGeometry = SHAPES.DONUT;
	pMeshRendererChild2->pPso = defaultPso;

	// Set le parentObject en tant que parent de childObject
	parentObject.AddChild(childObject);
	parentObject.AddChild(childObject2);

	// lorsque qu'un gameObject devient l'enfant d'un autre gameObject leur transform est lié : 
	// Exemple : 
	// La position du parent sera en 0.0f ,-2.0f,0.0f et la position de l'enfant sera en 2.0f,-2.0f,0.0f si l'on modifie sa position locale
	parentObject.transform.SetLocalPosition({ 0.0f,-2.0f,0.0f });
	childObject.transform.SetLocalPosition({ 2.0f,0.0f,0.0f });
	// Si on modifie directement la position World cela ne prends pas en compte la position du parent
	childObject2.transform.SetWorldPosition({ 2.0f,0.0f,0.0f });

	// Si l'on modifie la position du parent tous les enfants se déplacent
	//parentObject.transform.LocalTranslate({ 2.0f,0.0f,0.0f });


}