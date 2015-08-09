// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsAssetResourceManager.hpp>
#include <GTGameEngine/Graphics/GraphicsWorld.hpp>
#include <GTGameEngine/Assets/AssetLibrary.hpp>
#include <GTGameEngine/Assets/ImageAsset.hpp>
#include <GTGameEngine/Assets/MaterialAsset.hpp>
#include <GTGameEngine/Assets/ModelAsset.hpp>
#include <GTGameEngine/FileSystem.hpp>
#include "../external/easy_path/easy_path.h"
#include "../external/easy_vfs/easy_vfs.h"

namespace GT
{
    GraphicsAssetResourceManager::GraphicsAssetResourceManager(GraphicsWorld &graphicsWorld, AssetLibrary &assetLibrary)
        : m_graphicsWorld(graphicsWorld), m_assetLibrary(assetLibrary), m_loadedResources(),
          m_pDefaultTexture(nullptr), m_pDefaultMaterial(nullptr), m_pDefaultModel(nullptr), m_pDefaultMesh(nullptr)
    {
        this->LoadDefaultResources();
    }

    GraphicsAssetResourceManager::~GraphicsAssetResourceManager()
    {
        this->UnloadDefaultResources();
    }


    GraphicsWorld & GraphicsAssetResourceManager::GetGraphicsWorld() const
    {
        return m_graphicsWorld;
    }

    AssetLibrary & GraphicsAssetResourceManager::GetAssetLibrary() const
    {
        return m_assetLibrary;
    }


    GraphicsAssetResource_Texture* GraphicsAssetResourceManager::LoadTexture(const char* assetPath)
    {
        GTLib::String assetAbsolutePath;
        Asset* pAsset = nullptr;
        auto pExistingResource = this->FindExistingResourceOrLoadAsset(assetPath, AssetClass_Image, OUT assetAbsolutePath, OUT pAsset);
        if (pExistingResource == nullptr)
        {
            // The resource has not already been loaded.
            if (pAsset != nullptr)
            {
                auto pNewResource = this->LoadTexture(pAsset);
                if (pNewResource != nullptr)
                {
                    m_loadedResources.Add(assetAbsolutePath.c_str(), pExistingResource);
                    return pNewResource;
                }
                else
                {
                    // There was an error loading the resource from the returned asset. Unload the asset and return default resource.
                    m_assetLibrary.Unload(pAsset);
                }
            }

            return m_pDefaultTexture;
        }
        else
        {
            // The asset already exists - increment the reference counter.
            pExistingResource->IncrementReferenceCount();
            return reinterpret_cast<GraphicsAssetResource_Texture*>(pExistingResource);
        }
    }

    GraphicsAssetResource_Material* GraphicsAssetResourceManager::LoadMaterial(const char* assetPath)
    {
        GTLib::String assetAbsolutePath;
        Asset* pAsset = nullptr;
        auto pExistingResource = this->FindExistingResourceOrLoadAsset(assetPath, AssetClass_Material, OUT assetAbsolutePath, OUT pAsset);
        if (pExistingResource == nullptr)
        {
            // The resource has not already been loaded.
            if (pAsset != nullptr)
            {
                auto pNewResource = this->LoadMaterial(pAsset, assetAbsolutePath.c_str());
                if (pNewResource != nullptr)
                {
                    m_loadedResources.Add(assetAbsolutePath.c_str(), pExistingResource);
                    return pNewResource;
                }
                else
                {
                    // There was an error loading the resource from the returned asset. Unload the asset and return default resource.
                    m_assetLibrary.Unload(pAsset);
                }
            }

            return m_pDefaultMaterial;
        }
        else
        {
            // The asset already exists - increment the reference counter.
            pExistingResource->IncrementReferenceCount();
            return reinterpret_cast<GraphicsAssetResource_Material*>(pExistingResource);
        }
    }

    GraphicsAssetResource_Model* GraphicsAssetResourceManager::LoadModel(const char* assetPath)
    {
        GTLib::String assetAbsolutePath;
        Asset* pAsset = nullptr;
        auto pExistingResource = this->FindExistingResourceOrLoadAsset(assetPath, AssetClass_Model, OUT assetAbsolutePath, OUT pAsset);
        if (pExistingResource == nullptr)
        {
            // The resource has not already been loaded.
            if (pAsset != nullptr)
            {
                auto pNewResource = this->LoadModel(pAsset, assetAbsolutePath.c_str());
                if (pNewResource != nullptr)
                {
                    m_loadedResources.Add(assetAbsolutePath.c_str(), pExistingResource);
                    return pNewResource;
                }
                else
                {
                    // There was an error loading the resource from the returned asset. Unload the asset and return default resource.
                    m_assetLibrary.Unload(pAsset);
                }
            }

            return m_pDefaultModel;
        }
        else
        {
            // The asset already exists - increment the reference counter.
            pExistingResource->IncrementReferenceCount();
            return reinterpret_cast<GraphicsAssetResource_Model*>(pExistingResource);
        }
    }


