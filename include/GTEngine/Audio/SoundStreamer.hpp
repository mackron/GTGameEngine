// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Audio_SoundStreamer
#define GT_Audio_SoundStreamer

#include <GTEngine/Core/String.hpp>
#include <easy_audio/easy_audio.h>

namespace GT
{
    class Asset;
}

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
        virtual bool Initialize() = 0;


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
        virtual bool Read(void* pDataOut, unsigned int bytesToRead, unsigned int* bytesReadOut) = 0;

        /// Seeks the read positition of the streamer.
        ///
        /// @remarks
        ///     To seek to the start of the sound, call this method with a value of 0.
        virtual bool Seek(unsigned int offsetInBytesFromStart) = 0;


        /// Retrieves the number of channels in the sound.
        ///
        /// @return The number of channels in the sound.
        virtual unsigned int GetNumChannels() const = 0;

        /**
        *   \brief  Retrieves number of bits per sample. Usually set to 16.
        *   \return The number of bits per sample.
        */
        virtual unsigned int GetBitsPerSample() const = 0;

	    /**
        *   \brief  Retrieves the sample rate. 22050, 44100, etc.
        *   \return The sample rate of the sound.
        */
        virtual unsigned int GetSampleRate() const = 0;

        /**
        *   \brief  Retrieves the format of the audio.
        *   \return The format of the audio.
        */
        virtual easyaudio_format GetFormat() const = 0;




        ///////////////////////////////////////////////////////
        //
        // Static Functions
        //
        ///////////////////////////////////////////////////////

        /// Helper for creating a sound streamer from an asset.
        ///
        /// @remarks
        ///     Use Delete() to delete the streamer.
        static SoundStreamer* CreateFromAsset(GT::Asset* pAsset);

        /// Deletes a sound streamer that was created with CreateFromAsset().
        static void Delete(SoundStreamer* pStreamer);
    };
}


#endif