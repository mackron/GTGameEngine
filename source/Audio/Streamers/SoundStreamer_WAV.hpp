// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Audio_SoundStreamer_WAV_hpp_
#define __GTEngine_Audio_SoundStreamer_WAV_hpp_

#include <GTEngine/Audio/SoundStreamer.hpp>

namespace GTEngine
{
    class SoundStreamer_WAV : public SoundStreamer
    {
    public:

        /// Constructor.
        SoundStreamer_WAV(const void* fileData, size_t fileDataSizeInBytes);

        /// Destructor.
        virtual ~SoundStreamer_WAV();


        /// SoundStreamer::Initialize()
        bool Initialize();


        /// SoundStreamer::Read()
        bool Read(void* pDataOut, unsigned int bytesToRead, unsigned int* bytesReadOut);

        /// SoundStreamer::Seek()
        bool Seek(unsigned int offsetInBytesFromStart);

        /// SoundStreamer::GetNumChannels()
        unsigned int GetNumChannels() const;

        /// SoundStreamer::GetBitsPerSample()
        unsigned int GetBitsPerSample() const;

        /// SoundStreamer::GetSampleRate()
        unsigned int GetSampleRate() const;

        /// SoundStreamer::GetFormat()
        easyaudio_format GetFormat() const;


    private:

        /// Reads the RIFF header and chunks which will initialize the streamer.
        ///
        /// @return True if the header and chunks were read successfully.
        bool ReadRIFFHeaderAndChunks();

        /// Retrieves the number of bytes of PCM data that is played per second.
        ///
        /// @return The number of PCM data that is played per second.
        ///
        /// @remarks
        ///     This is used for time-based seeking.
	    uint32_t GetBytesPerSecond() const;



    private:

        /// A pointer to the file data.
        const void* m_fileDataPtr;

        /// The size in bytes of the file data.
        size_t m_fileDataSizeInBytes;


        /// The format code. We use this in determining whether or not we're loading integer or float data.
        uint16_t m_formatCode;

        /// The number of channels making up the sound. A value of 1 is mon, 2 is stereo, etc.
        uint16_t m_numChannels;

        /// The sample rate (blocks per second).
        uint32_t m_sampleRate;

        /// The average bytes per second (not currently used).
        uint32_t m_averageBytesPerSecond;

        /// The data block size, in bytes.
        uint16_t m_blockAlign;

        /// The number of bits per sample.
        uint16_t m_bitsPerSample;



        /// A pointer to the start of the audio data.
        const void* m_audioData;

        /// The size of the audio data, in bytes.
        size_t m_audioDataSize;

        /// The current read position.
        size_t m_readPos;


        /// A pointer to the next chunk that'll be read.
        //const void* m_nextChunkData;

        
    private:    // No copying.
        SoundStreamer_WAV(const SoundStreamer_WAV &);
        SoundStreamer_WAV & operator=(const SoundStreamer_WAV &);
    };
}


#endif
