// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Audio_SoundStreamer
#define GT_Audio_SoundStreamer

#include <dr_libs/dr_audio.h>

namespace GT
{
    class Asset;


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
        virtual bool Read(uint64_t samplestoRead, void* pSamplesOut) = 0;

        /// Seeks the read positition of the streamer.
        ///
        /// @remarks
        ///     To seek to the start of the sound, call this method with a value of 0.
        virtual bool Seek(uint64_t sample) = 0;


        /// Retrieves the format of the audio.
        ///
        /// @return The format of the audio.
        virtual dra_format GetFormat() const = 0;

        /// Retrieves the number of channels in the sound.
        ///
        /// @return The number of channels in the sound.
        virtual unsigned int GetNumChannels() const = 0;

        /// Retrieves the sample rate. 22050, 44100, etc.
        ///
        /// @return The sample rate of the sound.
        virtual unsigned int GetSampleRate() const = 0;



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