    void GraphicsAssetResourceManager::Unload(const char* assetPath)
    {
        GTLib::String assetAbsolutePath;
        size_t resourceIndex;
        if (this->FindResourceIndexByPath(assetPath, OUT assetAbsolutePath, OUT resourceIndex))
        {
            this->UnloadByIndex(resourceIndex);
        }
    }

    void GraphicsAssetResourceManager::Unload(GraphicsAssetResource* pResource)
    {
        size_t resourceIndex;
        if (this->FindResourceIndex(pResource, OUT resourceIndex))
        {
            this->UnloadByIndex(resourceIndex);
        }
    }


    GraphicsAssetResource* GraphicsAssetResourceManager::FindLoadedResourceByPath(const char* assetPath, OUT GTLib::String &absolutePath)
    {
        size_t resourceIndex;
        if (this->FindResourceIndexByPath(assetPath, OUT absolutePath, OUT resourceIndex))
        {
            return m_loadedResources.buffer[resourceIndex]->value;
        }

        return nullptr;
    }



    GraphicsAssetResource_Texture* GraphicsAssetResourceManager::GetDefaultTexture()
    {
        return m_pDefaultTexture;
    }

    GraphicsAssetResource_Material* GraphicsAssetResourceManager::GetDefaultMaterial()
    {
        return m_pDefaultMaterial;
    }

    GraphicsAssetResource_Model* GraphicsAssetResourceManager::GetDefaultModel()
    {
        return m_pDefaultModel;
    }

    GraphicsAssetResource_Mesh* GraphicsAssetResourceManager::GetDefaultMesh()
    {
        return m_pDefaultMesh;
    }



    GraphicsAssetResource_Texture* GraphicsAssetResourceManager::LoadTexture(Asset* pAsset)
    {
        assert(pAsset != nullptr);
        assert(pAsset->GetClass() == AssetClass_Image);

        auto pImageAsset = reinterpret_cast<ImageAsset*>(pAsset);

        GraphicsTextureResourceDesc desc;
        desc.width  = pImageAsset->GetImageWidth();
        desc.height = pImageAsset->GetImageHeight();
        desc.depth  = 1;
        desc.format = pImageAsset->GetImageFormat();
        desc.pData  = pImageAsset->GetImageData();
        HGraphicsResource hGraphicsResource = m_graphicsWorld.CreateTextureResource(desc);
        if (hGraphicsResource != 0)
        {
            auto pTextureResource = new GraphicsAssetResource_Texture(pAsset, hGraphicsResource);
            assert(pTextureResource != nullptr);

            return pTextureResource;
        }


        // Fall through to the default resource.
        return m_pDefaultTexture;
    }

