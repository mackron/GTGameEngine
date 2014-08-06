// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Audio_SoundStreamer_hpp_
#define __GTEngine_Audio_SoundStreamer_hpp_

#include <GTLib/String.hpp>
#include "AudioDataFormats.hpp"

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
        //virtual size_t GetChunkSize() const;


    // Virtual methods.
    public:

        /// Opens the streamer.
        ///
        /// @return True if the streamer is opened successfully; false otherwise.
        virtual bool Open() = 0;

        /// Closes the streamer.
        virtual void Close() = 0;


        /// Reads the next data chunk.
        ///
        /// @param dataSizeOut [in] A reference to the variable that will receive the size in bytes of the returned buffer.
        ///
        /// @return A pointer to the buffer containing the audio data.
        ///
        /// @remarks
        ///     This will not loop back to the start. To do this, call Seek(0).
        ///     @par
        ///     The returned pointer should not be cached. It should be considered invalid after the next call to ReadNextChunk().
        ///     @par
        ///     The returned pointer is memory managed internally. The buffer will become invalid after destruction, the next call
        ///     to ReadNextChunk() and Close().
        virtual const void* ReadNextChunk(size_t &dataSizeOut) = 0;

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
        *   \brief  Retrieves the number of channels in the sound.
        *   \return The number of channels in the sound.
        */
        virtual uint16_t GetNumChannels() const = 0;

        /**
        *   \brief  Retrieves number of bits per sample. Usually set to 16.
        *   \return The number of bits per sample.
        */
        virtual uint16_t GetBitsPerSample() const = 0;

	    /**
        *   \brief  Retrieves the sample rate. 22050, 44100, etc.
        *   \return The sample rate of the sound.
        */
        virtual uint32_t GetSampleRate() const = 0;

        /**
        *   \brief  Retrieves the format of the audio.
        *   \return The format of the audio.
        */
        virtual AudioDataFormat GetFormat() const = 0;


        /**
        *   \brief  Retrieves the total size of the PCM data.
        *   \return The total size of the PCM data.
        */
        //virtual uint64_t GetTotalPCMDataSize() = 0;




    protected:

        /// The path of the file being streamed.
        GTLib::String absolutePath;

#if 0
        /// The number of channels in the sound.
	    uint16_t numChannels;

        /// The bits per sample. Usually set to 16.
	    uint16_t bitsPerSample;

	    /// The sample rate. 22050, 44100, etc.
	    uint32_t sampleRate;

        /// The format of the audio.
        AudioDataFormat format;
#endif
    };
}


#endif