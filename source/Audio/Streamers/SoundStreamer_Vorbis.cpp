// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#pragma warning(push)
#pragma warning(disable: 4100)
#pragma warning(disable: 4127)
#pragma warning(disable: 4244)
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_INTEGER_CONVERSION        // <-- Not specifying this causes some compiler errors...
#include "../../stb/stb_vorbis.c"
#pragma warning(pop)

#include "SoundStreamer_Vorbis.hpp"

namespace GT
{
    namespace Engine
    {
        static const size_t ChunkSampleCount = 8192;       // <-- This controls the size of each streaming chunk. Larger values mean more memory usage, but less chance of stuttering.

        SoundStreamer_Vorbis::SoundStreamer_Vorbis(const void* fileData, size_t fileDataSizeInBytes)
            : SoundStreamer(),
              m_fileDataPtr(fileData), m_fileDataSizeInBytes(fileDataSizeInBytes),
              m_vorbis(nullptr), m_vorbisInfo(),
              m_nextChunkData(nullptr)
        {
        }

        SoundStreamer_Vorbis::~SoundStreamer_Vorbis()
        {
            free(m_nextChunkData);
        }

        bool SoundStreamer_Vorbis::Initialize()
        {
            int error;
            m_vorbis = stb_vorbis_open_memory(reinterpret_cast<const unsigned char*>(m_fileDataPtr), static_cast<int>(m_fileDataSizeInBytes), &error, nullptr);
            if (m_vorbis != nullptr)
            {
                m_vorbisInfo = stb_vorbis_get_info(m_vorbis);
                m_nextChunkData = reinterpret_cast<float*>(malloc(m_vorbisInfo.channels * ChunkSampleCount * sizeof(float)));

                return true;
            }
            else
            {
                // We failed top create the Vorbis streamer.
                return false;
            }
        }


        const void* SoundStreamer_Vorbis::ReadNextChunk(size_t &dataSizeOut)
        {
            dataSizeOut = stb_vorbis_get_samples_float_interleaved(m_vorbis, m_vorbisInfo.channels, m_nextChunkData, ChunkSampleCount * m_vorbisInfo.channels) * m_vorbisInfo.channels * sizeof(float);
            if (dataSizeOut > 0)
            {
                return m_nextChunkData;
            }
            else
            {
                return nullptr;
            }
        }


        void SoundStreamer_Vorbis::Seek(double time)
        {
            (void)time;

            if (time == 0.0)
            {
                stb_vorbis_seek_start(m_vorbis);
            }
            else
            {
                // Not working at the moment.
            }
        }


        uint16_t SoundStreamer_Vorbis::GetNumChannels() const
        {
            return static_cast<uint16_t>(m_vorbisInfo.channels);
        }

        uint16_t SoundStreamer_Vorbis::GetBitsPerSample() const
        {
            return 4;   // Float data.
        }

        uint32_t SoundStreamer_Vorbis::GetSampleRate() const
        {
            return m_vorbisInfo.sample_rate;
        }

        GTEngine::AudioDataFormat SoundStreamer_Vorbis::GetFormat() const
        {
            if (m_vorbisInfo.channels == 1)
            {
                return GTEngine::AudioDataFormat_Mono32F;
            }
            else if (m_vorbisInfo.channels == 2)
            {
                return GTEngine::AudioDataFormat_Stereo32F;
            }


            // Default case.
            return GTEngine::AudioDataFormat_Mono32F;
        }
    }
}