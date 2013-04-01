// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ShaderParameterCache.hpp>
#include <GTEngine/ShaderParameter.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/Errors.hpp>

namespace GTEngine
{
    ShaderParameterCache::ShaderParameterCache()
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


    /*
    void ShaderParameterCache::Set(const char* name, const ShaderParameter* parameter)
    {
        // This overload is a special case. We need to create a copy here and set it manually, making sure we delete the old value if any.
        auto iParam = this->parameters.Find(name);
        if (iParam != nullptr)
        {
            delete iParam->value;
        }

        this->parameters.Add(name, CopyShaderParameter(parameter));
    }
    */
    
    /*
    void ShaderParameterCache::Unset(const char* name)
    {
        this->UnsetFloat(name);
        this->UnsetFloat2(name);
        this->UnsetFloat3(name);
        this->UnsetFloat4(name);
        this->UnsetFloat2x2(name);
        this->UnsetFloat3x3(name);
        this->UnsetFloat4x4(name);
        this->UnsetTexture2D(name);
        this->UnsetTextureCube(name);
    }
    */

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



#if 0
    ShaderParameter* ShaderParameterCache::Get(const char* name) const
    {
        return const_cast<ShaderParameterCache*>(this)->Get(name);
    }

    ShaderParameter* ShaderParameterCache::Get(const char* name)
    {
        auto iParam = this->parameters.Find(name);
        if (iParam != nullptr)
        {
            return iParam->value;
        }

        return nullptr;

    }

    ShaderParameter* ShaderParameterCache::GetByIndex(size_t index) const
    {
        return const_cast<ShaderParameterCache*>(this)->GetByIndex(index);
    }

    ShaderParameter* ShaderParameterCache::GetByIndex(size_t index)
    {
        assert(index < this->parameters.count);
        assert(this->parameters.buffer[index] != nullptr);

        return this->parameters.buffer[index]->value;
    }

    const char* ShaderParameterCache::GetNameByIndex(size_t index) const
    {
        assert(index < this->parameters.count);
        assert(this->parameters.buffer[index] != nullptr);

        return this->parameters.buffer[index]->key;
    }

    size_t ShaderParameterCache::GetCount() const
    {
        return this->parameters.count;
    }
#endif


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


    void ShaderParameterCache::Serialize(GTCore::Serializer &serializer) const
    {
        // Just the one chunk with this one.
        GTCore::BasicSerializer intermediarySerializer;

        
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
            intermediarySerializer.WriteString(value->GetRelativePath());
        }


#if 0
        intermediarySerializer.Write(static_cast<uint32_t>(this->parameters.count));

        for (size_t i = 0; i < this->parameters.count; ++i)
        {
            auto name      = this->parameters.buffer[i]->key;
            auto parameter = this->parameters.buffer[i]->value;


            intermediarySerializer.Write(static_cast<uint32_t>(parameter->type));
            intermediarySerializer.WriteString(name);

            switch (parameter->type)
            {
            case ShaderParameterType_Float:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Float*>(parameter)->value);
                    break;
                }

            case ShaderParameterType_Float2:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Float2*>(parameter)->value);
                    break;
                }

            case ShaderParameterType_Float3:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Float3*>(parameter)->value);
                    break;
                }

            case ShaderParameterType_Float4:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Float4*>(parameter)->value);
                    break;
                }


#if 0
            case ShaderParameterType_Integer:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Integer*>(parameter)->value);
                    break;
                }

            case ShaderParameterType_Integer2:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Integer2*>(parameter)->value);
                    break;
                }

            case ShaderParameterType_Integer3:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Integer3*>(parameter)->value);
                    break;
                }

            case ShaderParameterType_Integer4:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Integer4*>(parameter)->value);
                    break;
                }
