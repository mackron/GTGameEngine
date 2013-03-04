// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/SpriteComponent.hpp>
#include <GTEngine/Texture2DLibrary.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(SpriteComponent, "Sprite")

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
        Texture2DLibrary::Unacquire(this->texture);
        this->texture = Texture2DLibrary::Acquire(filename);

        
        this->OnChanged();

        return this->texture;
    }
}