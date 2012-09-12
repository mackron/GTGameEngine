
#include <GTEngine/Rendering/TextureCube.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    TextureCube::TextureCube()
        : PositiveX(), NegativeX(),
          PositiveY(), NegativeY(),
          PositiveZ(), NegativeZ(),
          rendererData(nullptr), shaders()
    {
        Renderer::OnTextureCubeCreated(*this);

        this->PositiveX = new Texture2D(Texture2DTarget_Cube_PositiveX);
        this->NegativeX = new Texture2D(Texture2DTarget_Cube_NegativeX);
        this->PositiveY = new Texture2D(Texture2DTarget_Cube_PositiveY);
        this->NegativeY = new Texture2D(Texture2DTarget_Cube_NegativeY);
        this->PositiveZ = new Texture2D(Texture2DTarget_Cube_PositiveZ);
        this->NegativeZ = new Texture2D(Texture2DTarget_Cube_NegativeZ);
    }

    TextureCube::~TextureCube()
    {
        // This will detach this texture from any shaders that it's currently attached to.
        while (this->shaders.root != nullptr)
        {
            this->shaders.root->value->OnTextureDeleted(this);
            this->shaders.RemoveRoot();
        }

        delete this->PositiveX;
        delete this->NegativeX;
        delete this->PositiveY;
        delete this->NegativeY;
        delete this->PositiveZ;
        delete this->NegativeZ;

        Renderer::OnTextureCubeDeleted(*this);
    }


    void TextureCube::OnAttachToShader(Shader &shader)
    {
        if (this->shaders.Find(&shader) == nullptr)
        {
            this->shaders.Append(&shader);
        }
    }

    void TextureCube::OnDetachFromShader(Shader &shader)
    {
        this->shaders.Remove(this->shaders.Find(&shader));
    }
}
