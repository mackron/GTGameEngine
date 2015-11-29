// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/ShaderParameterCache.hpp>
#include <GTEngine/ShaderParameter.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTLib/Errors.hpp>

namespace GTEngine
{
    ShaderParameterCache::ShaderParameterCache()
        : floatParameters(),
          float2Parameters(),
          float3Parameters(),
          float4Parameters(),
          float2x2Parameters(),
          float3x3Parameters(),
          float4x4Parameters(),
          texture2DParameters(),
          textureCubeParameters()
    {
    }

    ShaderParameterCache::ShaderParameterCache(const ShaderParameterCache &other)
        : floatParameters(other.floatParameters),
          float2Parameters(other.float2Parameters),
          float3Parameters(other.float3Parameters),
          float4Parameters(other.float4Parameters),
          float2x2Parameters(other.float2x2Parameters),
          float3x3Parameters(other.float3x3Parameters),
          float4x4Parameters(other.float4x4Parameters),
          texture2DParameters(other.texture2DParameters),
          textureCubeParameters(other.textureCubeParameters)
    {
    }

    ShaderParameterCache::~ShaderParameterCache()
    {
    }


    void ShaderParameterCache::Set(const char* name, float value)
    {
        this->floatParameters.Add(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::vec2 &value)
    {
        this->float2Parameters.Add(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::vec3 &value)
    {
        this->float3Parameters.Add(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::vec4 &value)
    {
        this->float4Parameters.Add(name, value);
    }


#if 0
    void ShaderParameterCache::Set(const char* name, int value)
    {
        this->SetGeneric<ShaderParameter_Integer>(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::ivec2 &value)
    {
        this->SetGeneric<ShaderParameter_Integer2>(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::ivec3 &value)
    {
        this->SetGeneric<ShaderParameter_Integer3>(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::ivec4 &value)
    {
        this->SetGeneric<ShaderParameter_Integer4>(name, value);
    }
#endif


    void ShaderParameterCache::Set(const char* name, const glm::mat2 &value)
    {
        this->float2x2Parameters.Add(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::mat3 &value)
    {
        this->float3x3Parameters.Add(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::mat4 &value)
    {
        this->float4x4Parameters.Add(name, value);
    }

    void ShaderParameterCache::Set(const char* name, Texture2D* value)
    {
        this->texture2DParameters.Add(name, value);
    }
    void ShaderParameterCache::Set(const char* name, TextureCube* value)
    {
        this->textureCubeParameters.Add(name, value);
    }



    void ShaderParameterCache::UnsetFloat(const char* name)
    {
        this->floatParameters.RemoveByKey(name);
    }

    void ShaderParameterCache::UnsetFloat2(const char* name)
    {
        this->float2Parameters.RemoveByKey(name);
    }

    void ShaderParameterCache::UnsetFloat3(const char* name)
    {
        this->float3Parameters.RemoveByKey(name);
    }

    void ShaderParameterCache::UnsetFloat4(const char* name)
    {
        this->float4Parameters.RemoveByKey(name);
    }

    void ShaderParameterCache::UnsetFloat2x2(const char* name)
    {
        this->float2x2Parameters.RemoveByKey(name);
    }

    void ShaderParameterCache::UnsetFloat3x3(const char* name)
    {
        this->float3x3Parameters.RemoveByKey(name);
    }

    void ShaderParameterCache::UnsetFloat4x4(const char* name)
    {
        this->float4x4Parameters.RemoveByKey(name);
    }

    void ShaderParameterCache::UnsetTexture2D(const char* name)
    {
        this->texture2DParameters.RemoveByKey(name);
    }

    void ShaderParameterCache::UnsetTextureCube(const char* name)
    {
        this->textureCubeParameters.RemoveByKey(name);
    }



    const ShaderParameter_Float* ShaderParameterCache::GetFloatParameter(const char* name) const
    {
        auto iParameter = this->floatParameters.Find(name);
        if (iParameter != nullptr)
        {
            return &iParameter->value;
        }

        return nullptr;
    }

    const ShaderParameter_Float2* ShaderParameterCache::GetFloat2Parameter(const char* name) const
    {
        auto iParameter = this->float2Parameters.Find(name);
        if (iParameter != nullptr)
        {
            return &iParameter->value;
        }

        return nullptr;
    }

    const ShaderParameter_Float3* ShaderParameterCache::GetFloat3Parameter(const char* name) const
    {
        auto iParameter = this->float3Parameters.Find(name);
        if (iParameter != nullptr)
        {
            return &iParameter->value;
        }

        return nullptr;
    }

    const ShaderParameter_Float4* ShaderParameterCache::GetFloat4Parameter(const char* name) const
    {
        auto iParameter = this->float4Parameters.Find(name);
        if (iParameter != nullptr)
        {
            return &iParameter->value;
        }

        return nullptr;
    }

    const ShaderParameter_Float2x2* ShaderParameterCache::GetFloat2x2Parameter(const char* name) const
    {
        auto iParameter = this->float2x2Parameters.Find(name);
        if (iParameter != nullptr)
        {
            return &iParameter->value;
        }

        return nullptr;
    }

    const ShaderParameter_Float3x3* ShaderParameterCache::GetFloat3x3Parameter(const char* name) const
    {
        auto iParameter = this->float3x3Parameters.Find(name);
        if (iParameter != nullptr)
        {
            return &iParameter->value;
        }

        return nullptr;
    }

    const ShaderParameter_Float4x4* ShaderParameterCache::GetFloat4x4Parameter(const char* name) const
    {
        auto iParameter = this->float4x4Parameters.Find(name);
        if (iParameter != nullptr)
        {
            return &iParameter->value;
        }

        return nullptr;
    }

    const ShaderParameter_Texture2D* ShaderParameterCache::GetTexture2DParameter(const char* name) const
    {
        auto iParameter = this->texture2DParameters.Find(name);
        if (iParameter != nullptr)
        {
            return &iParameter->value;
        }

        return nullptr;
    }

    const ShaderParameter_TextureCube* ShaderParameterCache::GetTextureCubeParameter(const char* name) const
    {
        auto iParameter = this->textureCubeParameters.Find(name);
        if (iParameter != nullptr)
        {
            return &iParameter->value;
        }

        return nullptr;
    }


    void ShaderParameterCache::SetParameters(const ShaderParameterCache &other)
    {
        auto &otherFloatParameters       = other.GetFloatParameters();
        auto &otherFloat2Parameters      = other.GetFloat2Parameters();
        auto &otherFloat3Parameters      = other.GetFloat3Parameters();
        auto &otherFloat4Parameters      = other.GetFloat4Parameters();
        auto &otherFloat2x2Parameters    = other.GetFloat2x2Parameters();
        auto &otherFloat3x3Parameters    = other.GetFloat3x3Parameters();
        auto &otherFloat4x4Parameters    = other.GetFloat4x4Parameters();
        auto &otherTexture2DParameters   = other.GetTexture2DParameters();
        auto &otherTextureCubeParameters = other.GetTextureCubeParameters();


        // Float
        for (size_t i = 0; i < otherFloatParameters.count; ++i)
        {
            this->Set(otherFloatParameters.buffer[i]->key, otherFloatParameters.buffer[i]->value.value);
        }

        // Float2
        for (size_t i = 0; i < otherFloat2Parameters.count; ++i)
        {
            this->Set(otherFloat2Parameters.buffer[i]->key, otherFloat2Parameters.buffer[i]->value.value);
        }

        // Float3
        for (size_t i = 0; i < otherFloat3Parameters.count; ++i)
        {
            this->Set(otherFloat3Parameters.buffer[i]->key, otherFloat3Parameters.buffer[i]->value.value);
        }

        // Float4
        for (size_t i = 0; i < otherFloat4Parameters.count; ++i)
        {
            this->Set(otherFloat4Parameters.buffer[i]->key, otherFloat4Parameters.buffer[i]->value.value);
        }


        // Float2x2
        for (size_t i = 0; i < otherFloat2x2Parameters.count; ++i)
        {
            this->Set(otherFloat2x2Parameters.buffer[i]->key, otherFloat2x2Parameters.buffer[i]->value.value);
        }

        // Float3x3
        for (size_t i = 0; i < otherFloat3x3Parameters.count; ++i)
        {
            this->Set(otherFloat3x3Parameters.buffer[i]->key, otherFloat3x3Parameters.buffer[i]->value.value);
        }

        // Float4x4
        for (size_t i = 0; i < otherFloat4x4Parameters.count; ++i)
        {
            this->Set(otherFloat4x4Parameters.buffer[i]->key, otherFloat4x4Parameters.buffer[i]->value.value);
        }


        // Texture2D
        for (size_t i = 0; i < otherTexture2DParameters.count; ++i)
        {
            this->Set(otherTexture2DParameters.buffer[i]->key, otherTexture2DParameters.buffer[i]->value.value);
        }

        // TextureCube
        for (size_t i = 0; i < otherTextureCubeParameters.count; ++i)
        {
            this->Set(otherTextureCubeParameters.buffer[i]->key, otherTextureCubeParameters.buffer[i]->value.value);
        }
    }

    void ShaderParameterCache::UnsetParameters(const ShaderParameterCache &other)
    {
        auto &otherFloatParameters       = other.GetFloatParameters();
        auto &otherFloat2Parameters      = other.GetFloat2Parameters();
        auto &otherFloat3Parameters      = other.GetFloat3Parameters();
        auto &otherFloat4Parameters      = other.GetFloat4Parameters();
        auto &otherFloat2x2Parameters    = other.GetFloat2x2Parameters();
        auto &otherFloat3x3Parameters    = other.GetFloat3x3Parameters();
        auto &otherFloat4x4Parameters    = other.GetFloat4x4Parameters();
        auto &otherTexture2DParameters   = other.GetTexture2DParameters();
        auto &otherTextureCubeParameters = other.GetTextureCubeParameters();


        // Float
        for (size_t i = 0; i < otherFloatParameters.count; ++i)
        {
            this->UnsetFloat(otherFloatParameters.buffer[i]->key);
        }

        // Float2
        for (size_t i = 0; i < otherFloat2Parameters.count; ++i)
        {
            this->UnsetFloat2(otherFloat2Parameters.buffer[i]->key);
        }

        // Float3
        for (size_t i = 0; i < otherFloat3Parameters.count; ++i)
        {
            this->UnsetFloat3(otherFloat3Parameters.buffer[i]->key);
        }

        // Float4
        for (size_t i = 0; i < otherFloat4Parameters.count; ++i)
        {
            this->UnsetFloat4(otherFloat4Parameters.buffer[i]->key);
        }


        // Float2x2
        for (size_t i = 0; i < otherFloat2x2Parameters.count; ++i)
        {
            this->UnsetFloat2x2(otherFloat2x2Parameters.buffer[i]->key);
        }

        // Float3x3
        for (size_t i = 0; i < otherFloat3x3Parameters.count; ++i)
        {
            this->UnsetFloat3x3(otherFloat3x3Parameters.buffer[i]->key);
        }

        // Float4x4
        for (size_t i = 0; i < otherFloat4x4Parameters.count; ++i)
        {
            this->UnsetFloat4x4(otherFloat4x4Parameters.buffer[i]->key);
        }


        // Texture2D
        for (size_t i = 0; i < otherTexture2DParameters.count; ++i)
        {
            this->UnsetTexture2D(otherTexture2DParameters.buffer[i]->key);
        }

        // TextureCube
        for (size_t i = 0; i < otherTextureCubeParameters.count; ++i)
        {
            this->UnsetTextureCube(otherTextureCubeParameters.buffer[i]->key);
        }
    }


    void ShaderParameterCache::Clear()
    {
        this->floatParameters.Clear();
        this->float2Parameters.Clear();
        this->float3Parameters.Clear();
        this->float4Parameters.Clear();
        this->float2x2Parameters.Clear();
        this->float3x3Parameters.Clear();
        this->float4x4Parameters.Clear();
        this->texture2DParameters.Clear();
        this->textureCubeParameters.Clear();
    }


    void ShaderParameterCache::Serialize(GTLib::Serializer &serializer) const
    {
        // Just the one chunk with this one.
        GTLib::BasicSerializer intermediarySerializer;

        
        // Float
        intermediarySerializer.Write(static_cast<uint32_t>(this->floatParameters.count));

        for (size_t i = 0; i < this->floatParameters.count; ++i)
        {
            auto name  = this->floatParameters.buffer[i]->key;
            auto value = this->floatParameters.buffer[i]->value.value;

            intermediarySerializer.WriteString(name);
            intermediarySerializer.Write(value);
        }

        // Float2
        intermediarySerializer.Write(static_cast<uint32_t>(this->float2Parameters.count));

        for (size_t i = 0; i < this->float2Parameters.count; ++i)
        {
            auto name  = this->float2Parameters.buffer[i]->key;
            auto value = this->float2Parameters.buffer[i]->value.value;

            intermediarySerializer.WriteString(name);
            intermediarySerializer.Write(value);
        }

        // Float3
        intermediarySerializer.Write(static_cast<uint32_t>(this->float3Parameters.count));

        for (size_t i = 0; i < this->float3Parameters.count; ++i)
        {
            auto name  = this->float3Parameters.buffer[i]->key;
            auto value = this->float3Parameters.buffer[i]->value.value;

            intermediarySerializer.WriteString(name);
            intermediarySerializer.Write(value);
        }

        // Float4
        intermediarySerializer.Write(static_cast<uint32_t>(this->float4Parameters.count));

        for (size_t i = 0; i < this->float4Parameters.count; ++i)
        {
            auto name  = this->float4Parameters.buffer[i]->key;
            auto value = this->float4Parameters.buffer[i]->value.value;

            intermediarySerializer.WriteString(name);
            intermediarySerializer.Write(value);
        }


        // Float2x2
        intermediarySerializer.Write(static_cast<uint32_t>(this->float2x2Parameters.count));

        for (size_t i = 0; i < this->float2x2Parameters.count; ++i)
        {
            auto name  = this->float2x2Parameters.buffer[i]->key;
            auto value = this->float2x2Parameters.buffer[i]->value.value;

            intermediarySerializer.WriteString(name);
            intermediarySerializer.Write(value);
        }

        // Float3x3
        intermediarySerializer.Write(static_cast<uint32_t>(this->float3x3Parameters.count));

        for (size_t i = 0; i < this->float3x3Parameters.count; ++i)
        {
            auto name  = this->float3x3Parameters.buffer[i]->key;
            auto value = this->float3x3Parameters.buffer[i]->value.value;

            intermediarySerializer.WriteString(name);
            intermediarySerializer.Write(value);
        }

        // Float4x4
        intermediarySerializer.Write(static_cast<uint32_t>(this->float4x4Parameters.count));

        for (size_t i = 0; i < this->float4x4Parameters.count; ++i)
        {
            auto name  = this->float4x4Parameters.buffer[i]->key;
            auto value = this->float4x4Parameters.buffer[i]->value.value;

            intermediarySerializer.WriteString(name);
            intermediarySerializer.Write(value);
        }


        // Texture2D
        intermediarySerializer.Write(static_cast<uint32_t>(this->texture2DParameters.count));

        for (size_t i = 0; i < this->texture2DParameters.count; ++i)
        {
            auto name  = this->texture2DParameters.buffer[i]->key;
            auto value = this->texture2DParameters.buffer[i]->value.value;

            intermediarySerializer.WriteString(name);

            if (value != nullptr)
            {
                intermediarySerializer.WriteString(value->GetRelativePath());
            }
            else
            {
                intermediarySerializer.WriteString("engine/textures/default.png");
            }
        }


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_ShaderParameters;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void ShaderParameterCache::Deserialize(GTLib::Deserializer &deserializer)
    {
        this->Clear();

        Serialization::ChunkHeader header;
        deserializer.Read(header);

        assert(header.id == Serialization::ChunkID_ShaderParameters);
        {
            switch (header.version)
            {
            case 1:
                {
                    uint32_t count;

                    
                    // Float
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTLib::String name;
                        float          value;

                        deserializer.ReadString(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float2
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTLib::String name;
                        glm::vec2      value;

                        deserializer.ReadString(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float3
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTLib::String name;
                        glm::vec3      value;

                        deserializer.ReadString(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float4
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTLib::String name;
                        glm::vec4      value;

                        deserializer.ReadString(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }


                    // Float2x2
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTLib::String name;
                        glm::mat2      value;

                        deserializer.ReadString(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float3x3
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTLib::String name;
                        glm::mat3      value;

                        deserializer.ReadString(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float4x4
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTLib::String name;
                        glm::mat4      value;

                        deserializer.ReadString(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }


                    // Texture2D
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTLib::String name;
                        GTLib::String value;

                        deserializer.ReadString(name);
                        deserializer.ReadString(value);

                        auto texture = Texture2DLibrary::Acquire(value.c_str());
                        {
                            this->Set(name.c_str(), texture);
                        }
                        Texture2DLibrary::Unacquire(texture);

                        this->Set(name.c_str(), texture);
                    }



                    break;
                }


            default:
                {
                    GT::PostError("Failed to deserialize ShaderParameterCache. Unknown version (%d).", header.version);
                    break;
                }
            }
        }
    }



    ///////////////////////////////////////////
    // Static

    void ShaderParameterCache::Difference(const ShaderParameterCache &parametersA, const ShaderParameterCache &parametersB, ShaderParameterCache &difference)
    {
        auto &floatParametersA       = parametersA.GetFloatParameters();
        auto &float2ParametersA      = parametersA.GetFloat2Parameters();
        auto &float3ParametersA      = parametersA.GetFloat3Parameters();
        auto &float4ParametersA      = parametersA.GetFloat4Parameters();
        auto &float2x2ParametersA    = parametersA.GetFloat2x2Parameters();
        auto &float3x3ParametersA    = parametersA.GetFloat3x3Parameters();
        auto &float4x4ParametersA    = parametersA.GetFloat4x4Parameters();
        auto &texture2DParametersA   = parametersA.GetTexture2DParameters();
        auto &textureCubeParametersA = parametersA.GetTextureCubeParameters();

        auto &floatParametersB       = parametersB.GetFloatParameters();
        auto &float2ParametersB      = parametersB.GetFloat2Parameters();
        auto &float3ParametersB      = parametersB.GetFloat3Parameters();
        auto &float4ParametersB      = parametersB.GetFloat4Parameters();
        auto &float2x2ParametersB    = parametersB.GetFloat2x2Parameters();
        auto &float3x3ParametersB    = parametersB.GetFloat3x3Parameters();
        auto &float4x4ParametersB    = parametersB.GetFloat4x4Parameters();
        auto &texture2DParametersB   = parametersB.GetTexture2DParameters();
        auto &textureCubeParametersB = parametersB.GetTextureCubeParameters();


        // Float
        for (size_t i = 0; i < floatParametersA.count; ++i)
        {
            auto name  = floatParametersA.buffer[i]->key;
            auto value = floatParametersA.buffer[i]->value.value;

            if (!floatParametersB.Exists(name))
            {
                difference.Set(name, value);
            }
        }

        // Float2
        for (size_t i = 0; i < float2ParametersA.count; ++i)
        {
            auto name  = float2ParametersA.buffer[i]->key;
            auto value = float2ParametersA.buffer[i]->value.value;

            if (!float2ParametersB.Exists(name))
            {
                difference.Set(name, value);
            }
        }

        // Float3
        for (size_t i = 0; i < float3ParametersA.count; ++i)
        {
            auto name  = float3ParametersA.buffer[i]->key;
            auto value = float3ParametersA.buffer[i]->value.value;

            if (!float3ParametersB.Exists(name))
            {
                difference.Set(name, value);
            }
        }

        // Float4
        for (size_t i = 0; i < float4ParametersA.count; ++i)
        {
            auto name  = float4ParametersA.buffer[i]->key;
            auto value = float4ParametersA.buffer[i]->value.value;

            if (!float4ParametersB.Exists(name))
            {
                difference.Set(name, value);
            }
        }


        // Float2x2
        for (size_t i = 0; i < float2x2ParametersA.count; ++i)
        {
            auto name  = float2x2ParametersA.buffer[i]->key;
            auto value = float2x2ParametersA.buffer[i]->value.value;

            if (!float2x2ParametersB.Exists(name))
            {
                difference.Set(name, value);
            }
        }

        // Float3x3
        for (size_t i = 0; i < float3x3ParametersA.count; ++i)
        {
            auto name  = float3x3ParametersA.buffer[i]->key;
            auto value = float3x3ParametersA.buffer[i]->value.value;

            if (!float3x3ParametersB.Exists(name))
            {
                difference.Set(name, value);
            }
        }

        // Float4x4
        for (size_t i = 0; i < float4x4ParametersA.count; ++i)
        {
            auto name  = float4x4ParametersA.buffer[i]->key;
            auto value = float4x4ParametersA.buffer[i]->value.value;

            if (!float4x4ParametersB.Exists(name))
            {
                difference.Set(name, value);
            }
        }


        // Texture2D
        for (size_t i = 0; i < texture2DParametersA.count; ++i)
        {
            auto name  = texture2DParametersA.buffer[i]->key;
            auto value = texture2DParametersA.buffer[i]->value.value;

            if (!texture2DParametersB.Exists(name))
            {
                difference.Set(name, value);
            }
        }

        // TextureCube
        for (size_t i = 0; i < textureCubeParametersA.count; ++i)
        {
            auto name  = textureCubeParametersA.buffer[i]->key;
            auto value = textureCubeParametersA.buffer[i]->value.value;

            if (!textureCubeParametersB.Exists(name))
            {
                difference.Set(name, value);
            }
        }
    }
}
