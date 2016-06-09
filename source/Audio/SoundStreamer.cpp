// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Audio/SoundStreamer.hpp>
#include <GTGE/Assets/SoundAsset.hpp>

namespace GT
{
    SoundStreamer::SoundStreamer(const void* pData, size_t dataSize)
        : m_pData(pData), m_dataSize(dataSize)
    {
    }

    SoundStreamer::~SoundStreamer()
    {
    }


    bool SoundStreamer::Initialize()
    {
        if (!dra_decoder_open_memory(&m_decoder, m_pData, m_dataSize)) {
            return false;
        }

        return true;
    }

    uint64_t SoundStreamer::Read(uint64_t samplesToRead, void* pSamplesOut)
    {
        return dra_decoder_read_f32(&m_decoder, samplesToRead, (float*)pSamplesOut);
    }

    bool SoundStreamer::Seek(uint64_t sample)
    {
        return dra_decoder_seek_to_sample(&m_decoder, sample);
    }

    dra_format SoundStreamer::GetFormat() const
    {
        return dra_format_f32;
    }

    unsigned int SoundStreamer::GetNumChannels() const
    {
        return m_decoder.channels;
    }

    unsigned int SoundStreamer::GetSampleRate() const
    {
        return m_decoder.sampleRate;
    }



    ///////////////////////////////////////////////////////
    //
    // Static Functions
    //
    ///////////////////////////////////////////////////////

    SoundStreamer* SoundStreamer::CreateFromAsset(GT::Asset* pAsset)
    {
        if (pAsset->GetClass() != GT::AssetClass_Sound) {
            return NULL;
        }

        GT::SoundAsset* pSoundAsset = reinterpret_cast<GT::SoundAsset*>(pAsset);
        assert(pSoundAsset != nullptr);

        VoiceDesc dataInfo = pSoundAsset->GetDataInfo();
        return new SoundStreamer(dataInfo.pData, dataInfo.dataSize);
    }

    void SoundStreamer::Delete(SoundStreamer* pStreamer)
    {
        delete pStreamer;
    }
}
