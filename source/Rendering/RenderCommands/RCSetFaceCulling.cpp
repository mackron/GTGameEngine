
#include <GTEngine/Rendering/RenderCommands/RCSetFaceCulling.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    RCSetFaceCulling::RCSetFaceCulling()
        : cullFrontFaces(false), cullBackFaces(true)
    {
    }

    RCSetFaceCulling::~RCSetFaceCulling()
    {
    }

    void RCSetFaceCulling::SetCullingMode(bool cullFrontFacesIn, bool cullBackFacesIn)
    {
        this->cullFrontFaces = cullFrontFacesIn;
        this->cullBackFaces  = cullBackFacesIn;
    }

    void RCSetFaceCulling::Execute()
    {
        Renderer::SetFaceCulling(this->cullFrontFaces, this->cullBackFaces);
    }
}
