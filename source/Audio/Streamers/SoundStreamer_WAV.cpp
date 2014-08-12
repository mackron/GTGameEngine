// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "SoundStreamer_WAV.hpp"
#include <GTEngine/Errors.hpp>
#include <GTLib/IO.hpp>


#define WAVE_FORMAT_PCM        0x1
#define WAVE_FORMAT_IEEE_FLOAT 0x3
#define WAVE_FORMAT_ALAW       0x6
#define WAVE_FORMAT_MULAW      0x7


namespace GTEngine
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
          m_nextChunkData(nullptr), m_nextChunkDataSize(0)
    {
    }

    SoundStreamer_WAV::~SoundStreamer_WAV()
    {
    }

    bool SoundStreamer_WAV::Initialize()
    {
        return this->ReadRIFFHeaderAndChunks();
    }

    const void* SoundStreamer_WAV::ReadNextChunk(size_t &dataSizeOut)
    {
        // We need to calculate how much data is available to read. To do this, we'll first move the next chunk data pointer
        // forward by it's data size such that it will point to the new data chunk.
        m_nextChunkData = reinterpret_cast<const void*>(reinterpret_cast<size_t>(m_nextChunkData) + m_nextChunkDataSize);

        const size_t targetSize = ChunkSampleCount * m_numChannels * (m_bitsPerSample / 8);
        const size_t bytesLeft  = m_audioDataSize - (reinterpret_cast<size_t>(m_nextChunkData) - reinterpret_cast<size_t>(m_audioData));

        dataSizeOut = m_nextChunkDataSize = (targetSize < bytesLeft) ? targetSize : bytesLeft;
        if (m_nextChunkDataSize != 0)
        {
            return m_nextChunkData;
        }
        else
        {
            return nullptr;
        }
    }

    void SoundStreamer_WAV::Seek(double time)
    {
        assert(time >= 0.0);

        m_nextChunkData     = reinterpret_cast<const int8_t*>(m_audioData) + static_cast<int>(this->GetBytesPerSecond() * time);
        m_nextChunkDataSize = 0;
    }


    uint16_t SoundStreamer_WAV::GetNumChannels() const
    {
        return m_numChannels;
    }

    uint16_t SoundStreamer_WAV::GetBitsPerSample() const
    {
        return m_bitsPerSample;
    }

    uint32_t SoundStreamer_WAV::GetSampleRate() const
    {
        return m_sampleRate;
    }

    AudioDataFormat SoundStreamer_WAV::GetFormat() const
    {
        if (m_bitsPerSample == 8)
        {
            if (m_formatCode == WAVE_FORMAT_ALAW)
            {
                if (m_numChannels == 1)
                {
                    return AudioDataFormat_Mono_ALaw;
                }
                else if (m_numChannels == 2)
                {
                    return AudioDataFormat_Stereo_ALaw;
                }
            }
            else if (m_formatCode == WAVE_FORMAT_MULAW)
            {
                if (m_numChannels == 1)
                {
                    return AudioDataFormat_Mono_ULaw;
                }
                else if (m_numChannels == 2)
                {
                    return AudioDataFormat_Stereo_ULaw;
                }
            }
            else
            {
                if (m_numChannels == 1)
                {
                    return AudioDataFormat_Mono8;
                }
                else if (m_numChannels == 2)
                {
                    return AudioDataFormat_Stereo8;
                }
            }
        }
        else if (m_bitsPerSample == 16)
        {
            if (m_numChannels == 1)
            {
                return AudioDataFormat_Mono16;
            }
            else if (m_numChannels == 2)
            {
                return AudioDataFormat_Stereo16;
            }
        }
        else if (m_bitsPerSample == 24)
        {
            if (m_numChannels == 1)
            {
                return AudioDataFormat_Mono24;
            }
            else if (m_numChannels == 2)
            {
                return AudioDataFormat_Stereo24;
            }
        }
        else if (m_bitsPerSample == 32)
        {
            if (m_formatCode == WAVE_FORMAT_PCM)
            {
                // TODO: How would we handle this? Just add support for 32-bit with a data conversion?
            }
            else
            {
                if (m_numChannels == 1)
                {
                    return AudioDataFormat_Mono32F;
                }
                else if (m_numChannels == 2)
                {
                    return AudioDataFormat_Stereo32F;
                }
            }
        }
        else if (m_bitsPerSample == 64)
        {
            if (m_formatCode == WAVE_FORMAT_PCM)
            {
                // TODO: How would we handle this? Just add support for 64-bit with a data conversion?
            }
            else
            {
                if (m_numChannels == 1)
                {
                    return AudioDataFormat_Mono64F;
                }
                else if (m_numChannels == 2)
                {
                    return AudioDataFormat_Stereo64F;
                }
            }
        }


        // Default case.
        return AudioDataFormat_Mono8;
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
            GTEngine::PostError("SoundStreamer_WAV::ReadRIFFHeader() - File is not a RIFF file.");
		    return false;
        }

        if (*fileData32++ == 0)
        {
            GTEngine::PostError("SoundStreamer_WAV::ReadRIFFHeader() - cksize is 0.");
            return false;
        }

        if (*fileData32++ != ChunkID_WAVE)
        {
            GTEngine::PostError("SoundStreamer_WAV::ReadRIFFHeader() - File is not a WAVE file.");
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
                        GTEngine::PostError("SoundStreamer_WAV::ReadRIFFChunks() - Unsupported compression format.");
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

                    m_nextChunkData     = m_audioData;
                    m_nextChunkDataSize = 0;


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
            GTEngine::PostError("SoundStreamer_WAV::ReadRIFFChunks() - FMT and/or DATA chunks were not found.");
		    return false;
	    }

	    return true;
    }

    uint32_t SoundStreamer_WAV::GetBytesPerSecond() const
    {
	    return ((m_bitsPerSample / 8U) * m_numChannels) * m_sampleRate;
    }
}
