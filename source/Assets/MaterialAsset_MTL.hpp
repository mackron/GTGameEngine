// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_MaterialAsset_MTL
#define GT_MaterialAsset_MTL

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_MTL)
#include <GTGameEngine/Assets/MaterialAsset.hpp>

namespace GT
{
    /// Material asset for OBJ MTL material files.
    class MaterialAsset_MTL : public MaterialAsset
    {
    public:

        /// Constructor.
        MaterialAsset_MTL(AssetType assetType);

        /// Destructor.
        ~MaterialAsset_MTL();


        /// @copydoc Asset::Load()
        bool Load(const char* absolutePath, FileSystem &fileSystem);


        /// @copydoc MaterialAsset::GetData()
        const void* GetData() const;

        /// @copydoc MaterialAsset::GetDataSizeInBytes()
        unsigned int GetDataSizeInBytes() const;


    private:

        /// A pointer to the raw material data. This will be allocated in Load()
        void* m_pData;

        /// The size of the raw material data, in bytes.
        unsigned int m_dataSizeInBytes;
    };
}


#endif

#endif
