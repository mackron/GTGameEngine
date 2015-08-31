// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SoundStreamer
#define GT_SoundStreamer

#include <GTLib/String.hpp>
#include "AudioDataFormats.hpp"
#include "../FileSystem.hpp"

namespace GT
{
    class SoundStreamer
    {
    public:

        /// Constructor.
        SoundStreamer();

        /// Destructor.
        virtual ~SoundStreamer();


    // Virtual methods.
    public:

        /// Initializes the streamer.
        ///
        /// @return True if the streamer is initialized successfully; false otherwise.
        ///
        /// @remarks
        ///     This is where the validity of the file data should be initially checked.
        virtual bool Initialize(const char* filePath, FileSystem &fileSystem) = 0;

        /// Uninitializes the streamer.
        virtual void Uninitialize() = 0;


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
    };
}


#endif