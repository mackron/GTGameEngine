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
        easyvfs_free(m_dataInfo.pData);
    }


    bool SoundAsset_WAV::Load(const char* absolutePath, easyvfs_context* pVFS)
    {
        // For now we are just going to load a streamer and retrieve the information about the buffer. Later on we'll clean this up and do it properly.
        m_dataInfo.pData = easyvfs_open_and_read_binary_file(pVFS, absolutePath, &m_dataInfo.sizeInBytes);
        if (m_dataInfo.pData != nullptr)
        {
            GTEngine::SoundStreamer_WAV streamer(m_dataInfo.pData, m_dataInfo.sizeInBytes);
            if (streamer.Initialize())
            {
                m_dataInfo.flags         = 0;
                m_dataInfo.format        = streamer.GetFormat();
                m_dataInfo.channels      = streamer.GetNumChannels();
                m_dataInfo.sampleRate    = streamer.GetSampleRate();
                m_dataInfo.bitsPerSample = streamer.GetBitsPerSample();

                return true;
            }
        }

        return false;
    }


    CompressedAudioFormat SoundAsset_WAV::GetCompressedDataFormat() const
    {
        return CompressedAudioFormat::WAV;
    }

    easyaudio_buffer_desc SoundAsset_WAV::GetDataInfo() const
    {
        return m_dataInfo;
    }
}

#endif