    GraphicsAssetResource_Material* GraphicsAssetResourceManager::LoadMaterial(Asset* pAsset, const char* absolutePath)
    {
        assert(pAsset != nullptr);
        assert(pAsset->GetClass() == AssetClass_Material);
        assert(absolutePath != nullptr);

        auto pMaterialAsset = reinterpret_cast<MaterialAsset*>(pAsset);

        GraphicsMaterialResourceDesc desc;
        desc.pData           = pMaterialAsset->GetData();
        desc.dataSizeInBytes = pMaterialAsset->GetDataSizeInBytes();
        HGraphicsResource hGraphicsResource = m_graphicsWorld.CreateMaterialResource(desc);
        if (hGraphicsResource != 0)
        {
            auto pMaterialResource = new GraphicsAssetResource_Material(pAsset, hGraphicsResource);
            assert(pMaterialResource != nullptr);

            // Textures need to be loaded for both inputs and properties.
            unsigned int inputCount = pMaterialAsset->GetInputCount();
            for (unsigned int iInput = 0; iInput < inputCount; ++iInput)
            {
                MaterialVariable variable;
                if (pMaterialAsset->GetInputByIndex(iInput, variable))
                {
                    if (variable.type == GraphicsMaterialVariableType::Texture2D)
                    {
                        GraphicsAssetResource_Texture* pTextureResource = nullptr;

                        GTLib::String textureAbsolutePath;
                        if (m_assetLibrary.GetFileSystem().FindAbsolutePath(variable.path.value, this->GetMaterialBaseDirectory(absolutePath).c_str(), textureAbsolutePath))
                        {
                            pTextureResource = this->LoadTexture(textureAbsolutePath.c_str());
                        }
                        else
                        {
                            pTextureResource = this->GetDefaultTexture();
                        }

                        pMaterialResource->_InsertInputTexture(variable.name, pTextureResource);


                        // Set the value on the graphics world's material resource.
                        m_graphicsWorld.SetMaterialResourceInputVariable(pMaterialResource->GetGraphicsResource(), variable.name, pTextureResource->GetGraphicsResource());
                    }
                }
            }

            unsigned int propertyCount = pMaterialAsset->GetPropertyCount();
            for (unsigned int iProperty = 0; iProperty < propertyCount; ++iProperty)
            {
                MaterialVariable variable;
                if (pMaterialAsset->GetPropertyByIndex(iProperty, variable))
                {
                    if (variable.type == GraphicsMaterialVariableType::Texture2D)
                    {
                        GraphicsAssetResource_Texture* pTextureResource = nullptr;

                        GTLib::String textureAbsolutePath;
                        if (m_assetLibrary.GetFileSystem().FindAbsolutePath(variable.path.value, this->GetMaterialBaseDirectory(absolutePath).c_str(), textureAbsolutePath))
                        {
                            pTextureResource = this->LoadTexture(textureAbsolutePath.c_str());
                        }
                        else
                        {
                            pTextureResource = this->GetDefaultTexture();
                        }

                        pMaterialResource->_InsertInputTexture(variable.name, pTextureResource);
                    }
                }
            }


            return pMaterialResource;
        }


        // Fall through to the default resource.
        return m_pDefaultMaterial;
    }

    GraphicsAssetResource_Model* GraphicsAssetResourceManager::LoadModel(Asset* pAsset, const char* absolutePath)
    {
        assert(pAsset != nullptr);
        assert(pAsset->GetClass() == AssetClass_Model);
        assert(absolutePath != nullptr);

        auto pModelAsset = reinterpret_cast<ModelAsset*>(pAsset);

        GraphicsAssetResource_Model* pModelResource = new GraphicsAssetResource_Model(pAsset);

        unsigned int meshCount = pModelAsset->GetMeshCount();
        for (unsigned int iMesh = 0; iMesh < meshCount; ++iMesh)
        {
            GTLib::Vector<GraphicsAssetResource_Material*> materials;

            unsigned int materialCount = pModelAsset->GetMeshMaterialCount(iMesh);
            for (unsigned int iMaterial = 0; iMaterial < materialCount; ++iMaterial)
            {
                GraphicsAssetResource_Material* pMaterialResource = nullptr;

                char materialName[EASYVFS_MAX_PATH];
                pModelAsset->GetMeshMaterialName(iMesh, iMaterial, materialName, EASYVFS_MAX_PATH);

                GTLib::String materialAbsolutePath;
                if (m_assetLibrary.GetFileSystem().FindAbsolutePath(materialName, this->GetMaterialBaseDirectory(absolutePath).c_str(), materialAbsolutePath))
                {
                    pMaterialResource = this->LoadMaterial(materialAbsolutePath.c_str());
                }
                else
                {
                    pMaterialResource = this->GetDefaultMaterial();
                }

                materials.PushBack(pMaterialResource);
            }


            GT::GraphicsMeshResourceDesc desc;
            desc.topology                      = GT::PrimitiveTopologyType_Triangle;
            desc.pVertexData                   = pModelAsset->GetMeshVertexData(iMesh);
            desc.vertexDataSize                = pModelAsset->GetMeshVertexDataSize(iMesh);
            desc.vertexStride                  = pModelAsset->GetMeshVertexStride(iMesh);
            desc.pVertexLayout                 = pModelAsset->GetMeshVertexLayout(iMesh, desc.vertexAttribCount);
            desc.pIndexData                    = pModelAsset->GetMeshIndexData(iMesh);
            desc.indexDataSize                 = pModelAsset->GetMeshIndexDataSize(iMesh);
            desc.indexCount                    = pModelAsset->GetMeshIndexCount(iMesh);
            desc.indexFormat                   = pModelAsset->GetMeshIndexFormat(iMesh);
            desc.materialIndexOffsetCountPairs = pModelAsset->GetMeshMaterialIndexOffsetCountPairs(iMesh);
            desc.materialCount                 = pModelAsset->GetMeshMaterialCount(iMesh);
            desc.materials                     = new HGraphicsResource[desc.materialCount];
            for (size_t iMaterial = 0; iMaterial < materialCount; ++iMaterial)
            {
                desc.materials[iMaterial] = materials[iMaterial]->GetGraphicsResource();
            }

            auto pMeshResource = this->LoadMesh(desc);
            assert(pMeshResource != nullptr);


            // The mesh resource needs to know about it's materials.
            for (unsigned int iMaterial = 0; iMaterial < materialCount; ++iMaterial)
            {
                pMeshResource->_InsertMaterial(materials[iMaterial]);
            }


            // The model needs to know about the mesh.
            pModelResource->_InsertMesh(pMeshResource);

            delete [] desc.materials;
        }

        return pModelResource;
    }

