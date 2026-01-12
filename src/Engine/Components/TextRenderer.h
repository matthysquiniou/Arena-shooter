#ifndef ENGINE_TEXT_RENDERER_H_INCLUDED
#define ENGINE_TEXT_RENDERER_H_INCLUDED

#include "define.h"
#include "UIBase.h"
#include <Structs.h>
#include <Font.h>

namespace gce {

DECLARE_COMPONENT( TextRenderer, UiBase )
{
	WStringView text;
	Font* pFont;
	UiBrush* pBrush;
	RectanglePosF* rectPosF;
	gce::Vector2f32 scale = { 1, 1 };
protected:
	void Init() override;

	friend class UiSystem;
};


}

#endif