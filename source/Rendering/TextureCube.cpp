// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Rendering/TextureCube.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    TextureCube::TextureCube(Texture2D* positiveX, Texture2D* negativeX, Texture2D* positiveY, Texture2D* negativeY, Texture2D* positiveZ, Texture2D* negativeZ)
        : PositiveX(positiveX), NegativeX(negativeX),
          PositiveY(positiveY), NegativeY(negativeY),
          PositiveZ(positiveZ), NegativeZ(negativeZ),
          minFilter(TextureFilter_Linear), magFilter(TextureFilter_Linear), anisotropy(1),
          shaders()
    {
    }

    TextureCube::~TextureCube()
    {
    }



    void TextureCube::SetMinificationFilter(TextureFilter newFilter)
    {
        if (this->minFilter != newFilter)
        {
            this->minFilter = newFilter;
        }
    }

    void TextureCube::SetMagnificationFilter(TextureFilter newFilter)
    {
        if (this->magFilter != newFilter)
        {
            this->magFilter = newFilter;
        }
    }

    TextureFilter TextureCube::GetMinificationFilter() const
    {
        return this->minFilter;
    }

    TextureFilter TextureCube::GetMagnificationFilter() const
    {
        return this->magFilter;
    }


    void TextureCube::SetAnisotropy(unsigned int newAnisotropy)
    {
        if (this->anisotropy != newAnisotropy)
        {
            this->anisotropy = newAnisotropy;
        }
    }

    unsigned int TextureCube::GetAnisotropy() const
    {
        return this->anisotropy;
    }
}