    GraphicsAssetResource_Mesh* GraphicsAssetResourceManager::LoadMesh(GraphicsMeshResourceDesc &desc)
    {
        HGraphicsResource hMeshResource = m_graphicsWorld.CreateMeshResource(desc);
        if (hMeshResource != 0)
        {
            GraphicsAssetResource_Mesh* pMeshResource = new GraphicsAssetResource_Mesh(hMeshResource);
            assert(pMeshResource != nullptr);

            return pMeshResource;
        }

        return m_pDefaultMesh;
    }


    void GraphicsAssetResourceManager::UnloadByIndex(size_t resourceIndex)
    {
        auto pResource = m_loadedResources.buffer[resourceIndex]->value;
        assert(pResource != nullptr);
        {
            // Don't touch the resource if it's a default.
            if (this->IsDefaultResource(pResource))
            {
                return;
            }


            if (pResource->DecrementReferenceCount() == 0)
            {
                auto pAsset = pResource->GetAsset();
                switch (pAsset->GetClass())
                {
                case AssetClass_Image:
                    {
                        this->UnloadTexture(reinterpret_cast<GraphicsAssetResource_Texture*>(pResource));
                        break;
                    }

                case AssetClass_Material:
                    {
                        this->UnloadMaterial(reinterpret_cast<GraphicsAssetResource_Material*>(pResource));
                        break;
                    }

                case AssetClass_Model:
                    {
                        this->UnloadModel(reinterpret_cast<GraphicsAssetResource_Model*>(pResource));
                        break;
                    }

                default:
                    {
                        // Should never get here.
                        assert(false);
                        break;
                    }
                }


                // Unload the asset.
                m_assetLibrary.Unload(pAsset);

                // Finally we remove the resource by it's index.
                m_loadedResources.RemoveByIndex(resourceIndex);
            }
        }
    }

    void GraphicsAssetResourceManager::UnloadTexture(GraphicsAssetResource_Texture* pTexture)
    {
        assert(pTexture != nullptr);

        m_graphicsWorld.DeleteResource(pTexture->GetGraphicsResource());
        delete pTexture;
    }

    void GraphicsAssetResourceManager::UnloadMaterial(GraphicsAssetResource_Material* pMaterial)
    {
        assert(pMaterial != nullptr);

        // Unload every texture first.
        for (size_t iTexture = 0; iTexture < pMaterial->GetTextureCount(); ++iTexture)
        {
            auto pTexture = pMaterial->GetTextureByIndex(iTexture);
            assert(pTexture != nullptr);

            this->Unload(pTexture);
        }


        // Now unload the material itself.
        m_graphicsWorld.DeleteResource(pMaterial->GetGraphicsResource());
        delete pMaterial;
    }

    void GraphicsAssetResourceManager::UnloadModel(GraphicsAssetResource_Model* pModel)
    {
        assert(pModel != nullptr);

        // Unload every mesh first.
        for (size_t iMesh = 0; iMesh < pModel->GetMeshCount(); ++iMesh)
        {
            auto pMesh = pModel->GetMeshByIndex(iMesh);
            assert(pMesh != nullptr);

            this->UnloadMesh(pMesh);
        }


        // Now unload the model itself.
        delete pModel;
    }

    void GraphicsAssetResourceManager::UnloadMesh(GraphicsAssetResource_Mesh* pMesh)
    {
        assert(pMesh != nullptr);

        // Unload every material first.
        for (size_t iMaterial = 0; iMaterial < pMesh->GetMaterialCount(); ++iMaterial)
        {
            auto pMaterial = pMesh->GetMaterialByIndex(iMaterial);
            assert(pMaterial != nullptr);

            this->Unload(pMaterial);
        }


        // Now unload the mesh itself.
        m_graphicsWorld.DeleteResource(pMesh->GetGraphicsResource());
        delete pMesh;
    }


