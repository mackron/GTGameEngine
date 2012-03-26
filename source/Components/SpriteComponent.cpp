
#include <GTEngine/Components/SpriteComponent.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(SpriteComponent, "Sprite")

    SpriteComponent::SpriteComponent(SceneNode &node)
        : Component(node), texture(nullptr)
    {
    }

    SpriteComponent::~SpriteComponent()
    {
        delete this->texture;
    }

    Texture2D* SpriteComponent::SetTexture(const char* filename)
    {
        delete this->texture;
        this->texture = new Texture2D(filename);

        return this->texture;
    }
}