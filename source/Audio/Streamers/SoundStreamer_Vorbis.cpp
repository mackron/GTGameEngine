// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#pragma warning(push)
#pragma warning(disable: 4100)
#pragma warning(disable: 4127)
#pragma warning(disable: 4244)
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_INTEGER_CONVERSION        // <-- Not specifying this causes some compiler errors...
#pragma warning(pop)

#include "SoundStreamer_Vorbis.hpp"

namespace GT
{
    namespace Engine
    {
        SoundStreamer_Vorbis::SoundStreamer_Vorbis(const void* fileData, size_t fileDataSizeInBytes)
            : SoundStreamer(),
              m_fileDataPtr(fileData), m_fileDataSizeInBytes(fileDataSizeInBytes),
              m_vorbis(nullptr), m_vorbisInfo()
        {
        }

        SoundStreamer_Vorbis::~SoundStreamer_Vorbis()
        {
            stb_vorbis_close(m_vorbis);
        }

        bool SoundStreamer_Vorbis::Initialize()
        {
            int error;
            m_vorbis = stb_vorbis_open_memory(reinterpret_cast<const unsigned char*>(m_fileDataPtr), static_cast<int>(m_fileDataSizeInBytes), &error, nullptr);
            if (m_vorbis != nullptr)
            {
                m_vorbisInfo = stb_vorbis_get_info(m_vorbis);
                return true;
            }
            else
            {
                // We failed top create the Vorbis streamer.
                return false;
            }
        }


        bool SoundStreamer_Vorbis::Read(void* pDataOut, unsigned int bytesToRead, unsigned int* bytesReadOut)
        {
            int bytesRead = stb_vorbis_get_samples_float_interleaved(m_vorbis, m_vorbisInfo.channels, reinterpret_cast<float*>(pDataOut), bytesToRead / sizeof(float)) * m_vorbisInfo.channels * sizeof(float);
            if (bytesRead > 0)
            {
                if (bytesReadOut != nullptr) {
                    *bytesReadOut = bytesRead;
                }

                return true;
            }

            return false;
        }


        bool SoundStreamer_Vorbis::Seek(unsigned int offsetInBytesFromStart)
        {
            if (offsetInBytesFromStart == 0)
            {
                stb_vorbis_seek_start(m_vorbis);
                return true;
            }
            else
            {
                // Not working at the moment.
            }

            return false;
        }


        unsigned int SoundStreamer_Vorbis::GetNumChannels() const
        {
            return static_cast<uint16_t>(m_vorbisInfo.channels);
        }

        unsigned int SoundStreamer_Vorbis::GetBitsPerSample() const
        {
            return 4;   // Float data.
        }

        unsigned int SoundStreamer_Vorbis::GetSampleRate() const
        {
            return m_vorbisInfo.sample_rate;
        }

        easyaudio_format SoundStreamer_Vorbis::GetFormat() const
        {
            return easyaudio_format_float;
        }
    }
}