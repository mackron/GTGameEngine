// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ShaderParameterCache.hpp>
#include <GTEngine/ShaderParameter.hpp>
#include <GTEngine/Texture2DLibrary.hpp>

namespace GTEngine
{
    ShaderParameterCache::ShaderParameterCache()
        : parameters()
    {
    }

    ShaderParameterCache::~ShaderParameterCache()
    {
        for (size_t i = 0; i < this->parameters.count; ++i)
        {
            delete this->parameters.buffer[i]->value;
        }
    }


    void ShaderParameterCache::Set(const char* name, float value)
    {
        this->SetGeneric<ShaderParameter_Float>(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::vec2 &value)
    {
        this->SetGeneric<ShaderParameter_Float2>(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::vec3 &value)
    {
        this->SetGeneric<ShaderParameter_Float3>(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::vec4 &value)
    {
        this->SetGeneric<ShaderParameter_Float4>(name, value);
    }


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


    void ShaderParameterCache::Set(const char* name, const glm::mat2 &value)
    {
        this->SetGeneric<ShaderParameter_Float2x2>(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::mat3 &value)
    {
        this->SetGeneric<ShaderParameter_Float3x3>(name, value);
    }
    void ShaderParameterCache::Set(const char* name, const glm::mat4 &value)
    {
        this->SetGeneric<ShaderParameter_Float4x4>(name, value);
    }

    void ShaderParameterCache::Set(const char* name, Texture2D* value)
    {
        this->SetGeneric<ShaderParameter_Texture2D>(name, value);
    }
    void ShaderParameterCache::Set(const char* name, TextureCube* value)
    {
        this->SetGeneric<ShaderParameter_TextureCube>(name, value);
    }


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


    void ShaderParameterCache::Clear()
    {
        for (size_t i = 0; i < this->parameters.count; ++i)
        {
            delete this->parameters.buffer[i]->value;
        }

        this->parameters.Clear();
    }


    void ShaderParameterCache::Serialize(GTCore::Serializer &serializer) const
    {
        // Just the one chunk with this one.
        GTCore::BasicSerializer intermediarySerializer;

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
                    if (reinterpret_cast<ShaderParameter_Texture2D*>(parameter)->value->GetLinkedFileName() != nullptr)
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

                        this->Set(name.c_str(), Texture2DLibrary::Acquire(value.c_str()));

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

                        break;
                    }


                default:
                    break;
                }
            }
        }
    }
}
