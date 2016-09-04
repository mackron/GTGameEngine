// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_SoundAsset
#define GT_SoundAsset

namespace GT
{
    struct VoiceDesc
    {
        dra_format format;
        unsigned int channels;
        unsigned int sampleRate;

        void* pData;
        size_t dataSize;
    };


    /// Base class for sound assets.
    class SoundAsset : public Asset
    {
    public:

        /// Constructor
        SoundAsset(const char* absolutePathOrIdentifier, AssetType assetType);

        /// Destructor.
        ~SoundAsset();


        /// @copydoc Asset::GetClass()
        AssetClass GetClass() const;

        /// @copydoc Asset::Load()
        bool Load(const char* absolutePath, drfs_context* pVFS);

        /// Retrieves information about the audio data of the sound.
        ///
        /// @remarks
        ///     The pInitialData member of the returned object is a shared pointer to the compressed audio data. The sizeInBytes member is the size
        ///     of the data referred to by pInitialData. All other members of the returned object refer to the uncompressed data and can be passed
        ///     directly to dr_audio.
        ///     @par
        ///     Currently, the pInitialData member should point to the first byte of the entire file.
        VoiceDesc GetDataInfo() const;



    private:

        VoiceDesc m_dataInfo;
    };
}

#endif