#endif


            case ShaderParameterType_Float2x2:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Float2x2*>(parameter)->value);
                    break;
                }

            case ShaderParameterType_Float3x3:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Float3x3*>(parameter)->value);
                    break;
                }

            case ShaderParameterType_Float4x4:
                {
                    intermediarySerializer.Write(reinterpret_cast<ShaderParameter_Float4x4*>(parameter)->value);
                    break;
                }


            case ShaderParameterType_Texture2D:
                {
                    if (reinterpret_cast<ShaderParameter_Texture2D*>(parameter)->value->GetRelativePath() != nullptr)
                    {
                        intermediarySerializer.WriteString(reinterpret_cast<ShaderParameter_Texture2D*>(parameter)->value->GetRelativePath());
                    }

                    break;
                }


            case ShaderParameterType_Texture1D:
            case ShaderParameterType_Texture3D:
            case ShaderParameterType_TextureCube:
            default:
                break;
            }
        }
#endif


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_ShaderParameters;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void ShaderParameterCache::Deserialize(GTCore::Deserializer &deserializer)
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
                        GTCore::String name;
                        float          value;

                        deserializer.Read(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float2
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTCore::String name;
                        glm::vec2      value;

                        deserializer.Read(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float3
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTCore::String name;
                        glm::vec3      value;

                        deserializer.Read(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float4
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTCore::String name;
                        glm::vec4      value;

                        deserializer.Read(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }


                    // Float2x2
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTCore::String name;
                        glm::mat2      value;

                        deserializer.Read(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float3x3
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTCore::String name;
                        glm::mat3      value;

                        deserializer.Read(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }

                    // Float4x4
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTCore::String name;
                        glm::mat4      value;

                        deserializer.Read(name);
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);
                    }


                    // Texture2D
                    deserializer.Read(count);

                    for (size_t i = 0; i < count; ++i)
                    {
                        GTCore::String name;
                        GTCore::String value;

                        deserializer.Read(name);
                        deserializer.Read(value);

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
                    GTEngine::PostError("Failed to deserialize ShaderParameterCache. Unknown version (%d).", header.version);
                    break;
                }
            }


#if 0
            uint32_t count;
            deserializer.Read(count);

            for (uint32_t i = 0; i < count; ++i)
            {
                uint32_t type;
                deserializer.Read(type);

                GTCore::String name;
                deserializer.Read(name);

                switch (type)
                {
                case ShaderParameterType_Float:
                    {
                        float value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }

                case ShaderParameterType_Float2:
                    {
                        glm::vec2 value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }

                case ShaderParameterType_Float3:
                    {
                        glm::vec3 value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }

                case ShaderParameterType_Float4:
                    {
                        glm::vec4 value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }


#if 0
                case ShaderParameterType_Integer:
                    {
                        int value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }

                case ShaderParameterType_Integer2:
                    {
                        glm::ivec2 value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }

                case ShaderParameterType_Integer3:
                    {
                        glm::ivec3 value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }

                case ShaderParameterType_Integer4:
                    {
                        glm::ivec4 value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }
#endif


                case ShaderParameterType_Float2x2:
                    {
                        glm::mat2 value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }

                case ShaderParameterType_Float3x3:
                    {
                        glm::mat3 value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }

                case ShaderParameterType_Float4x4:
                    {
                        glm::mat4 value;
                        deserializer.Read(value);

                        this->Set(name.c_str(), value);

                        break;
                    }


                case ShaderParameterType_Texture2D:
                    {
                        GTCore::String value;
                        deserializer.Read(value);

                        auto texture = Texture2DLibrary::Acquire(value.c_str());
                        {
                            this->Set(name.c_str(), texture);
                        }
                        Texture2DLibrary::Unacquire(texture);

                        /*
                        // Because we acquire a new texture here, we need to make it so it is unacquired when this parameter is destructed.
                        auto iParameter = this->parameters.Find(name.c_str());
                        if (iParameter != nullptr)
                        {
                            assert(iParameter->value       != nullptr);
                            assert(iParameter->value->type == ShaderParameterType_Texture2D);
                            {
                                reinterpret_cast<ShaderParameter_Texture2D*>(iParameter->value)->unacquireInDtor = true;
                            }
                        }
                        */

                        break;
                    }


                default:
                    break;
                }
            }
#endif
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
