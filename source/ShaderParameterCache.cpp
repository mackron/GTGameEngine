
#include <GTEngine/ShaderParameterCache.hpp>
#include <GTEngine/ShaderParameter.hpp>

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
}
