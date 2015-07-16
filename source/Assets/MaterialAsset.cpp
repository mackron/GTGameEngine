// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/MaterialAsset.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    /////////////////////////////////////
    // MaterialAssetVariable

    MaterialAssetVariable::MaterialAssetVariable(MaterialVariableType type, const char* name)
        : m_type(type), m_name()
    {
        // Name.
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_name)[i] = 0;
        }

        for (int i = 0; i < 31 && name[i] != '\0'; ++i)
        {
            m_name[i] = name[i];
        }
    }

    MaterialVariableType MaterialAssetVariable::GetType() const
    {
        return m_type;
    }

    const char* MaterialAssetVariable::GetName() const
    {
        return m_name;
    }


    // Float.
    float MaterialAssetVariable_Float::GetValue() const
    {
        return m_value;
    }

    void MaterialAssetVariable_Float::SetValue(float value)
    {
        m_value = value;
    }

    
    // Float2.
    



    /////////////////////////////////////
    // MaterialChannel

    MaterialChannel::MaterialChannel(const char* name, const char* shaderCode)
        : m_name(), m_shaderCode(shaderCode)
    {
        // Name.
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_name)[i] = 0;
        }

        for (int i = 0; i < 31 && name[i] != '\0'; ++i)
        {
            m_name[i] = name[i];
        }
    }


    void MaterialChannel::SetName(const char* name)
    {
        // Name.
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_name)[i] = 0;
        }

        for (int i = 0; i < 31 && name[i] != '\0'; ++i)
        {
            m_name[i] = name[i];
        }
    }

    const char* MaterialChannel::GetName() const
    {
        return m_name;
    }


    void MaterialChannel::SetShaderCode(const char* shaderCode)
    {
        m_shaderCode = shaderCode;
    }

    const char* MaterialChannel::GetShaderCode() const
    {
        return m_shaderCode.c_str();
    }




    /////////////////////////////////////
    // MaterialAsset

    MaterialAsset::MaterialAsset(AssetType assetType)
        : Asset(assetType),
          m_channels()
    {
    }

    MaterialAsset::~MaterialAsset()
    {
        for (size_t iChannel = 0; iChannel < m_channels.GetCount(); ++iChannel)
        {
            delete m_channels[iChannel];
        }
    }


    AssetClass MaterialAsset::GetClass() const
    {
        return AssetClass_Material;
    }


    void MaterialAsset::AddChannel(const char* name, const char* shaderCode)
    {
        // Create the new chunk.
        MaterialChannel* pNewChannel = new MaterialChannel(name, shaderCode);


        // Check if a chunk of the same name already exists.
        for (size_t iChannel = 0; iChannel < m_channels.GetCount(); ++iChannel)
        {
            auto pChannel = m_channels[iChannel];
            assert(pChannel != nullptr);
            {
                if (strcmp(name, pChannel->GetName()) == 0)
                {
                    // A chunk of the same name exists - replace it.
                    delete pChannel;
                    m_channels[iChannel] = pNewChannel;

                    return;
                }
            }
        }

        // If we get here it means a chunk of the same name does not exist. In this case we just push it to the back.
        m_channels.PushBack(pNewChannel);
    }

    void MaterialAsset::RemoveChannel(const char* name)
    {
        for (size_t iChannel = 0; iChannel < m_channels.GetCount(); ++iChannel)
        {
            auto pChannel = m_channels[iChannel];
            assert(pChannel != nullptr);
            {
                if (strcmp(name, pChannel->GetName()) == 0)
                {
                    // We found the chunk.
                    delete pChannel;
                    m_channels.Remove(iChannel);

                    return;
                }
            }
        }
    }

    void MaterialAsset::SetChannelShaderCode(const char* name, const char* shaderCode)
    {
        for (size_t iChannel = 0; iChannel < m_channels.GetCount(); ++iChannel)
        {
            auto pChannel = m_channels[iChannel];
            assert(pChannel != nullptr);
            {
                if (strcmp(name, pChannel->GetName()) == 0)
                {
                    // We found the chunk.
                    pChannel->SetShaderCode(shaderCode);
                }
            }
        }
    }

    const char* MaterialAsset::GetChannelShaderCode(const char* name) const
    {
        for (size_t iChannel = 0; iChannel < m_channels.GetCount(); ++iChannel)
        {
            auto pChannel = m_channels[iChannel];
            assert(pChannel != nullptr);
            {
                if (strcmp(name, pChannel->GetName()) == 0)
                {
                    // We found the chunk.
                    return pChannel->GetShaderCode();
                }
            }
        }

        // The chunk doesn't exist.
        return nullptr;
    }


    bool MaterialAsset::SetChannelByteCodeData(const char* name, size_t sizeInBytes, const void* pData)
    {
        (void)name;
        (void)sizeInBytes;
        (void)pData;

        return false;
    }

    size_t MaterialAsset::GetChannelByteCodeSizeInBytes(const char* name) const
    {
        (void)name;

        return 0;
    }

    void MaterialAsset::GetChannelByteCodeData(const char* name, void* pDataOut) const
    {
        (void)name;
        (void)pDataOut;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
