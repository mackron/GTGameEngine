// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SpriteComponent_hpp_
#define __GTEngine_SpriteComponent_hpp_

#include "../Component.hpp"
#include "../Rendering/Texture2D.hpp"

namespace GTEngine
{
    class SpriteComponent : public Component
    {
    public:

        /// Constructor.
        SpriteComponent(SceneNode &node);

        /// Destructor.
        ~SpriteComponent();

        /// Sets the texture to use with the sprite.
        Texture2D* SetTexture(const char* filename);

        /// Retrieves the texture of the sprite.
        Texture2D* GetTexture() { return this->texture; }

    private:

        /// The texture of the sprite.
        Texture2D *texture;


        GTENGINE_DECL_COMPONENT_ATTRIBS(SpriteComponent)
    };
}

#endif