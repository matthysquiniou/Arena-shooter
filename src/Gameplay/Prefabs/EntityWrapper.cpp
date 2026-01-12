#include "EntityWrapper.h"
#include "GameObject.h"
#include "Script.h"
#include "GameManager.h"

EntityWrapper::EntityWrapper() : gce::GameObject()
{
}

EntityWrapper& EntityWrapper::Create()
{
	EntityWrapper* const pNew = new EntityWrapper();
	pNew->m_pScene = &gce::GameManager::GetScene();
	pNew->transform.m_pOwner = pNew;
	gce::GameManager::GetLifespanSystem().m_toCreate.gameObjects.Push(pNew);
	return *pNew;
}

EntityWrapper& EntityWrapper::SetProperties(const char* name, std::vector<Tag> tags, gce::Vector3f32 pos, gce::Vector3f32 rotation, gce::Vector3f32 scale)
{
	SetName(name);
	AddTags(tags);
	transform.SetWorldPosition(pos);
	transform.SetWorldRotation(rotation);
	transform.SetWorldScale(scale);

	return *this;
}

EntityWrapper& EntityWrapper::SetChildProperties(EntityWrapper& parent, const char* name, std::vector<Tag> tags, gce::Vector3f32 pos, gce::Vector3f32 rotation, gce::Vector3f32 scale)
{
	parent.AddChild(*this);

	SetName(name);
	AddTags(tags);
	transform.SetLocalPosition(pos);
	transform.SetLocalRotation(rotation);
	transform.SetLocalScale(scale);

	return *this;
}

gce::MeshRenderer* EntityWrapper::AddMeshRenderer(gce::Geometry* pGeo, const char* albedoPath, const char* roughnessPath, const char* metalnessPath, const char* normalPath)
{
	gce::MeshRenderer* component = AddComponent<gce::MeshRenderer>();

	component->pGeometry = pGeo;

	if (albedoPath != "")
	{
		gce::Texture* pAlbedo = gce::Texture::GetTexture(albedoPath);

		component->pMaterial->albedoTextureID = pAlbedo->GetTextureID();
		m_pDefaultTexture = pAlbedo;
	}
	else
	{
		gce::Texture* pWhite = gce::GameManager::s_pInstance->m_pWhiteTexture;
		component->pMaterial->albedoTextureID = pWhite->GetTextureID();
		m_pDefaultTexture = pWhite;
	}

	component->pMaterial->useTextureAlbedo = 1;

	if (roughnessPath != "")
	{
		gce::Texture* pRoughness = gce::Texture::GetTexture(roughnessPath);
		component->pMaterial->roughnessTextureID = pRoughness->GetTextureID();
		component->pMaterial->useTextureRoughness = 1;
	}

	if (metalnessPath != "")
	{
		gce::Texture* pMetalness = gce::Texture::GetTexture(metalnessPath);
		component->pMaterial->metalnessTextureID = pMetalness->GetTextureID();
		component->pMaterial->useTextureMetalness = 1;
	}

	if (normalPath != "")
	{
		gce::Texture* pNormal = gce::Texture::GetTexture(normalPath);
		component->pMaterial->normalTextureID = pNormal->GetTextureID();
		component->pMaterial->useTextureNormal = 1;
	}

	component->pMaterial->subsurface = 1;
	component->pPso = gce::GameManager::GetSceneManager().GetPSO();

	return component;
}

gce::PhysicComponent* EntityWrapper::AddPhysics(float32 mass, float32 gravityScale, float32 bounciness)
{
	gce::PhysicComponent* component = AddComponent<gce::PhysicComponent>();

	component->SetMass(mass);
	component->SetGravityScale(gravityScale);
	component->SetBounciness(bounciness);

	return component;
}

gce::TextRenderer* EntityWrapper::AddStaticTextRenderer(std::wstring txt, gce::RectanglePosF dimensions, gce::Color txtColor, gce::Vector2f32 scale, std::wstring fontName)
{
	gce::TextRenderer* component = AddComponent<gce::TextRenderer>();

	component->pFont = gce::Font::GetFont(fontName);
	component->pBrush = new gce::ColorBrush(txtColor);

	std::wstring* pTxt = new std::wstring(txt);
	component->text = *pTxt;
	component->rectPosF = new gce::RectanglePosF(dimensions);
	component->scale = scale;

	return component;
}

gce::TextRenderer* EntityWrapper::AddDynamicTextRenderer(std::wstring& txt, gce::RectanglePosF dimensions, gce::Color txtColor, gce::Vector2f32 scale, std::wstring fontName)
{
	gce::TextRenderer* component = AddComponent<gce::TextRenderer>();

	component->pFont = gce::Font::GetFont(fontName);
	component->pBrush = new gce::ColorBrush(txtColor);
	component->text = txt;
	component->rectPosF = new gce::RectanglePosF(dimensions);
	component->scale = scale;

	return component;
}

void EntityWrapper::UpdateDynamicText(std::wstring& referenceTxt)
{
	gce::TextRenderer* component = GetComponent<gce::TextRenderer>();

	if (component == nullptr)
		return;

	component->text = referenceTxt;
}

gce::UIButton* EntityWrapper::AddUIButton(gce::Vector2f32 pos, gce::Vector2f32 rotation, gce::Vector2f32 scale, const char* textureBrushPath, const char* textureHoverBrushPath)
{
	transform.SetWorldPosition({pos.x, pos.y, 0});
	transform.SetWorldRotation({rotation.x, rotation.y, 0});
	transform.SetWorldScale({scale.x, scale.y, 0});

	gce::UIButton* component = AddComponent<gce::UIButton>();

	if (textureBrushPath == "")
	{
		component->pBitMapBrush = new gce::BitMapBrush("res/Assets/white.png");
		component->pHoverBitMapBrush = component->pBitMapBrush;
	}
	else
	{
		auto bitmap = new gce::BitMapBrush(textureBrushPath);
		component->pBitMapBrush = bitmap;

		if (textureHoverBrushPath == "")
		{
			component->pHoverBitMapBrush = component->pBitMapBrush;
		}
		else
			component->pHoverBitMapBrush = new gce::BitMapBrush(textureHoverBrushPath);
	}

	return component;
}

gce::UiImage* EntityWrapper::AddUiImage(const char* imgPath, gce::Vector2f32 leftTopRectPos, gce::Vector2f32 imgSize, gce::Vector2f32 pos, gce::Vector2f32 scale, float32 rotation)
{
	auto component = AddComponent<gce::UiImage>();

	component->InitializeImage(leftTopRectPos, imgSize, 1.f);
	component->btmBrush = new gce::BitMapBrush(imgPath);

	component->btmBrush->SetTransformMatrix({ pos.x, pos.y, 0 }, { scale.x, scale.y, 1 }, rotation);
	component->SetActive(true);
	return nullptr;
}

void EntityWrapper::SetUiImageTransform(gce::Vector2f32 pos, gce::Vector2f32 scale, float32 rotation)
{
	auto component = GetComponent<gce::UiImage>();

	if (component == nullptr)
		return;

	component->btmBrush->SetTransformMatrix({ pos.x, pos.y, 0 }, { scale.x, scale.y, 1 }, rotation);
}

///////////////////////////////////////////////////////////////////
