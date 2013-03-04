// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    Shader::Shader(const char* vertexSourceIn, const char* fragmentSourceIn, const char* geometrySourceIn)
        : vertexSource(vertexSourceIn), fragmentSource(fragmentSourceIn), geometrySource(geometrySourceIn),
          pendingParameters(),
          currentTextures()
    {
    }

    Shader::~Shader()
    {
        this->ClearPendingParameters();

        // We need to let any textures know that they are no longer attached to this shader.
        for (size_t i = 0; i < this->currentTextures.count; ++i)
        {
            auto iTexture = this->currentTextures.buffer[i];
            assert(iTexture != nullptr);

            auto &attachment = iTexture->value;

            if (attachment.texture != nullptr)
            {
                switch (attachment.type)
                {
                //case ShaderParameterType_Texture1D:   static_cast<Texture1D*  >(attachment.texture)->OnDetachFromShader(*this); break;
                case ShaderParameterType_Texture2D:   static_cast<Texture2D*  >(attachment.texture)->OnDetachFromShader(*this); break;
                //case ShaderParameterType_Texture3D:   static_cast<Texture3D*  >(attachment.texture)->OnDetachFromShader(*this); break;
                case ShaderParameterType_TextureCube: static_cast<TextureCube*>(attachment.texture)->OnDetachFromShader(*this); break;


                case ShaderParameterType_Float:
                case ShaderParameterType_Float2:
                case ShaderParameterType_Float3:
                case ShaderParameterType_Float4:
                case ShaderParameterType_Float2x2:
                case ShaderParameterType_Float3x3:
                case ShaderParameterType_Float4x4:
                case ShaderParameterType_Texture1D:
                case ShaderParameterType_Texture3D:
                default: break;
                }
            }
        }
    }

    

    void Shader::ClearPendingParameters()
    {
        this->pendingParameters.Clear();
    }


    void Shader::OnTextureDeleted(Texture2D* texture)
    {
        // We need to look for the shader in our current shaders and clear to null. We're also going to set the parameter to null so that
        // it will be updated on the renderer in the next binding.
        for (size_t i = 0; i < currentTextures.count; ++i)
        {
            auto iTexture = currentTextures.buffer[i];
            assert(iTexture != nullptr);

            auto &attachment = iTexture->value;

            if (attachment.texture == texture)
            {
                attachment.texture = nullptr;
                this->SetParameter(iTexture->key, static_cast<Texture2D*>(nullptr));
            }
        }
    }

    void Shader::OnTextureDeleted(TextureCube* texture)
    {
        // We need to look for the shader in our current shaders and clear to null. We're also going to set the parameter to null so that
        // it will be updated on the renderer in the next binding.
        for (size_t i = 0; i < currentTextures.count; ++i)
        {
            auto iTexture = currentTextures.buffer[i];
            assert(iTexture != nullptr);

            auto &attachment = iTexture->value;

            if (attachment.texture == texture)
            {
                attachment.texture = nullptr;
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
            for (size_t i = 0; i < currentTextures.count; ++i)
            {
                if (currentTextures.buffer[i]->value.texture == oldTexture)
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

    void Shader::OnTextureParameterChanged(TextureCube* oldTexture)
    {
        if (oldTexture != nullptr)
        {
            bool referenced = false;

            // We check if the old texture is being used by anything. If not, we let it know that it's no longer being used.
            for (size_t i = 0; i < currentTextures.count; ++i)
            {
                if (currentTextures.buffer[i]->value.texture == oldTexture)
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
