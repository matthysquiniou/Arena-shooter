#pragma once
#include "GameObject.h"
#include "define.h"
#include "GameTransform.h"
#include "Components.h"
#include "Maths/Vector3.h"

class EntityWrapper : public gce::GameObject
{
public:
    EntityWrapper();

    static EntityWrapper& Create();
    EntityWrapper& SetProperties(const char* name, std::vector<Tag> tags = {}, gce::Vector3f32 pos = {0, 0, 0}, gce::Vector3f32 rotation = {0, 0, 0}, gce::Vector3f32 scale = {1, 1, 1});
    EntityWrapper& SetChildProperties(EntityWrapper& parent, const char* name, std::vector<Tag> tags = {}, gce::Vector3f32 pos = {0, 0, 0}, gce::Vector3f32 rotation = {0, 0, 0}, gce::Vector3f32 scale = {1, 1, 1});
    gce::MeshRenderer* AddMeshRenderer(gce::Geometry* pGeo,
        const char* albedoPath = "", const char* roughnessPath = "",
        const char* metalnessPath = "", const char* normalPath = "");

    gce::PhysicComponent* AddPhysics(float32 mass, float32 gravityScale, float32 bounciness);

    gce::TextRenderer* AddStaticTextRenderer(std::wstring txt, gce::RectanglePosF dimensions = { 0, 0, 200, 200 }, gce::Color txtColor = gce::Color::Black, gce::Vector2f32 scale = { 1, 1 }, std::wstring fontName = L""); // The text won't change
    gce::TextRenderer* AddDynamicTextRenderer(std::wstring& txt, gce::RectanglePosF dimensions = {0, 0, 200, 200}, gce::Color txtColor = gce::Color::Black, gce::Vector2f32 scale = { 1, 1 }, std::wstring fontName = L""); // The text can be changed
    void UpdateDynamicText(std::wstring& referenceTxt);

    gce::UIButton* AddUIButton(gce::Vector2f32 pos, gce::Vector2f32 rotation, gce::Vector2f32 scale, const char* textureBrushPath = "", const char* textureHoverBrushPath = "");
    gce::UiImage* AddUiImage(const char* imgPath, gce::Vector2f32 leftTopRectPos, gce::Vector2f32 imgDimensions, gce::Vector2f32 pos = {0, 0}, gce::Vector2f32 scale = {1, 1}, float32 rotation = 0.f);
    void SetUiImageTransform(gce::Vector2f32 pos, gce::Vector2f32 scale, float32 rotation);
};