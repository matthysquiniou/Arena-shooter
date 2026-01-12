#include "UiBar.h"
#include "EntityWrapper.h"
#include <algorithm>

void UiBar::InitFrame(const char* imgPath, gce::Vector2f32 imgDimensions, gce::Vector2f32 pos, gce::Vector2f32 scale, gce::Vector2f32 rotation)
{
	auto& obj = m_frame.first;
	obj = &EntityWrapper::Create();

	obj->AddUIButton(pos, rotation, imgDimensions * scale, imgPath, imgPath);

	m_frame.second = TransformData(pos, imgDimensions * scale, rotation);
}

void UiBar::InitFilledBar1(const char* imgPath, gce::Vector2f32 imgDimensions, gce::Vector2f32 pos, gce::Vector2f32 scale, gce::Vector2f32 rotation)
{
	auto& obj = m_filledBar1.first;
	obj = &EntityWrapper::Create();

	obj->AddUIButton(pos, rotation, imgDimensions * scale, imgPath, imgPath);

	m_filledBar1.second = TransformData(pos, imgDimensions * scale, rotation);
}

void UiBar::InitFilledBar2(const char* imgPath, gce::Vector2f32 imgDimensions, gce::Vector2f32 pos, gce::Vector2f32 scale, gce::Vector2f32 rotation)
{
	auto& obj = m_filledBar2.first;
	obj = &EntityWrapper::Create();

	obj->AddUIButton(pos, rotation, imgDimensions * scale, imgPath, imgPath);

	m_filledBar2.second = TransformData(pos, imgDimensions * scale, rotation);
}

void UiBar::InitEmptyBar(const char* imgPath, gce::Vector2f32 imgDimensions, gce::Vector2f32 pos, gce::Vector2f32 scale, gce::Vector2f32 rotation)
{
	auto& obj = m_emptyBar.first;
	obj = &EntityWrapper::Create();
	
	obj->AddUIButton(pos, rotation, imgDimensions * scale, imgPath, imgPath);

	m_emptyBar.second = TransformData(pos, imgDimensions * scale, rotation);
}

void UiBar::SetFilledBar1ByRatio(float current, float max, bool invert)
{
	auto obj = m_filledBar1.first;
	auto data = m_filledBar1.second;

	if (obj == nullptr || obj->IsActive() == false)
		return;

	if (max == 0)
		max = INT_MAX;

	float ratio = std::clamp(current / max, 0.f, 1.f);

	if (invert == true)
	{
		ratio = 1 - ratio;
	}

	float newScaleX = data.scale.x * ratio;
	float deltaScale = (data.scale.x - newScaleX) * 0.5f;

	obj->transform.SetWorldScale({ newScaleX, data.scale.y, 1 });
	obj->transform.SetWorldPosition({ data.pos.x - deltaScale, data.pos.y, 0});
}

void UiBar::SetFilledBar2ByRatio(float current, float max, bool invert)
{
	auto obj = m_filledBar2.first;
	auto data = m_filledBar2.second;

	if (obj == nullptr || obj->IsActive() == false)
		return;

	if (max == 0)
		max = INT_MAX;

	float ratio = std::clamp(current / max, 0.f, 1.f);

	if (invert == true)
	{
		ratio = 1 - ratio;
	}

	float newScaleX = data.scale.x * ratio;
	float deltaScale = (data.scale.x - newScaleX) * 0.5f;

	obj->transform.SetWorldScale({ newScaleX, data.scale.y, 1 });
	obj->transform.SetWorldPosition({ data.pos.x - deltaScale, data.pos.y, 0 });
}

void UiBar::SetActive(bool state)
{
	if (m_emptyBar.first)
		m_emptyBar.first->SetActive(state);

	if (m_filledBar1.first)
		m_filledBar1.first->SetActive(state);

	if (m_filledBar2.first)
		m_filledBar2.first->SetActive(state);

	if (m_frame.first)
		m_frame.first->SetActive(state);
}