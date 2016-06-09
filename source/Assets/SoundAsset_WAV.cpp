// Copyright (C) 2015 David Reid. See included LICENCE file.

#include "SoundAsset_WAV.hpp"

#if defined(GT_BUILD_WAV)
#include "../Audio/Streamers/SoundStreamer_WAV.hpp"

namespace GT
{
    SoundAsset_WAV::SoundAsset_WAV(const char* absolutePathOrIdentifier, AssetType assetType)
        : SoundAsset(absolutePathOrIdentifier, assetType)
    {
    }

    SoundAsset_WAV::~SoundAsset_WAV()
    {
        drfs_free(m_pData);
    }


    bool SoundAsset_WAV::Load(const char* absolutePath, drfs_context* pVFS)
    {
        // For now we are just going to load a streamer and retrieve the information about the buffer. Later on we'll clean this up and do it properly.
        m_pData = drfs_open_and_read_binary_file(pVFS, absolutePath, &m_dataSize);
        if (m_pData != nullptr)
        {
            SoundStreamer_WAV streamer(m_pData, m_dataSize);
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


    CompressedAudioFormat SoundAsset_WAV::GetCompressedDataFormat() const
    {
        return CompressedAudioFormat::WAV;
    }

    VoiceDesc SoundAsset_WAV::GetDataInfo() const
    {
        return m_dataInfo;
    }
}

#endif
