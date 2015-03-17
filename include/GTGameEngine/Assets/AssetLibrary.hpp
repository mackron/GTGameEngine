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
        void RegisterAllocator(AssetAllocator &allocator);

        /// Creates a mapping between a file extension and an asset type.
        ///
        /// @param extensions      [in] A pointer to the list of extension mappings.
        /// @param extensionsCount [in] The number of extensions defined in \c extensions.
        ///
        /// @remarks
        ///     Once an extension has been registered, it can never be unregistered. See remarks in RegisterAllocator().
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
