
#include <GTEngine/Rendering/TextureCube.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    TextureCube::TextureCube()
        : PositiveX(), NegativeX(),
          PositiveY(), NegativeY(),
          PositiveZ(), NegativeZ()
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
        delete this->PositiveX;
        delete this->NegativeX;
        delete this->PositiveY;
        delete this->NegativeY;
        delete this->PositiveZ;
        delete this->NegativeZ;

        Renderer::OnTextureCubeDeleted(*this);
    }
}
