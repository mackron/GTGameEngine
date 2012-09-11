
#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    Shader::Shader(const char* vertexSource, const char* fragmentSource)
        : vertexSource(vertexSource), fragmentSource(fragmentSource),
          pendingParameters(),
          rendererData(nullptr),
          currentTexture2Ds(), currentTextureCubes()
    {
        Renderer::OnShaderCreated(*this);
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

        Renderer::OnShaderDeleted(*this);
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

    void Shader::OnTextureDeleted(TextureCube* texture)
    {
        // We need to look for the shader in our current shaders and clear to null. We're also going to set the parameter to null so that
        // it will be updated on the renderer in the next binding.
        for (size_t i = 0; i < currentTextureCubes.count; ++i)
        {
            auto iTexture = currentTextureCubes.buffer[i];
            assert(iTexture != nullptr);

            if (iTexture->value == texture)
            {
                iTexture->value = nullptr;
                this->SetParameter(iTexture->key, static_cast<TextureCube*>(nullptr));
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

    void Shader::OnTextureParameterChanged(TextureCube* oldTexture)
    {
        if (oldTexture != nullptr)
        {
            bool referenced = false;

            // We check if the old texture is being used by anything. If not, we let it know that it's no longer being used.
            for (size_t i = 0; i < currentTextureCubes.count; ++i)
            {
                if (currentTextureCubes.buffer[i]->value == oldTexture)
                {
                    referenced = true;
                }
            }

            // If the texture is no longer referenced, we let it know that it is detached from the shader.
            if (!referenced)
            {
                oldTexture->OnDetachFromShader(*this);
            }
        }
    }
}