// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_AssetLibrary_hpp_
#define __GT_AssetLibrary_hpp_

#include <GTLib/Vector.hpp>
#include <GTLib/Dictionary.hpp>
#include "AssetTypes.hpp"

namespace GT
{
    class Asset;
    class AssetAllocator;
    class FileSystem;

    

    /// Class for managing every type of asset.
    class AssetLibrary
    {
    public:

        /// Constructor.
        AssetLibrary(FileSystem &fileSystem);

        /// Destructor.
        ~AssetLibrary();


        /// Starts up the asset library.
        bool Startup();

        /// Shuts down the asset library.
        void Shutdown();


        /// Loads an asset using the give file path.
        ///
        /// @param filePath [in] The absolute or relative file path of the asset to load.
        ///
        /// @return A pointer to the new asset object, or null if an error occurs.
        ///
        /// @remarks
        ///     If \c filePath is relative, it will be converted to absolute based on the base directory priorities
        ///     @par
        ///     If the asset has already been loaded, this will return a pointer to the existing asset. If the asset should be re-loaded, use Reload().
        Asset* Load(const char* filePath);

        /// Increments the reference counter of an existing asset.
        ///
        /// @param pAsset [in] A pointer to the asset whose reference counter should be incremented.
        ///
        /// @return \c pAsset. The return value is only used for consistency with Load(const char*).
        Asset* Load(Asset* pAsset);

        /// Unloads an asset.
        ///
        /// @param asset [in] A pointer to the asset to unload.
        void Unload(Asset* asset);

        /// Reloads the asset at the given file path.
        ///
        /// @param filePath [in] the absolute or relative path of the asset to reload.
        void Reload(const char* filePath);


        void RegisterAllocator(AssetAllocator &allocator);
        void RegisterExtensions(AssetExtensionDesc* extensions, size_t extensionsCount);


    private:

        AssetAllocator* FindAllocatorByExtension(const char* extension);
        AssetAllocator* FindAllocatorByType(AssetType type);
        AssetType FindTypeByExtension(const char* extension);
        AssetClass FindClassByExtension(const char* extension);


    private:

        /// A reference to the file system to use when loading and unloading assets.
        FileSystem &m_fileSystem;

        /// The list of allocators.
        GTLib::Vector<AssetAllocator*> m_allocators;

        /// The list of extension/classification/type mappings. The value is the asset class and type encoded as ((class << 32) | type)
        GTLib::Dictionary<uint64_t> m_extensionTypeMapping;

        /// The list of loaded assets, by absolute file path.
        GTLib::Dictionary<Asset*> m_loadedAssets;


    private:    // No copying.
        AssetLibrary(const AssetLibrary &);
        AssetLibrary & operator=(const AssetLibrary &);
    };
}

#endif
