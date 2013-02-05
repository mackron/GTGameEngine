// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Audio_SoundStreamer_hpp_
#define __GTEngine_Audio_SoundStreamer_hpp_

#include <GTCore/String.hpp>
#include "SoundFormats.hpp"

namespace GTEngine
{
    class SoundStreamer
    {
    public:

        /// Constructor.
        SoundStreamer(const char* fileName);

        /// Destructor.
        virtual ~SoundStreamer();


        /**
        *   \brief  Retrieves the size of each chunk.
        *   \return The size of each chunk.
        */
        virtual uint32_t GetChunkSize();


    // Virtual methods.
    public:

        /// Opens the streamer.
        ///
        /// @return True if the streamer is opened successfully; false otherwise.
        virtual bool Open() = 0;

        /// Closes the streamer.
        virtual void Close() = 0;

        /**
        *   \brief             Reads a chunk of data.
        *   \param  data [out] Pointer to the buffer that will recieve the data.
        *   \return            The number of bytes that was read.
        *
        *   \remarks
        *       The size of the buffer pointed to by \c data in bytes should be equal to value returned
        *       by SoundStreamer::GetChunkSize().
        *       \par
        *       When reading a chunk causes the streamer to reach the end of the file, it will not read
        *       from the start of the sound to fill up the chunk.
        *       \par
        *       When this function returns 0, it can be assumed that the sound is at the end of itself.
        *       A subsequent call to ReadChunk() will cause the streamer to move back to the start of
        *       the file.
        */
        virtual uint32_t ReadChunk(void *data) = 0;

        /**
        *   \brief            Seeks the read positition of the streamer.
        *   \param  time [in] The time in seconds to seek to.
        *
        *   \remarks
        *       To seek to the start of the sound, call this method with a value of 0.
        *       \par
        *       The input time is always clamped between 0 and the length of the sound in seconds.
        */
        virtual void Seek(double time) = 0;


        /**
        *   \brief  Retrieves the total size of the PCM data.
        *   \return The total size of the PCM data.
        */
        virtual uint64_t GetTotalPCMDataSize() = 0;


    public:	// Accessors / mutators.

	    /**
        *   \brief  Retrieves the number of channels in the sound.
        *   \return The number of channels in the sound.
        */
        uint16_t GetNumChannels() const { return this->numChannels; }

        /**
        *   \brief  Retrieves number of bits per sample. Usually set to 16.
        *   \return The number of bits per sample.
        */
        uint16_t GetBitsPerSample() const { return this->bitsPerSample; }

	    /**
        *   \brief  Retrieves the sample rate. 22050, 44100, etc.
        *   \return The sample rate of the sound.
        */
        uint32_t GetSampleRate() const { return this->sampleRate; }

        /**
        *   \brief  Retrieves the format of the audio.
        *   \return The format of the audio.
        */
        SoundFormat GetFormat() const { return this->format; }

	    


    protected:

        /// The path of the file being streamed.
        GTCore::String absolutePath;

        /// The number of channels in the sound.
	    uint16_t numChannels;

        /// The bits per sample. Usually set to 16.
	    uint16_t bitsPerSample;

	    /// The sample rate. 22050, 44100, etc.
	    uint32_t sampleRate;

        /// The format of the audio.
        SoundFormat format;
    };
}


#endif