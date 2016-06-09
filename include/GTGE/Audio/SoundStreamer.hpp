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
        SoundStreamer(const void* fileData, size_t fileDataSizeInBytes);

        /// Destructor.
        ~SoundStreamer();


    // Virtual methods.
    public:

        /// Initializes the streamer.
        ///
        /// @return True if the streamer is initialized successfully; false otherwise.
        ///
        /// @remarks
        ///     This is where the validity of the file data should be initially checked.
        bool Initialize();


        /// Reads the next data chunk.
        uint64_t Read(uint64_t samplesToRead, void* pSamplesOut);

        /// Seeks the read positition of the streamer.
        ///
        /// @remarks
        ///     To seek to the start of the sound, call this method with a value of 0.
        bool Seek(uint64_t sample);


        /// Retrieves the format of the audio.
        ///
        /// @return The format of the audio.
        dra_format GetFormat() const;

        /// Retrieves the number of channels in the sound.
        ///
        /// @return The number of channels in the sound.
        unsigned int GetNumChannels() const;

        /// Retrieves the sample rate. 22050, 44100, etc.
        ///
        /// @return The sample rate of the sound.
        unsigned int GetSampleRate() const;



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



    private:

        // A pointer to the raw file data.
        const void* m_pData;

        // The size of the data in bytes.
        size_t m_dataSize;

        // The internal dr_audio decoder.
        dra_decoder m_decoder;
    };
}


#endif