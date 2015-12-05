// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GraphicsAssetResources
#define GT_GraphicsAssetResources

#include "GraphicsWorld.hpp"
#include <GTGE/Core/Dictionary.hpp>
#include <GTGE/Core/Vector.hpp>

namespace GT
{
    class Asset;
    class GraphicsAssetResourceManager;

    /// Base class for tying graphics resources to assets.
    class GraphicsAssetResource
    {
    protected:

        /// Constructor.
        GraphicsAssetResource(GraphicsAssetResourceManager &resourceManager, Asset* pAsset);


    public:

        /// Retrieves the resource manager that owns the asset.
        GraphicsAssetResourceManager & GetResourceManager();

        /// Retrieves a pointer to the asset.
        Asset* GetAsset() const;

        /// Retrieves the current reference count.
        size_t GetReferenceCount() const;

        /// Increments the reference counter, returning the new value.
        size_t IncrementReferenceCount();

        /// Decremenets the reference counter, returning the new value.
        size_t DecrementReferenceCount();


    private:

        /// A reference to the resource manager that created the resource.
        GraphicsAssetResourceManager &m_resourceManager;

        /// A pointer to the associated asset.
        Asset* m_pAsset;

        /// The reference counter. Defaults to 1. When this hits zero it should be deleted by the higher level manager.
        size_t m_referenceCount;
    };



    ///////////////////////////////////////////
    // Textures

    class GraphicsAssetResource_Texture : public GraphicsAssetResource
    {
    public:

        /// Constructor.
        GraphicsAssetResource_Texture(GraphicsAssetResourceManager &resourceManager, Asset* pAsset, HGraphicsResource hGraphicsResource);

        /// Retrieves a handle to the associated graphics resource.
        HGraphicsResource GetGraphicsResource() const;


    private:

        /// A handle to the graphics resource that was created by the graphics world. This is set once via the constructor.
        HGraphicsResource m_hGraphicsResource;
    };



    ///////////////////////////////////////////
    // Material

    class GraphicsAssetResource_Material : public GraphicsAssetResource
    {
    public:

        /// Constructor.
        GraphicsAssetResource_Material(GraphicsAssetResourceManager &resourceManager, Asset* pAsset, HGraphicsResource hGraphicsResource);

        /// Retrieves a handle to the associated graphics resource.
        HGraphicsResource GetGraphicsResource() const;


        /// Retrieves the number of textures this material references.
        size_t GetTextureCount() const;

        /// Retrieves a pointer to the texture asset resource at the given index.
        GraphicsAssetResource_Texture* GetTextureByIndex(size_t index) const;


        /// Retrieves the number of textures that are mapped to input variables.
        size_t GetInputTextureCount() const;

        /// Retrieves a texture that is mapped to an input variable by it's index.
        GraphicsAssetResource_Texture* GetInputTextureByIndex(size_t index) const;

        /// Retrieves a pointer to the texture asset resource that is mapped to an input variable.
        GraphicsAssetResource_Texture* GetInputTextureByName(const char* inputName) const;


        /// Retrieves the number of textures that are mapped to properties.
        size_t GetPropertyTextureCount() const;

        /// Retrieves a texture that is mapped to a property by it's index.
        GraphicsAssetResource_Texture* GetPropertyTextureByIndex(size_t index) const;

        /// Retrieves a pointer to the texture asset resource that is mapped to a property.
        GraphicsAssetResource_Texture* GetPropertyTextureByName(const char* propertyName) const;



        /// [Internal Use Only] Inserts a texture resource that is mapped to an input variable.
        void _InsertInputTexture(const char* inputName, GraphicsAssetResource_Texture* pTexture);

        /// [Internal Use Only] Inserts a texture resource that is mapped to a property.
        void _InsertPropertyTexture(const char* propertyName, GraphicsAssetResource_Texture* pTexture);


    private:

        /// A handle to the graphics resource that was created by the graphics world. This is set once via the constructor.
        HGraphicsResource m_hGraphicsResource;

        /// The list of texture resources that the material references.
        Vector<GraphicsAssetResource_Texture*> m_textures;

        /// The input texture mappings. The value is an index into m_textures.
        Dictionary<size_t> m_inputTextures;

        /// The property texture mappints. The value is an index into m_textures.
        Dictionary<size_t> m_propertyTextures;
    };



    ///////////////////////////////////////////
    // Meshes

    class GraphicsAssetResource_Mesh : public GraphicsAssetResource
    {
    public:

        /// Constructor.
        GraphicsAssetResource_Mesh(GraphicsAssetResourceManager &resourceManager, HGraphicsResource hGraphicsResource);

        /// Retrieves a handle to the associated graphics resource.
        HGraphicsResource GetGraphicsResource() const;


        /// Retrieves the number of materials this mesh references.
        size_t GetMaterialCount() const;

        /// Retrieves a pointer to the material asset resource at the given index.
        GraphicsAssetResource_Material* GetMaterialByIndex(size_t index) const;


        /// [Internal Use Only] Appends a material resource that is used by a material group.
        void _InsertMaterial(GraphicsAssetResource_Material* pMaterial);



    private:

        /// A handle to the graphics resource that was created by the graphics world. This is set once via the constructor.
        HGraphicsResource m_hGraphicsResource;

        /// The list of material resources that the mesh references.
        Vector<GraphicsAssetResource_Material*> m_materials;
    };



    ///////////////////////////////////////////
    // Models

    class GraphicsAssetResource_Model : public GraphicsAssetResource
    {
    public:

        /// Constructor.
        GraphicsAssetResource_Model(GraphicsAssetResourceManager &resourceManager, Asset* pAsset);


        /// Retrieves the number of meshes this model references.
        size_t GetMeshCount() const;

        /// Retrieves a pointer to the mesh asset resource at the given index.
        GraphicsAssetResource_Mesh* GetMeshByIndex(size_t index) const;


        /// [Internal Use Only] Appends a mesh resource.
        void _InsertMesh(GraphicsAssetResource_Mesh* pMesh);


    private:

        /// The list of mesh resources that the mesh references.
        Vector<GraphicsAssetResource_Mesh*> m_meshes;
    };
}

#endif
