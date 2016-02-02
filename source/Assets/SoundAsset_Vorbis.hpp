// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_SoundAsset_Vorbis
#define GT_SoundAsset_Vorbis

#include <GTGE/Config.hpp>

#if defined(GT_BUILD_VORBIS)
#include <GTGE/Assets/SoundAsset.hpp>

namespace GT
{
    class SoundAsset_Vorbis : public SoundAsset
    {
    public:

        /// Constructor
        SoundAsset_Vorbis(const char* absolutePathOrIdentifier, AssetType assetType);

        /// Destructor.
        ~SoundAsset_Vorbis();


        /// @copydoc Asset::Load()
        bool Load(const char* absolutePath, drvfs_context* pVFS);


        /// @copydoc SoundAsset::GetCompressedFormat()
        CompressedAudioFormat GetCompressedDataFormat() const;

        /// @copydoc SoundAsset::GetDataInfo()
        draudio_buffer_desc GetDataInfo() const;


    private:

        /// The data info.
        draudio_buffer_desc m_dataInfo;
    };
}

#endif

#endif
