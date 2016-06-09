// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGE/Assets/SoundAsset.hpp>
#include <GTGE/Audio/SoundStreamer.hpp>

namespace GT
{
    SoundAsset::SoundAsset(const char* absolutePathOrIdentifier, AssetType assetType)
        : Asset(absolutePathOrIdentifier, assetType)
    {
    }

    SoundAsset::~SoundAsset()
    {
    }


    AssetClass SoundAsset::GetClass() const
    {
        return AssetClass_Sound;
    }

    bool SoundAsset::Load(const char* absolutePath, drfs_context* pVFS)
    {
        // For now we are just going to load a streamer and retrieve the information about the buffer. Later on we'll clean this up and do it properly.
        m_dataInfo.pData = drfs_open_and_read_binary_file(pVFS, absolutePath, &m_dataInfo.dataSize);
        if (m_dataInfo.pData != nullptr)
        {
            SoundStreamer streamer(m_dataInfo.pData, m_dataInfo.dataSize);
            if (streamer.Initialize())
            {
                m_dataInfo.format     = streamer.GetFormat();
                m_dataInfo.channels   = streamer.GetNumChannels();
                m_dataInfo.sampleRate = streamer.GetSampleRate();

                return true;
            }
        }

        return false;
    }

    VoiceDesc SoundAsset::GetDataInfo() const
    {
        return m_dataInfo;
    }
}
