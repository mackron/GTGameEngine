// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsAssetResources.hpp>
#include <GTGameEngine/Assets/ImageAsset.hpp>
#include <GTGameEngine/Assets/MaterialAsset.hpp>
#include <GTGameEngine/Assets/ModelAsset.hpp>

namespace GT
{
    GraphicsAssetResource::GraphicsAssetResource(Asset* pAsset)
        : m_pAsset(pAsset), m_referenceCount(1)
    {
    }


    Asset* GraphicsAssetResource::GetAsset() const
    {
        return m_pAsset;
    }

    size_t GraphicsAssetResource::GetReferenceCount() const
    {
        return m_referenceCount;
    }

    size_t GraphicsAssetResource::IncrementReferenceCount()
    {
        m_referenceCount += 1;
        return m_referenceCount;
    }

    size_t GraphicsAssetResource::DecrementReferenceCount() 
    {
        assert(m_referenceCount > 0);

        m_referenceCount -= 1;
        return m_referenceCount;
    }




    ///////////////////////////////////////////
    // Textures

    GraphicsAssetResource_Texture::GraphicsAssetResource_Texture(Asset* pAsset, HGraphicsResource hGraphicsResource)
        : GraphicsAssetResource(pAsset), m_hGraphicsResource(hGraphicsResource)
    {
        assert(pAsset != nullptr);
        assert(pAsset->GetClass() == AssetClass_Image);
    }

    HGraphicsResource GraphicsAssetResource_Texture::GetGraphicsResource() const
    {
        return m_hGraphicsResource;
    }



    ///////////////////////////////////////////
    // Materials

    GraphicsAssetResource_Material::GraphicsAssetResource_Material(Asset* pAsset, HGraphicsResource hGraphicsResource)
        : GraphicsAssetResource(pAsset), m_hGraphicsResource(hGraphicsResource), m_textures()
    {
        assert(pAsset != nullptr);
        assert(pAsset->GetClass() == AssetClass_Material);
    }

    HGraphicsResource GraphicsAssetResource_Material::GetGraphicsResource() const
    {
        return m_hGraphicsResource;
    }

    size_t GraphicsAssetResource_Material::GetTextureCount() const
    {
        return m_textures.GetCount();
    }

    GraphicsAssetResource_Texture* GraphicsAssetResource_Material::GetTextureByIndex(size_t index) const
    {
        return m_textures[index];
    }


    size_t GraphicsAssetResource_Material::GetInputTextureCount() const
    {
        return m_inputTextures.count;
    }

    GraphicsAssetResource_Texture* GraphicsAssetResource_Material::GetInputTextureByIndex(size_t index) const
    {
        return m_textures[m_inputTextures.buffer[index]->value];
    }

    GraphicsAssetResource_Texture* GraphicsAssetResource_Material::GetInputTextureByName(const char* inputName) const
    {
        auto iTexture = m_inputTextures.Find(inputName);
        if (iTexture != nullptr)
        {
            return m_textures[iTexture->value];
        }

        return nullptr;
    }


    size_t GraphicsAssetResource_Material::GetPropertyTextureCount() const
    {
        return m_propertyTextures.count;
    }

    GraphicsAssetResource_Texture* GraphicsAssetResource_Material::GetPropertyTextureByIndex(size_t index) const
    {
        return m_textures[m_propertyTextures.buffer[index]->value];
    }

    GraphicsAssetResource_Texture* GraphicsAssetResource_Material::GetPropertyTextureByName(const char* propertyName) const
    {
        auto iTexture = m_propertyTextures.Find(propertyName);
        if (iTexture != nullptr)
        {
            return m_textures[iTexture->value];
        }

        return nullptr;
    }


    void GraphicsAssetResource_Material::_InsertInputTexture(const char* inputName, GraphicsAssetResource_Texture* pTexture)
    {
        m_textures.PushBack(pTexture);
        m_inputTextures.Add(inputName, m_textures.GetCount() - 1);
    }

    void GraphicsAssetResource_Material::_InsertPropertyTexture(const char* propertyName, GraphicsAssetResource_Texture* pTexture)
    {
        m_textures.PushBack(pTexture);
        m_propertyTextures.Add(propertyName, m_textures.GetCount() - 1);
    }



    ///////////////////////////////////////////
    // Meshes

    GraphicsAssetResource_Mesh::GraphicsAssetResource_Mesh(HGraphicsResource hGraphicsResource)
        : GraphicsAssetResource(nullptr), m_hGraphicsResource(hGraphicsResource), m_materials()
    {
    }

    HGraphicsResource GraphicsAssetResource_Mesh::GetGraphicsResource() const
    {
        return m_hGraphicsResource;
    }

    size_t GraphicsAssetResource_Mesh::GetMaterialCount() const
    {
        return m_materials.GetCount();
    }

    GraphicsAssetResource_Material* GraphicsAssetResource_Mesh::GetMaterialByIndex(size_t index) const
    {
        return m_materials[index];
    }


    void GraphicsAssetResource_Mesh::_InsertMaterial(GraphicsAssetResource_Material* pMaterial)
    {
        m_materials.PushBack(pMaterial);
    }



    ///////////////////////////////////////////
    // Models

    GraphicsAssetResource_Model::GraphicsAssetResource_Model(Asset* pAsset)
        : GraphicsAssetResource(pAsset), m_meshes()
    {
        assert(pAsset != nullptr);
        assert(pAsset->GetClass() == AssetClass_Model);
    }

    size_t GraphicsAssetResource_Model::GetMeshCount() const
    {
        return m_meshes.GetCount();
    }

    GraphicsAssetResource_Mesh* GraphicsAssetResource_Model::GetMeshByIndex(size_t index) const
    {
        return m_meshes[index];
    }


    void GraphicsAssetResource_Model::_InsertMesh(GraphicsAssetResource_Mesh* pMesh)
    {
        m_meshes.PushBack(pMesh);
    }
}
