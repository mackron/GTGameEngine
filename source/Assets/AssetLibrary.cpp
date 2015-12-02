// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGE/Assets/AssetLibrary.hpp>
#include <GTGE/Assets/Asset.hpp>
#include <GTGE/Assets/AssetAllocator.hpp>
#include <easy_path/easy_path.h>

#if defined(GT_BUILD_DEFAULT_ASSETS)
#include "DefaultAssetAllocator.hpp"
#endif

namespace GT
{
    AssetLibrary::AssetLibrary()
        : m_pVFS(nullptr),
          m_allocators(),
          m_loadedAssets()
#if defined(GT_BUILD_DEFAULT_ASSETS)
        , m_pDefaultAssetAllocator(nullptr)
#endif
    {
    }

    AssetLibrary::~AssetLibrary()
    {
    }
    

    bool AssetLibrary::Startup(easyvfs_context* pVFS)
    {
        // Don't do anything if it's already been initialized.
        if (m_pVFS == nullptr)
        {
            m_pVFS = pVFS;

#if defined(GT_BUILD_DEFAULT_ASSETS)
            // Create and register the default allocator.
            m_pDefaultAssetAllocator = new DefaultAssetAllocator();
            this->RegisterAllocator(*m_pDefaultAssetAllocator);
#endif
        }

        return m_pVFS != nullptr;
    }

    void AssetLibrary::Shutdown()
    {
        for (size_t iAsset = 0; iAsset < m_loadedAssets.count; ++iAsset)
        {
            auto pAsset = m_loadedAssets.buffer[iAsset]->value;
            assert(pAsset != nullptr);
            {
                auto pAllocator = this->FindAllocatorByType(pAsset->GetType());
                assert(pAllocator != nullptr);
                {
                    pAllocator->DeleteAsset(pAsset);
                }
            }
        }

        m_loadedAssets.Clear();
        m_allocators.Clear();



#if defined(GT_BUILD_DEFAULT_ASSETS)
        delete m_pDefaultAssetAllocator;
        m_pDefaultAssetAllocator = nullptr;
#endif

        m_pVFS = nullptr;
    }


    easyvfs_context* AssetLibrary::GetVFS() const
    {
        return m_pVFS;
    }


    Asset* AssetLibrary::Load(const char* filePathOrIdentifier, AssetType explicitAssetType)
    {
        // When an asset is cached, the absolute path is used to retrieve the cached object. It is possible, however, for an asset to not actually
        // be loaded from a file, in which case filePathOrIdentifier is used as the unique identifier without any modification.

        char absolutePathOrIdentifier[EASYVFS_MAX_PATH];
        if (!easyvfs_find_absolute_path(m_pVFS, filePathOrIdentifier, absolutePathOrIdentifier, sizeof(absolutePathOrIdentifier)))
        {
            // The file could not be found, but there may be a metadata file. It is possible that the data for an asset is
            // entirely defined in the metadata file, we'll look for that file too.
            char metadataPath[EASYVFS_MAX_PATH];
            easypath_copy_and_append_extension(metadataPath, EASYVFS_MAX_PATH, filePathOrIdentifier, "gtdata");

            if (easyvfs_find_absolute_path(m_pVFS, metadataPath, absolutePathOrIdentifier, sizeof(absolutePathOrIdentifier)))
            {
                // The metadata file was found. Later on we'll load the metadata for real, so we'll need to remove the ".gtdata" extension beforehand.
                easypath_remove_extension(absolutePathOrIdentifier);
            }
            else
            {
                // The file nor it's metadata file could not be found, but the asset loader might be using it as a unique identifier, so we just use it as-is in this case.
                strcpy_s(absolutePathOrIdentifier, filePathOrIdentifier);
            }
        }


        auto iExistingAsset = m_loadedAssets.Find(absolutePathOrIdentifier);
        if (iExistingAsset == nullptr)
        {
            AssetAllocator* pAllocator = nullptr;

            AssetType assetType = explicitAssetType;
            if (assetType == AssetType_Unknown)
            {
                pAllocator = this->FindAllocatorAndTypeByPath(absolutePathOrIdentifier, assetType);
            }
            else
            {
                pAllocator = this->FindAllocatorByType(assetType);
            }


            if (pAllocator != nullptr)
            {
                auto pAsset = pAllocator->CreateAsset(absolutePathOrIdentifier, assetType);
                if (pAsset != nullptr)
                {
                    // Load the metadata first. It does not matter if this fails so the return value doesn't need to be checked.
                    char metadataAbsolutePath[EASYVFS_MAX_PATH];
                    easypath_copy_and_append_extension(metadataAbsolutePath, EASYVFS_MAX_PATH, absolutePathOrIdentifier, "gtdata");
                    pAsset->LoadMetadata(metadataAbsolutePath, m_pVFS);


                    // Load the asset after the metadata.
                    if (pAsset->Load(absolutePathOrIdentifier, m_pVFS))
                    {
                        m_loadedAssets.Add(absolutePathOrIdentifier, pAsset);
                        return pAsset;
                    }
                    else
                    {
                        // Failed to load the asset.
                        pAllocator->DeleteAsset(pAsset);
                        return nullptr;
                    }
                }
                else
                {
                    // Failed to create an asset of the given type.
                    return nullptr;
                }
            }
            else
            {
                // Could not find a supported allocator.
                return nullptr;
            }
        }
        else
        {
            auto pExistingAsset = iExistingAsset->value;
            assert(pExistingAsset != nullptr);
            {
                pExistingAsset->IncrementReferenceCount();
            }

            return iExistingAsset->value;
        }
    }

