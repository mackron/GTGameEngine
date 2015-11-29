// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Audio/SoundStreamer.hpp>
#include <GTEngine/Assets/SoundAsset.hpp>

#include "Streamers/SoundStreamer_WAV.hpp"
#include "Streamers/SoundStreamer_Vorbis.hpp"

namespace GT
{
    SoundStreamer::SoundStreamer()
    {
    }

    SoundStreamer::~SoundStreamer()
    {
    }



    ///////////////////////////////////////////////////////
    //
    // Static Functions
    //
    ///////////////////////////////////////////////////////

    SoundStreamer* SoundStreamer::CreateFromAsset(GT::Asset* pAsset)
    {
        SoundStreamer* pStreamer = nullptr;

        if (pAsset->GetClass() == GT::AssetClass_Sound)
        {
            GT::SoundAsset* pSoundAsset = reinterpret_cast<GT::SoundAsset*>(pAsset);
            assert(pSoundAsset != nullptr);

            easyaudio_buffer_desc dataInfo = pSoundAsset->GetDataInfo();


            // We need to create a streamer based on the compressed format of the audio data.
            switch (pSoundAsset->GetCompressedDataFormat())
            {
            case GT::CompressedAudioFormat::WAV:    pStreamer = new SoundStreamer_WAV(dataInfo.pData, dataInfo.sizeInBytes);      break;
            case GT::CompressedAudioFormat::Vorbis: pStreamer = new GT::SoundStreamer_Vorbis(dataInfo.pData, dataInfo.sizeInBytes); break;

            case GT::CompressedAudioFormat::FLAC:
            case GT::CompressedAudioFormat::Unknown:
            default: break;
            }
        }

        return pStreamer;
    }

    void SoundStreamer::Delete(SoundStreamer* pStreamer)
    {
        delete pStreamer;
    }
}
