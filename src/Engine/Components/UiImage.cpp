#include "pch.h"
#include "UiImage.h"

namespace gce
{
    void UiImage::InitializeImage(Vector2f32 const& pos, Vector2f32 const& size, float32 const opacity)
    {
        this->size = size;
        position = pos;
        rectImage = new RectanglePosF(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
    }

    void UiImage::SetPosition(Vector2f32 const& position)
    {
        this->position = position;
        SyncRectImage();
    }

    void UiImage::SetSize(Vector2f32 const& size)
    {
        this->size = size;
        SyncRectImage();
    }

    void UiImage::SyncRectImage()
    {
        rectImage->left = position.x;
        rectImage->top = position.y;
        rectImage->right = position.x + size.x;
        rectImage->bottom = position.y + size.y;
    }

} // namespace gce