    bool GraphicsAssetResourceManager::FindResourceIndex(GraphicsAssetResource* pResource, size_t &indexOut) const
    {
        for (size_t iExistingItem = 0; m_loadedResources.count; ++iExistingItem)
        {
            auto pExistingItem = m_loadedResources.buffer[iExistingItem];
            assert(pExistingItem != nullptr);

            if (pResource == pExistingItem->value)
            {
                indexOut = iExistingItem;
                return true;
            }
        }

        return false;
    }

    bool GraphicsAssetResourceManager::FindResourceIndexByPath(const char* assetPath, OUT GTLib::String &absolutePath, size_t &indexOut) const
    {
        FileSystem &fileSystem = m_assetLibrary.GetFileSystem();
        
        if (fileSystem.FindAbsolutePath(assetPath, absolutePath))
        {
            auto iExistingItem = m_loadedResources.Find(absolutePath.c_str(), absolutePath.GetLengthInTs());
            if (iExistingItem != nullptr)
            {
                indexOut = iExistingItem->index;
                return true;
            }
        }

        return false;
    }

    GraphicsAssetResource* GraphicsAssetResourceManager::FindExistingResourceOrLoadAsset(const char* assetPath, AssetClass expectedAssetClass, OUT GTLib::String &absolutePathOut, OUT Asset* &pAssetOut)
    {
        auto pExistingResource = this->FindLoadedResourceByPath(assetPath, OUT absolutePathOut);
        if (pExistingResource != nullptr)
        {
            // Already exists. We need to increment the reference counter and return the existing item.
            return reinterpret_cast<GraphicsAssetResource_Texture*>(pExistingResource);
        }
        else
        {
            // Doesn't already exist. We will load the asset from absolutePathOut which was set by FindLoadedResourceByPath().
            Asset* pAsset = m_assetLibrary.Load(absolutePathOut.c_str());
            if (pAsset != nullptr)
            {
                if (pAsset->GetClass() != expectedAssetClass)
                {
                    // The asset is not of the expected class.
                    m_assetLibrary.Unload(pAsset);
                    pAsset = nullptr;
                }
            }

            pAssetOut = pAsset;
        }

        return nullptr;
    }


    bool GraphicsAssetResourceManager::IsDefaultResource(GraphicsAssetResource* pResource)
    {
        return pResource == m_pDefaultTexture || pResource == m_pDefaultMaterial || pResource == m_pDefaultModel;
    }

    void GraphicsAssetResourceManager::LoadDefaultResources()
    {
        this->LoadDefaultTexture();
        this->LoadDefaultMaterial();
        this->LoadDefaultMesh();
        this->LoadDefaultModel();
    }

    void GraphicsAssetResourceManager::LoadDefaultTexture()
    {
        // TODO: Implement.
        m_pDefaultTexture = nullptr;
    }

    void GraphicsAssetResourceManager::LoadDefaultMaterial()
    {
        // TODO: Implement.
        m_pDefaultMaterial = nullptr;
    }

    void GraphicsAssetResourceManager::LoadDefaultModel()
    {
        // TODO: Implement.
        m_pDefaultModel = nullptr;
    }

    void GraphicsAssetResourceManager::LoadDefaultMesh()
    {
        // TODO: Implement.
        m_pDefaultMesh = nullptr;
    }

    void GraphicsAssetResourceManager::UnloadDefaultResources()
    {
        this->UnloadDefaultModel();
        this->UnloadDefaultMesh();
        this->UnloadDefaultMaterial();
        this->UnloadDefaultTexture();
    }

    void GraphicsAssetResourceManager::UnloadDefaultTexture()
    {
        this->UnloadTexture(m_pDefaultTexture);
    }

    void GraphicsAssetResourceManager::UnloadDefaultMaterial()
    {
        this->UnloadMaterial(m_pDefaultMaterial);
    }

    void GraphicsAssetResourceManager::UnloadDefaultModel()
    {
        this->UnloadModel(m_pDefaultModel);
    }

    void GraphicsAssetResourceManager::UnloadDefaultMesh()
    {
        this->UnloadMesh(m_pDefaultMesh);
    }


    GTLib::String GraphicsAssetResourceManager::GetMaterialBaseDirectory(const GTLib::String &materialAbsolutePath) const
    {
        // The base path for materials need special treatment because we cannot have the base path set to a .mtl archive file.

        char basePath[EASYVFS_MAX_PATH];
        easypath_basepath(materialAbsolutePath.c_str(), basePath, EASYVFS_MAX_PATH);

        if (easypath_extensionequal(basePath, "mtl"))
        {
            char basePath2[EASYVFS_MAX_PATH];
            easypath_basepath(basePath, basePath2, EASYVFS_MAX_PATH);

            return basePath2;
        }

        return basePath;
    }
}
