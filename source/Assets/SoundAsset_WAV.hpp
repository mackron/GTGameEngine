// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_SoundAsset_WAV
#define GT_SoundAsset_WAV

#include <GTGE/Config.hpp>

#if defined(GT_BUILD_WAV)
#include <GTGE/Assets/SoundAsset.hpp>

namespace GT
{
    class SoundAsset_WAV : public SoundAsset
    {
    public:

        /// Constructor
        SoundAsset_WAV(const char* absolutePathOrIdentifier, AssetType assetType);

        /// Destructor.
        ~SoundAsset_WAV();


        /// @copydoc Asset::Load()
        bool Load(const char* absolutePath, drvfs_context* pVFS);


        /// @copydoc SoundAsset::GetCompressedFormat()
        CompressedAudioFormat GetCompressedDataFormat() const;

        /// @copydoc SoundAsset::GetDataInfo()
        easyaudio_buffer_desc GetDataInfo() const;


    private:

        /// The data info.
        easyaudio_buffer_desc m_dataInfo;
    };
}

#endif

#endif
