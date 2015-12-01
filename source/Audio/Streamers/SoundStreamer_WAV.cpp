// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "SoundStreamer_WAV.hpp"
#include <GTEngine/GTEngine.hpp>


#define WAVE_FORMAT_PCM        0x1
#define WAVE_FORMAT_IEEE_FLOAT 0x3
#define WAVE_FORMAT_ALAW       0x6
#define WAVE_FORMAT_MULAW      0x7


namespace GT
{
    static const size_t ChunkSampleCount = 8192;       // <-- This controls the size of each streaming chunk. Larger values mean more memory usage, but less chance of stuttering.

    SoundStreamer_WAV::SoundStreamer_WAV(const void* fileData, size_t fileDataSizeInBytes)
        : SoundStreamer(),
          m_fileDataPtr(fileData), m_fileDataSizeInBytes(fileDataSizeInBytes),
          m_formatCode(0),
          m_numChannels(0),
          m_sampleRate(0),
          m_averageBytesPerSecond(0),
          m_blockAlign(0),
          m_bitsPerSample(0),
          m_audioData(nullptr), m_audioDataSize(0),
          m_readPos(0)
    {
    }

    SoundStreamer_WAV::~SoundStreamer_WAV()
    {
    }

    bool SoundStreamer_WAV::Initialize()
    {
        bool result = this->ReadRIFFHeaderAndChunks();
        if (result)
        {
            if (m_formatCode == WAVE_FORMAT_PCM || m_formatCode == WAVE_FORMAT_IEEE_FLOAT)
            {
                return true;
            }
            else
            {
                // Unsupported format. Only supporting PCM and IEEE float for the moment.
                return false;
            }
        }

        return false;
    }

    bool SoundStreamer_WAV::Read(void* pDataOut, unsigned int bytesToRead, unsigned int* bytesReadOut)
    {
        if (m_readPos >= m_audioDataSize)
        {
            // No data available.
            return false;
        }

        size_t bytesAvailable = m_audioDataSize - m_readPos;
        if (bytesAvailable < bytesToRead) {
            bytesToRead = static_cast<unsigned int>(bytesAvailable);
        }

        memcpy(pDataOut, reinterpret_cast<const char*>(m_audioData) + m_readPos, bytesToRead);
        m_readPos += bytesToRead;

        if (bytesReadOut != NULL) {
            *bytesReadOut = bytesToRead;
        }

        return true;
    }

    bool SoundStreamer_WAV::Seek(unsigned int offsetInBytesFromStart)
    {
        assert(offsetInBytesFromStart >= 0);

        if (offsetInBytesFromStart > m_audioDataSize) {
            return false;
        }

        m_readPos = offsetInBytesFromStart;
        return true;
    }


    unsigned int SoundStreamer_WAV::GetNumChannels() const
    {
        return m_numChannels;
    }

    unsigned int SoundStreamer_WAV::GetBitsPerSample() const
    {
        return m_bitsPerSample;
    }

    unsigned int SoundStreamer_WAV::GetSampleRate() const
    {
        return m_sampleRate;
    }

    easyaudio_format SoundStreamer_WAV::GetFormat() const
    {
        if (m_formatCode == WAVE_FORMAT_IEEE_FLOAT) {
            return easyaudio_format_float;
        }

        return easyaudio_format_pcm;
    }



    bool SoundStreamer_WAV::ReadRIFFHeaderAndChunks()
    {
        const uint32_t ChunkID_RIFF = 0x46464952;
        const uint32_t ChunkID_WAVE = 0x45564157;
        const uint32_t ChunkID_FMT  = 0x20746D66;
        const uint32_t ChunkID_DATA = 0x61746164;

        const uint32_t* fileData32 = reinterpret_cast<const uint32_t*>(m_fileDataPtr);

        // The first 4 bytes should read "RIFF".
        if (*fileData32++ != ChunkID_RIFF)
        {
            g_EngineContext->LogErrorf("SoundStreamer_WAV::ReadRIFFHeader() - File is not a RIFF file.");
		    return false;
        }

        if (*fileData32++ == 0)
        {
            g_EngineContext->LogErrorf("SoundStreamer_WAV::ReadRIFFHeader() - cksize is 0.");
            return false;
        }

        if (*fileData32++ != ChunkID_WAVE)
        {
            g_EngineContext->LogErrorf("SoundStreamer_WAV::ReadRIFFHeader() - File is not a WAVE file.");
		    return false;
        }


        // Determines if we've found our needed chunks.
	    bool foundFMT  = false;
	    bool foundDATA = false;

        size_t bytesRead = 12;      // <-- We start at 12 here, because this should always come after the RIFF header, which is 12 bytes.

        while ((!foundFMT || !foundDATA) && bytesRead < m_fileDataSizeInBytes)
        {
            const uint32_t chunkName = *fileData32++;
            const uint32_t chunkSize = *fileData32++;

            switch (chunkName)
            {
            case ChunkID_FMT:
                {
                    const uint16_t* chunkData = reinterpret_cast<const uint16_t*>(fileData32);

                    // Format code - 2 bytes
                    m_formatCode = chunkData[0];
			        if (m_formatCode != WAVE_FORMAT_PCM && m_formatCode != WAVE_FORMAT_IEEE_FLOAT && m_formatCode != WAVE_FORMAT_ALAW && m_formatCode != WAVE_FORMAT_MULAW)
			        {
                        g_EngineContext->LogErrorf("SoundStreamer_WAV::ReadRIFFChunks() - Unsupported compression format.");
				        return false;
			        }


                    // Channels count - 2 bytes.
                    m_numChannels = chunkData[1];

                    // Sample rate, per second - 4 bytes.
                    m_sampleRate = (chunkData[2] << 0) | (chunkData[3] << 16);

                    // Average bytes per second - 4 bytes.
                    m_averageBytesPerSecond = (chunkData[4] << 0) | (chunkData[5] << 16);

                    // Block align - 2 bytes.
                    m_blockAlign = chunkData[6];

                    // Bits per sample - 2 bytes.
                    m_bitsPerSample = chunkData[7];

                    


                    foundFMT = true;
                    break;
                }

            case ChunkID_DATA:
                {
                    m_audioData     = reinterpret_cast<const void*>(fileData32);
                    m_audioDataSize = static_cast<uint32_t>(chunkSize);
                    m_readPos       = 0;

                    //m_nextChunkData     = m_audioData;
                    //m_nextChunkDataSize = 0;


                    foundDATA = true;
                    break;
                }

            default:
                {
                    // We don't care about this chunk - skip over it.
                    break;
                }
            }


            bytesRead += chunkSize;
            fileData32 = reinterpret_cast<const uint32_t*>(reinterpret_cast<size_t>(fileData32) + chunkSize);
        }

        if (!foundFMT || !foundDATA)
	    {
            g_EngineContext->LogErrorf("SoundStreamer_WAV::ReadRIFFChunks() - FMT and/or DATA chunks were not found.");
		    return false;
	    }

	    return true;
    }

    uint32_t SoundStreamer_WAV::GetBytesPerSecond() const
    {
	    return ((m_bitsPerSample / 8U) * m_numChannels) * m_sampleRate;
    }
}
