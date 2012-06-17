
#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    Shader::Shader(const char* vertexSource, const char* fragmentSource)
        : vertexSource(vertexSource), fragmentSource(fragmentSource),
          pendingParameters(),
          rendererData(nullptr),
          currentTexture2Ds(),
          vertexAttributeLocations(),
          needsRelink(false)
    {
        // TODO: Should control this with a pre-processor macro for when we do non-GLSL shaders.
        this->SetStandardGLSLVertexAttributeLocations();
    }

    Shader::~Shader()
    {
        this->ClearPendingParameters();

        // We need to let any textures know that they are no longer attached to this shader.
        for (size_t i = 0; i < this->currentTexture2Ds.count; ++i)
        {
            auto iTexture = this->currentTexture2Ds.buffer[i];
            assert(iTexture != nullptr);

            if (iTexture->value != nullptr)
            {
                iTexture->value->OnDetachFromShader(this);
            }
        }

        Renderer::MarkForCollection(this);
    }

    void Shader::SetVertexAttributeLocation(const char* name, unsigned int location)
    {
        this->needsRelink = true;
        this->vertexAttributeLocations.Add(name, location);
    }

    void Shader::SetStandardGLSLVertexAttributeLocations()
    {
        this->SetVertexAttributeLocation("VertexInput_Position",  0);
        this->SetVertexAttributeLocation("VertexInput_TexCoord",  1);
        this->SetVertexAttributeLocation("VertexInput_Normal",    2);
        this->SetVertexAttributeLocation("VertexInput_Tangent",   3);
        this->SetVertexAttributeLocation("VertexInput_Bitangent", 4);
        this->SetVertexAttributeLocation("VertexInput_Colour",    5);
    }



    void Shader::ClearPendingParameters()
    {
        this->pendingParameters.Clear();
    }


    void Shader::OnTextureDeleted(Texture2D* texture)
    {
        // We need to look for the shader in our current shaders and clear to null. We're also going to set the parameter to null so that
        // it will be updated on the renderer in the next binding.
        for (size_t i = 0; i < currentTexture2Ds.count; ++i)
        {
            auto iTexture = currentTexture2Ds.buffer[i];
            assert(iTexture != nullptr);

            if (iTexture->value == texture)
            {
                iTexture->value = nullptr;
                this->SetParameter(iTexture->key, static_cast<Texture2D*>(nullptr));
            }
        }
    }

    void Shader::OnTextureParameterChanged(Texture2D* oldTexture)
    {
        if (oldTexture != nullptr)
        {
            bool referenced = false;

            // We check if the old texture is being used by anything. If not, we let it know that it's no longer being used.
            for (size_t i = 0; i < currentTexture2Ds.count; ++i)
            {
                if (currentTexture2Ds.buffer[i]->value == oldTexture)
                {
                    referenced = true;
                }
            }

            // If the texture is no longer referenced, we let it know that it is detached from the shader.
            if (!referenced)
            {
                oldTexture->OnDetachFromShader(this);
            }
        }
    }

    void Shader::OnLink()
    {
        this->needsRelink = false;
    }
}