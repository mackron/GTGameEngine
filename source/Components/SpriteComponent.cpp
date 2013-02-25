// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/SpriteComponent.hpp>

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
        delete this->texture;
        this->texture = new Texture2D(filename, filename);

        this->OnChanged();

        return this->texture;
    }
}