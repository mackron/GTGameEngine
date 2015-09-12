// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Audio_SoundStreamer_WAV
#define GT_Audio_SoundStreamer_WAV

#include <GTGameEngine/Audio/SoundStreamer.hpp>

namespace GT
{
    class SoundStreamer_WAV : public SoundStreamer
    {
    public:

        /// Constructor.
        SoundStreamer_WAV();

        /// Destructor.
        virtual ~SoundStreamer_WAV();


        /// SoundStreamer::Initialize()
        bool Initialize(const char* filePath, FileSystem &fileSystem);

        /// SoundStreamer::Uninitialize()
        void Uninitialize();


        /// SoundStreamer::ReadNextChunk()
        const void* ReadNextChunk(size_t &dataSizeOut);

        /// SoundStreamer::Seek()
        void Seek(double time);

        /// SoundStreamer::GetNumChannels()
        uint16_t GetNumChannels() const;

        /// SoundStreamer::GetBitsPerSample()
        uint16_t GetBitsPerSample() const;

        /// SoundStreamer::GetSampleRate()
        uint32_t GetSampleRate() const;

        /// SoundStreamer::GetFormat()
        AudioDataFormat GetFormat() const;


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
        void* m_fileDataPtr;

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


        /// A pointer to the next chunk that'll be read.
        const void* m_nextChunkData;

        /// The size of the data of the next chunk, in bytes.
        size_t m_nextChunkDataSize;

        
    private:    // No copying.
        SoundStreamer_WAV(const SoundStreamer_WAV &);
        SoundStreamer_WAV & operator=(const SoundStreamer_WAV &);
    };
}


#endif