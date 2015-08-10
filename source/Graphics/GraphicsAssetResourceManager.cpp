// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsAssetResourceManager.hpp>
#include <GTGameEngine/Graphics/GraphicsWorld.hpp>
#include <GTGameEngine/Assets/AssetLibrary.hpp>
#include <GTGameEngine/Assets/ImageAsset.hpp>
#include <GTGameEngine/Assets/MaterialAsset.hpp>
#include <GTGameEngine/Assets/ModelAsset.hpp>
#include <GTGameEngine/FileSystem.hpp>
#include <GTGameEngine/MeshBuilder.hpp>
#include "../external/easy_path/easy_path.h"
#include "../external/easy_vfs/easy_vfs.h"
#include "../external/easy_mtl/easy_mtl.h"

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
        auto pTextureResource = this->CreateTextureResourceFromDesc(desc, pAsset);
        if (pTextureResource == nullptr)
        {
            pTextureResource = m_pDefaultTexture;
        }

        return pTextureResource;
    }

    GraphicsAssetResource_Material* GraphicsAssetResourceManager::LoadMaterial(Asset* pAsset, const char* absolutePath)
    {
        assert(pAsset != nullptr);
        assert(pAsset->GetClass() == AssetClass_Material);
        assert(absolutePath != nullptr);

        char basePath[EASYVFS_MAX_PATH];
        this->GetMaterialBaseDirectory(absolutePath, basePath, EASYVFS_MAX_PATH);

        auto pMaterialAsset = reinterpret_cast<MaterialAsset*>(pAsset);

        GraphicsMaterialResourceDesc desc;
        desc.pData           = pMaterialAsset->GetData();
        desc.dataSizeInBytes = pMaterialAsset->GetDataSizeInBytes();
        auto pMaterialResource = this->CreateMaterialResourceFromDesc(desc, pAsset, basePath);
        if (pMaterialResource == nullptr)
        {
            pMaterialResource = m_pDefaultMaterial;
        }

        return pMaterialResource;
    }

    GraphicsAssetResource_Model* GraphicsAssetResourceManager::LoadModel(Asset* pAsset, const char* absolutePath)
    {
        assert(pAsset != nullptr);
        assert(pAsset->GetClass() == AssetClass_Model);
        assert(absolutePath != nullptr);

        char basePath[EASYVFS_MAX_PATH];
        this->GetModelBaseDirectory(absolutePath, basePath, EASYVFS_MAX_PATH);


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
                if (m_assetLibrary.GetFileSystem().FindAbsolutePath(materialName, basePath, materialAbsolutePath))
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
                auto pMaterial = materials[iMaterial];
                if (pMaterial != nullptr)
                {
                    desc.materials[iMaterial] = pMaterial->GetGraphicsResource();
                }
                else
                {
                    desc.materials[iMaterial] = 0;
                }
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
        auto pMeshResource = this->CreateMeshResourceFromDesc(desc);
        if (pMeshResource == nullptr)
        {
            pMeshResource = m_pDefaultMesh;
        }

        return pMeshResource;
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


    GraphicsAssetResource_Texture* GraphicsAssetResourceManager::CreateTextureResourceFromDesc(GraphicsTextureResourceDesc &desc, Asset* pAsset)
    {
        HGraphicsResource hGraphicsResource = m_graphicsWorld.CreateTextureResource(desc);
        if (hGraphicsResource != 0)
        {
            return new GraphicsAssetResource_Texture(pAsset, hGraphicsResource);
        }

        return nullptr;
    }

    GraphicsAssetResource_Material* GraphicsAssetResourceManager::CreateMaterialResourceFromDesc(GraphicsMaterialResourceDesc &desc, Asset* pAsset, const char* basePath)
    {
        HGraphicsResource hGraphicsResource = m_graphicsWorld.CreateMaterialResource(desc);
        if (hGraphicsResource != 0)
        {
            auto pMaterialResource = new GraphicsAssetResource_Material(pAsset, hGraphicsResource);
            assert(pMaterialResource != nullptr);

            // Textures need to be loaded for both inputs and properties.
            easymtl_material material;
            if (easymtl_initfromexisting_nocopy(&material, desc.pData, desc.dataSizeInBytes))
            {
                // Inputs.
                unsigned int inputCount = easymtl_getinputcount(&material);
                for (unsigned int iInput = 0; iInput < inputCount; ++iInput)
                {
                    easymtl_input* pInput = easymtl_getinputbyindex(&material, iInput);
                    if (pInput != nullptr)
                    {
                        easymtl_identifier* pIdentifier = easymtl_getidentifier(&material, pInput->identifierIndex);
                        if (pIdentifier != nullptr && pIdentifier->type == easymtl_type_tex2d)
                        {
                            GraphicsAssetResource_Texture* pTextureResource = nullptr;

                            GTLib::String textureAbsolutePath;
                            if (m_assetLibrary.GetFileSystem().FindAbsolutePath(pInput->path.value, basePath, textureAbsolutePath))
                            {
                                pTextureResource = this->LoadTexture(textureAbsolutePath.c_str());
                            }
                            else
                            {
                                pTextureResource = this->GetDefaultTexture();
                            }

                            pMaterialResource->_InsertInputTexture(pIdentifier->name, pTextureResource);


                            // Set the value on the graphics world's material resource.
                            m_graphicsWorld.SetMaterialResourceInputVariable(pMaterialResource->GetGraphicsResource(), pIdentifier->name, pTextureResource->GetGraphicsResource());
                        }
                    }
                }


                // Properties
                unsigned int propertyCount = easymtl_getpropertycount(&material);
                for (unsigned int iProperty = 0; iProperty < propertyCount; ++iProperty)
                {
                    easymtl_property* pProperty = easymtl_getpropertybyindex(&material, iProperty);
                    if (pProperty != nullptr && pProperty->type == easymtl_type_tex2d)
                    {
                        GraphicsAssetResource_Texture* pTextureResource = nullptr;

                        GTLib::String textureAbsolutePath;
                        if (m_assetLibrary.GetFileSystem().FindAbsolutePath(pProperty->path.value, basePath, textureAbsolutePath))
                        {
                            pTextureResource = this->LoadTexture(textureAbsolutePath.c_str());
                        }
                        else
                        {
                            pTextureResource = this->GetDefaultTexture();
                        }

                        pMaterialResource->_InsertInputTexture(pProperty->name, pTextureResource);
                    }
                }
            }


            return pMaterialResource;
        }

        return nullptr;
    }

    GraphicsAssetResource_Mesh* GraphicsAssetResourceManager::CreateMeshResourceFromDesc(GraphicsMeshResourceDesc &desc)
    {
        HGraphicsResource hMeshResource = m_graphicsWorld.CreateMeshResource(desc);
        if (hMeshResource != 0)
        {
            GraphicsAssetResource_Mesh* pMeshResource = new GraphicsAssetResource_Mesh(hMeshResource);
            assert(pMeshResource != nullptr);

            return pMeshResource;
        }

        return nullptr;
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
        return pResource == m_pDefaultTexture || pResource == m_pDefaultMaterial || pResource == m_pDefaultModel || pResource == m_pDefaultMesh;
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
        uint32_t imageData[4] = 
        {
            0xFFFFFFFF, 0x80808080,
            0x80808080, 0xFFFFFFFF
        };

        GraphicsTextureResourceDesc desc;
        desc.width  = 2;
        desc.height = 2;
        desc.depth  = 1;
        desc.format = TextureFormat_RGBA8;
        desc.pData  = imageData;
        HGraphicsResource hGraphicsResource = m_graphicsWorld.CreateTextureResource(desc);
        if (hGraphicsResource != 0)
        {
            m_pDefaultTexture = new GraphicsAssetResource_Texture(nullptr, hGraphicsResource);
        }
    }

    void GraphicsAssetResourceManager::LoadDefaultMaterial()
    {
        // The default material just returns constant values for Diffuse, Specular, Specular Exponent, Emissive and Alpha.
        easymtl_material material;
        if (easymtl_init(&material))
        {
            easymtl_appendchannel(&material, easymtl_channel_float3("DiffuseChannel"));
            easymtl_appendinstruction(&material, easymtl_retf3_c3(1, 1, 1));

            easymtl_appendchannel(&material, easymtl_channel_float3("SpecularChannel"));
            easymtl_appendinstruction(&material, easymtl_retf3_c3(1, 1, 1));

            easymtl_appendchannel(&material, easymtl_channel_float3("SpecularExponentChannel"));
            easymtl_appendinstruction(&material, easymtl_retf1_c1(10));

            easymtl_appendchannel(&material, easymtl_channel_float3("EmissiveChannel"));
            easymtl_appendinstruction(&material, easymtl_retf3_c3(0, 0, 0));

            easymtl_appendchannel(&material, easymtl_channel_float3("AlphaChannel"));
            easymtl_appendinstruction(&material, easymtl_retf1_c1(1));

            easymtl_appendproperty(&material, easymtl_property_bool("IsTransparent", false));


            GraphicsMaterialResourceDesc desc;
            desc.pData           = material.pRawData;
            desc.dataSizeInBytes = material.sizeInBytes;
            m_pDefaultMaterial = this->CreateMaterialResourceFromDesc(desc, nullptr, "");
        }
    }

    void GraphicsAssetResourceManager::LoadDefaultModel()
    {
        // The default model is made up of a single mesh which is set to the default mesh.
        m_pDefaultModel = new GraphicsAssetResource_Model(nullptr);
        assert(m_pDefaultModel);

        m_pDefaultModel->_InsertMesh(m_pDefaultMesh);
    }

    void GraphicsAssetResourceManager::LoadDefaultMesh()
    {
        float pos0X = -1.0f; float pos0Y =  1.0f; float pos0Z =  1.0f;
        float pos1X =  1.0f; float pos1Y =  1.0f; float pos1Z =  1.0f;
        float pos2X = -1.0f; float pos2Y = -1.0f; float pos2Z =  1.0f;
        float pos3X =  1.0f; float pos3Y = -1.0f; float pos3Z =  1.0f;
        float pos4X =  1.0f; float pos4Y =  1.0f; float pos4Z = -1.0f;
        float pos5X = -1.0f; float pos5Y =  1.0f; float pos5Z = -1.0f;
        float pos6X =  1.0f; float pos6Y = -1.0f; float pos6Z = -1.0f;
        float pos7X = -1.0f; float pos7Y = -1.0f; float pos7Z = -1.0f;
        
        float tex0X =  0.0f; float tex0Y =  1.0f;
        float tex1X =  1.0f; float tex1Y =  1.0f;
        float tex2X =  0.0f; float tex2Y =  0.0f;
        float tex3X =  1.0f; float tex3Y =  0.0f;

        float nor0X =  0.0f; float nor0Y =  0.0f; float nor0Z =  1.0f;
        float nor1X =  0.0f; float nor1Y =  0.0f; float nor1Z = -1.0f;
        float nor2X =  0.0f; float nor2Y =  1.0f; float nor2Z =  0.0f;
        float nor3X =  0.0f; float nor3Y = -1.0f; float nor3Z =  0.0f;
        float nor4X =  1.0f; float nor4Y =  0.0f; float nor4Z =  0.0f;
        float nor5X = -1.0f; float nor5Y =  0.0f; float nor5Z =  0.0f;


        MeshBuilderP3T2N3 builder;

        // Front
        {
            builder.EmitVertex(pos0X, pos0Y, pos0Z, tex0X, tex0Y, nor0X, nor0Y, nor0Z);
            builder.EmitVertex(pos1X, pos1Y, pos1Z, tex1X, tex1Y, nor0X, nor0Y, nor0Z);
            builder.EmitVertex(pos2X, pos2Y, pos2Z, tex2X, tex2Y, nor0X, nor0Y, nor0Z);

            builder.EmitVertex(pos2X, pos2Y, pos2Z, tex2X, tex2Y, nor0X, nor0Y, nor0Z);
            builder.EmitVertex(pos1X, pos1Y, pos1Z, tex1X, tex1Y, nor0X, nor0Y, nor0Z);
            builder.EmitVertex(pos3X, pos3Y, pos3Z, tex3X, tex3Y, nor0X, nor0Y, nor0Z);
        }

        // Back
        {
            builder.EmitVertex(pos4X, pos4Y, pos4Z, tex0X, tex0Y, nor1X, nor1Y, nor1Z);
            builder.EmitVertex(pos5X, pos5Y, pos5Z, tex1X, tex1Y, nor1X, nor1Y, nor1Z);
            builder.EmitVertex(pos6X, pos6Y, pos6Z, tex2X, tex2Y, nor1X, nor1Y, nor1Z);

            builder.EmitVertex(pos6X, pos6Y, pos6Z, tex2X, tex2Y, nor1X, nor1Y, nor1Z);
            builder.EmitVertex(pos5X, pos5Y, pos5Z, tex1X, tex1Y, nor1X, nor1Y, nor1Z);
            builder.EmitVertex(pos7X, pos7Y, pos7Z, tex3X, tex3Y, nor1X, nor1Y, nor1Z);
        }

        // Top
        {
            builder.EmitVertex(pos5X, pos5Y, pos5Z, tex0X, tex0Y, nor2X, nor2Y, nor2Z);
            builder.EmitVertex(pos4X, pos4Y, pos4Z, tex1X, tex1Y, nor2X, nor2Y, nor2Z);
            builder.EmitVertex(pos0X, pos0Y, pos0Z, tex2X, tex2Y, nor2X, nor2Y, nor2Z);

            builder.EmitVertex(pos0X, pos0Y, pos0Z, tex2X, tex2Y, nor2X, nor2Y, nor2Z);
            builder.EmitVertex(pos4X, pos4Y, pos4Z, tex1X, tex1Y, nor2X, nor2Y, nor2Z);
            builder.EmitVertex(pos1X, pos1Y, pos1Z, tex3X, tex3Y, nor2X, nor2Y, nor2Z);
        }

        // Bottom
        {
            builder.EmitVertex(pos2X, pos2Y, pos2Z, tex0X, tex0Y, nor3X, nor3Y, nor3Z);
            builder.EmitVertex(pos3X, pos3Y, pos3Z, tex1X, tex1Y, nor3X, nor3Y, nor3Z);
            builder.EmitVertex(pos7X, pos7Y, pos7Z, tex2X, tex2Y, nor3X, nor3Y, nor3Z);

            builder.EmitVertex(pos7X, pos7Y, pos7Z, tex2X, tex2Y, nor3X, nor3Y, nor3Z);
            builder.EmitVertex(pos3X, pos3Y, pos3Z, tex1X, tex1Y, nor3X, nor3Y, nor3Z);
            builder.EmitVertex(pos6X, pos6Y, pos6Z, tex3X, tex3Y, nor3X, nor3Y, nor3Z);
        }

        // Right
        {
            builder.EmitVertex(pos1X, pos1Y, pos1Z, tex0X, tex0Y, nor4X, nor4Y, nor4Z);
            builder.EmitVertex(pos4X, pos4Y, pos4Z, tex1X, tex1Y, nor4X, nor4Y, nor4Z);
            builder.EmitVertex(pos3X, pos3Y, pos3Z, tex2X, tex2Y, nor4X, nor4Y, nor4Z);

            builder.EmitVertex(pos3X, pos3Y, pos3Z, tex2X, tex2Y, nor4X, nor4Y, nor4Z);
            builder.EmitVertex(pos4X, pos4Y, pos4Z, tex1X, tex1Y, nor4X, nor4Y, nor4Z);
            builder.EmitVertex(pos6X, pos6Y, pos6Z, tex3X, tex3Y, nor4X, nor4Y, nor4Z);
        }

        // Left
        {
            builder.EmitVertex(pos5X, pos5Y, pos5Z, tex0X, tex0Y, nor5X, nor5Y, nor5Z);
            builder.EmitVertex(pos0X, pos0Y, pos0Z, tex1X, tex1Y, nor5X, nor5Y, nor5Z);
            builder.EmitVertex(pos7X, pos7Y, pos7Z, tex2X, tex2Y, nor5X, nor5Y, nor5Z);

            builder.EmitVertex(pos7X, pos7Y, pos7Z, tex2X, tex2Y, nor5X, nor5Y, nor5Z);
            builder.EmitVertex(pos0X, pos0Y, pos0Z, tex1X, tex1Y, nor5X, nor5Y, nor5Z);
            builder.EmitVertex(pos2X, pos2Y, pos2Z, tex3X, tex3Y, nor5X, nor5Y, nor5Z);
        }

        
        


        VertexAttribLayout vertexLayout[3];
        vertexLayout[0] = { VertexAttribFormat_Float, 3, sizeof(float) * 0, VertexAttribSemantic_Position };
        vertexLayout[1] = { VertexAttribFormat_Float, 2, sizeof(float) * 3, VertexAttribSemantic_TexCoord };
        vertexLayout[2] = { VertexAttribFormat_Float, 3, sizeof(float) * 5, VertexAttribSemantic_Normal   };

        uint32_t materialOffsetCountPair[2];
        materialOffsetCountPair[0] = 0;
        materialOffsetCountPair[1] = builder.GetIndexCount();

        HGraphicsResource hMaterialResource[1] = { 0 };
        if (m_pDefaultMaterial != nullptr)
        {
            hMaterialResource[0] = m_pDefaultMaterial->GetGraphicsResource();
        }

        GT::GraphicsMeshResourceDesc desc;
        desc.topology                      = PrimitiveTopologyType_Triangle;
        desc.pVertexData                   = builder.GetVertexData();
        desc.vertexDataSize                = builder.GetVertexSizeInFloats() * sizeof(float) * builder.GetVertexCount();
        desc.vertexStride                  = sizeof(float) * (3 + 2 + 3);
        desc.pVertexLayout                 = vertexLayout;
        desc.vertexAttribCount             = 3;
        desc.pIndexData                    = builder.GetIndexData();
        desc.indexDataSize                 = builder.GetIndexCount() * sizeof(uint32_t);
        desc.indexCount                    = builder.GetIndexCount();
        desc.indexFormat                   = IndexFormat_UInt32;
        desc.materialIndexOffsetCountPairs = materialOffsetCountPair;
        desc.materialCount                 = 1;
        desc.materials                     = hMaterialResource;
        m_pDefaultMesh = this->CreateMeshResourceFromDesc(desc);
        if (m_pDefaultMesh != nullptr)
        {
            m_pDefaultMesh->_InsertMaterial(m_pDefaultMaterial);
        }
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


    void GraphicsAssetResourceManager::GetMaterialBaseDirectory(const char* materialAbsolutePath, char* basePath, unsigned int basePathSize) const
    {
        // The base path for materials need special treatment because we cannot have the base path set to a .mtl archive file.

        easypath_copybasepath(materialAbsolutePath, basePath, basePathSize);

        if (easypath_extensionequal(basePath, "mtl"))
        {
            easypath_basepath(basePath);
        }
    }

    void GraphicsAssetResourceManager::GetModelBaseDirectory(const char* modelAbsolutePath, char* basePath, unsigned int basePathSize) const
    {
        easypath_copybasepath(modelAbsolutePath, basePath, basePathSize);
    }
}
