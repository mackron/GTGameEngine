
#include <GTEngine/Rendering/RenderCommands/RCDrawVA.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderParameter.hpp>

namespace GTEngine
{
    RCDrawVA::RCDrawVA()
        : va(nullptr), shader(nullptr), parameters()
    {
    }

    RCDrawVA::~RCDrawVA()
    {
    }

    void RCDrawVA::Clear()
    {
        this->va     = nullptr;
        this->shader = nullptr;
        this->parameters.Clear();
    }


    void RCDrawVA::Execute()
    {
        // The first thing to do is activate the shader, if we have one. It's possible that the shader has already been set
        // by a previous command. If this->shader is null, it means that is the case and we don't want to change it.
        if (this->shader != nullptr)
        {
            for (size_t i = 0; i < this->parameters.GetCount(); ++i)
            {
                auto iParamName  = this->parameters.GetNameByIndex(i);
                auto iParamValue = this->parameters.GetByIndex(i);

                this->shader->SetParameter(iParamName, iParamValue);
            }

            Renderer::SetShader(this->shader);
        }


        /*
        // Now properties need to be set on the shader.
        for (size_t i = 0; i < this->parameters.GetCount(); ++i)
        {
            auto iParamName  = this->parameters.GetNameByIndex(i);
            auto iParamValue = this->parameters.GetByIndex(i);

            iParamValue->SetOnCurrentShader(iParamName);
        }
        */
        

        // Now we draw the mesh.
        Renderer::Draw(this->va);
    }
}
