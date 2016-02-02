// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_AssetLibrary
#define GT_AssetLibrary

#include <GTGE/Core/Vector.hpp>
#include <GTGE/Core/Dictionary.hpp>
#include "../Config.hpp"
#include "AssetTypes.hpp"
#include <dr_libs/dr_vfs.h>
#include <dr_libs/dr_util.h>

namespace GT
{
    class Asset;
    class AssetAllocator;

#if defined(GT_BUILD_DEFAULT_ASSETS)
    class DefaultAssetAllocator;
#endif


    /// Class for managing every type of asset.
    ///
    /// Asset paths where the asset data is stored in a library/archive type file is supported. In this case, the last extension is used to determine
    /// which extension should be used for loading. An example of such a path is OBJ MTL files such as "data/material.mtl/mymaterial". In this case,
    /// whichever allocator is associated with the ".mtl" extension is used because that is the last extension mentioned in the path.
    class AssetLibrary
    {
    public:

        /// Constructor.
        AssetLibrary();

        /// Destructor.
        ~AssetLibrary();


        /// Starts up the asset library.
        bool Startup(drvfs_context* pVFS);

        /// Shuts down the asset library.
        void Shutdown();


        /// Retrieves a reference to the asset library that is being used to load the asset files.
        drvfs_context* GetVFS() const;


        /// Loads an asset using the give file path.
        ///
        /// @param filePathOrIdentifier [in] The absolute or relative file path of the asset to load if it is backed by a file, or a unique identifier if the asset is not backed by a file.
        /// @param explicitAssetType    [in] The asset type to load. If this is set to AssetType_Unknow (the default), the extension of \c filePathOrIdentifier will be used to determine the type based on the registered extensions.
        ///
        /// @return A pointer to the new asset object, or null if an error occurs.
        ///
        /// @remarks
        ///     If \c filePathOrIdentifier is a relative path, it will be converted to absolute based on the base directory priorities
        ///     @par
        ///     If the asset has already been loaded, this will return a pointer to the existing asset. If the asset should be re-loaded, use Reload().
        Asset* Load(const char* filePathOrIdentifier, AssetType explicitAssetType = AssetType_Unknown);

        /// Increments the reference counter of an existing asset.
        ///
        /// @param pAsset [in] A pointer to the asset whose reference counter should be incremented.
        ///
        /// @return \c pAsset. The return value is only used for consistency with Load(const char*).
        Asset* Load(Asset* pAsset);

        /// Unloads an asset.
        ///
        /// @param pAsset [in] A pointer to the asset to unload.
        void Unload(Asset* pAsset);

        /// Reloads the asset at the given file path.
        ///
        /// @param filePathOrIdentifier [in] The absolute or relative file path of the asset to reload if it is backed by a file, or a unique identifier if the asset is not backed by a file.
        void Reload(const char* filePathOrIdentifier);


        /// Registers an asset allocator which is used to create and delete asset objects.
        ///
        /// @param allocator [in] A reference to the allocator to register.
        ///
        /// @remarks
        ///     An allocator can never be unregistered and they should always be registered before loading the first asset. The reason for this is that the allocator
        ///     is not directly tied to the assets it creates, which means when an asset is deleted (which is done via the allocator) the library needs a consistent
        ///     list of registered allocators in order to accurately determine the correct allocator to use for deleting.
        ///     @par
        ///     In debug mode, an assertion will be thrown if an asset has previously been loaded.
        ///     @par
        ///     As allocators are added, they are given the highest priority. This makes is possible to override existing allocators for any particular asset type. The
        ///     first allocator to be registered will usually be the engine's default allocator, which means it's possible for the host application to override any of
        ///     the engine's default asset loaders.
        void RegisterAllocator(AssetAllocator &allocator);


    private:

        AssetAllocator* FindAllocatorAndTypeByPath(const char* filePath, AssetType &assetTypeOut);
        AssetAllocator* FindAllocatorByType(AssetType type);


    private:

        /// A pointer to the file system to use when loading and unloading assets.
        drvfs_context* m_pVFS;

        /// The list of allocators.
        Vector<AssetAllocator*> m_allocators;

        /// The list of loaded assets, by absolute file path.
        Dictionary<Asset*> m_loadedAssets;

        /// The mutex for loading and unloading assets.
        dr_mutex m_mutex;

        
#if defined(GT_BUILD_DEFAULT_ASSETS)
        /// A pointer to the default asset allocator. This is always present if at least one default asset is included in the build.
        DefaultAssetAllocator* m_pDefaultAssetAllocator;
#endif


    private:    // No copying.
        AssetLibrary(const AssetLibrary &);
        AssetLibrary & operator=(const AssetLibrary &);
    };
}

#endif