    Asset* AssetLibrary::Load(Asset* pAsset)
    {
        if (pAsset != nullptr)
        {
            pAsset->IncrementReferenceCount();
        }

        return pAsset;
    }

    void AssetLibrary::Unload(Asset* pAsset)
    {
        if (pAsset != nullptr)
        {
            if (pAsset->DecrementReferenceCount() == 0)
            {
                for (size_t iAsset = 0; iAsset < m_loadedAssets.count; ++iAsset)
                {
                    if (m_loadedAssets.buffer[iAsset]->value == pAsset)
                    {
                        // Remove from the cache.
                        m_loadedAssets.RemoveByIndex(iAsset);

                        
                        // Delete the asset object.
                        auto pAllocator = this->FindAllocatorByType(pAsset->GetType());
                        assert(pAllocator != nullptr);
                        {
                            pAllocator->DeleteAsset(pAsset);
                        }


                        break;
                    }
                }
            }
        }
    }

    void AssetLibrary::Reload(const char* filePathOrIdentifier)
    {
        char absolutePathOrIdentifier[EASYVFS_MAX_PATH];
        if (!easyvfs_find_absolute_path(m_pVFS, filePathOrIdentifier, absolutePathOrIdentifier, sizeof(absolutePathOrIdentifier)))
        {
            char metadataPath[EASYVFS_MAX_PATH];
            easypath_copy_and_append_extension(metadataPath, EASYVFS_MAX_PATH, filePathOrIdentifier, "gtdata");

            if (!easyvfs_find_absolute_path(m_pVFS, metadataPath, absolutePathOrIdentifier, sizeof(absolutePathOrIdentifier)))
            {
                // The file nor it's metadata file could not be found, but the asset loader might be using it as a unique token, so we just assume use it as-is for the absolute path in this case.
                strcpy_s(absolutePathOrIdentifier, filePathOrIdentifier);
            }
        }

        auto iAsset = m_loadedAssets.Find(absolutePathOrIdentifier);
        if (iAsset != nullptr)
        {
            auto pAsset = iAsset->value;
            assert(pAsset != nullptr);
            {
                // Load the metadata first. It does not matter if this fails so the return value doesn't need to be checked.
                char metadataAbsolutePath[EASYVFS_MAX_PATH];
                easypath_copy_and_append_extension(metadataAbsolutePath, EASYVFS_MAX_PATH, filePathOrIdentifier, "gtdata");
                pAsset->LoadMetadata(metadataAbsolutePath, m_pVFS);

                // Load the asset after the metadata.
                if (pAsset->Load(absolutePathOrIdentifier, m_pVFS))
                {
                }
                else
                {
                    // Failed to reload asset.
                }
            }
        }
    }


    void AssetLibrary::RegisterAllocator(AssetAllocator &allocator)
    {
        // It is considered an error if the allocator already exists.
        assert(m_allocators.Exists(&allocator) == false);
        {
            m_allocators.InsertAt(&allocator, 0);
        }
    }



    ////////////////////////////////////
    // Private

    AssetAllocator* AssetLibrary::FindAllocatorAndTypeByPath(const char* filePath, AssetType &assetTypeOut)
    {
        for (size_t iAllocator = 0; iAllocator < m_allocators.GetCount(); ++iAllocator)
        {
            auto pAllocator = m_allocators[iAllocator];
            assert(pAllocator != nullptr);
            {
                AssetType assetType = pAllocator->GetAssetTypeByPath(filePath);
                if (assetType != AssetType_Unknown)
                {
                    assetTypeOut = assetType;
                    return pAllocator;
                }
            }
        }

        return nullptr; 
    }

    AssetAllocator* AssetLibrary::FindAllocatorByType(AssetType type)
    {
        for (size_t iAllocator = 0; iAllocator < m_allocators.GetCount(); ++iAllocator)
        {
            auto pAllocator = m_allocators[iAllocator];
            assert(pAllocator != nullptr);
            {
                if (pAllocator->IsAssetTypeSupported(type))
                {
                    return pAllocator;
                }
            }
        }

        return nullptr;
    }
}
