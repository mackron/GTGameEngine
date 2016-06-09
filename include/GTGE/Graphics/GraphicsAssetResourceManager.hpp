// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GraphicsAssetResourceManager
#define GT_GraphicsAssetResourceManager

#include "GraphicsAssetResources.hpp"
#include "../Assets/AssetTypes.hpp"

#include <dr_libs/dr.h>

namespace GT
{
    class GraphicsWorld;
    class AssetLibrary;

    /// Class for managing the creation and deletion of graphics resources.
    class GraphicsAssetResourceManager
    {
    public:

        /// Constructor.
        GraphicsAssetResourceManager(GraphicsWorld &graphicsWorld, AssetLibrary &assetLibrary);

        /// Destructor.
        ~GraphicsAssetResourceManager();


        /// Retrieves a reference to the graphics world.
        GraphicsWorld & GetGraphicsWorld() const;

        /// Retrieves a reference to the asset library that is being used to load the assets.
        AssetLibrary & GetAssetLibrary() const;


        /// Initializes the resource manager by loading default resources.
        bool Startup();

        /// Shuts down the resource manager by unloaded every loaded resource.
        void Shutdown();


        /// Loads a texture.
        ///
        /// @remarks
        ///     If there is an error loading the asset, the default texture is returned.
        GraphicsAssetResource_Texture* LoadTexture(const char* assetPath);

        /// Loads a material.
        ///
        /// @remarks
        ///     If there is an error loading the asset, the default material is returned.
        GraphicsAssetResource_Material* LoadMaterial(const char* assetPath);

        /// Loads a model.
        ///
        /// @remarks
        ///     If there is an error loading the asset, the default model is returned.
        GraphicsAssetResource_Model* LoadModel(const char* assetPath);

        /// Loads a resource from a pre-loaded asset.
        ///
        /// @remarks
        ///     The asset must have been created by the asset library that was passed to the constructor of this object.
        GraphicsAssetResource* Load(Asset* pAsset);
        GraphicsAssetResource* Load(Asset &asset) { return Load(&asset); }


        /// Unloads a resource by it;s path.
        void Unload(const char* assetPath);

        /// Unloads a resource.
        void Unload(GraphicsAssetResource* pResource);


        /// Finds an already-loaded resource by it's path.
        GraphicsAssetResource* FindLoadedResourceByPath(const char* assetPath, OUT char* absolutePath, size_t absolutePathSize);


        /// Retrieves the default texture.
        GraphicsAssetResource_Texture* GetDefaultTexture();

        /// Retrieves the default material.
        GraphicsAssetResource_Material* GetDefaultMaterial();

        /// Retrieves the default model.
        GraphicsAssetResource_Model* GetDefaultModel();

        /// Retrieves the default mesh.
        GraphicsAssetResource_Mesh* GetDefaultMesh();


    private:

        /// Loads a texture.
        GraphicsAssetResource_Texture* LoadTexture(Asset* pAsset);

        /// Loads a material.
        GraphicsAssetResource_Material* LoadMaterial(Asset* pAsset);

        /// Loads a model.
        GraphicsAssetResource_Model* LoadModel(Asset* pAsset);

        /// Loads a mesh from a mesh descriptor.
        GraphicsAssetResource_Mesh* LoadMesh(GraphicsMeshResourceDesc &desc);


        /// Unloads a resource by it's index.
        void UnloadByIndex(size_t resourceIndex);

        /// Unloads the given texture.
        void UnloadTexture(GraphicsAssetResource_Texture* pTexture);

        /// Unloads the given material
        void UnloadMaterial(GraphicsAssetResource_Material* pMaterial);

        /// Unloads the given model
        void UnloadModel(GraphicsAssetResource_Model* pModel);

        /// Unloads the given mesh.
        void UnloadMesh(GraphicsAssetResource_Mesh* pMesh);


        /// Creates a texture resource from a descriptor.
        ///
        /// @remarks
        ///     This does not add the new resource to the internal list.
        GraphicsAssetResource_Texture* CreateTextureResourceFromDesc(GraphicsTextureResourceDesc &desc, Asset* pAsset);

        /// Creates a material resource from a descriptor.
        ///
        /// @param basePath [in] The base path of the material so that textures can be loaded relative to it.
        ///
        /// @remarks
        ///     This does not add the new resource to the internal list.
        GraphicsAssetResource_Material* CreateMaterialResourceFromDesc(GraphicsMaterialResourceDesc &desc, Asset* pAsset, const char* basePath);

        /// Creates a mesh resource from a descriptor.
        ///
        /// @param basePath [in] The base path of the model so that materials can be loaded relative to it.
        ///
        /// @remarks
        ///     This does not add the new resource to the internal list.
        GraphicsAssetResource_Mesh* CreateMeshResourceFromDesc(GraphicsMeshResourceDesc &desc);


        /// Finds the index of the given resource.
        bool FindResourceIndex(GraphicsAssetResource* pResource, size_t &indexOut) const;

        /// Finds the index of the resource by it's path.
        bool FindResourceIndexByPath(const char* assetPath, OUT size_t &indexOut, OUT char* absolutePath, size_t absolutePathSize) const;

        /// Finds an already-loaded resource, and if it doesn't exist, loads the asset.
        GraphicsAssetResource* FindExistingResourceOrLoadAsset(const char* assetPath, AssetClass expectedAssetClass, OUT Asset* &pAsset);


        /// Determines whether or not the given resource is a default one.
        bool IsDefaultResource(GraphicsAssetResource* pResource);

        /// Loads the default resources.
        void LoadDefaultResources();

        /// Loads the default texture.
        void LoadDefaultTexture();

        /// Loads the default material.
        void LoadDefaultMaterial();

        /// Loads the default model.
        void LoadDefaultModel();

        /// Loads the default mesh.
        void LoadDefaultMesh();

        /// Unloads the default resources.
        void UnloadDefaultResources();

        /// Unloads the default texture.
        void UnloadDefaultTexture();

        /// Unloads the default material.
        void UnloadDefaultMaterial();

        /// Unloads the default model.
        void UnloadDefaultModel();

        /// Unloads the default mesh.
        void UnloadDefaultMesh();


        /// Helper function for retrieving the base directory of the given material's path.
        ///
        /// @param materialAbsolutePath [in] The absolute path of the material.
        ///
        /// @remarks
        ///     The purpose of this function is to make it possible to load a texture relative to the material. The problem, however, is that
        ///     Wavefront MTL files are contained in .mtl files which are virtualized as archive files - that cannot be used as the base path,
        ///     so we need to check for it explicitly.
        void GetMaterialBaseDirectory(const char* materialAbsolutePath, char* basePath, unsigned int basePathSize) const;

        /// Helper for retrieving the base directory of the given model's path.
        ///
        /// @param modelAbsolutePath [in] The absolute path of the model.
        /// @param basePath [out] A pointer to the buffer that will receive the base path.
        /// @param basePathSize [in] The size of the buffer pointed to by basePath.
        void GetModelBaseDirectory(const char* modelAbsolutePath, char* basePath, unsigned int basePathSize) const;


    private:

        /// A reference to teh graphics world.
        GraphicsWorld &m_graphicsWorld;

        /// A reference to the asset library that the assets will be loaded from.
        AssetLibrary &m_assetLibrary;

        /// The dictionary that maps an asset's path to it's resource object.
        Dictionary<GraphicsAssetResource*> m_loadedResources;


        /// The default texture.
        GraphicsAssetResource_Texture* m_pDefaultTexture;

        /// The default material.
        GraphicsAssetResource_Material* m_pDefaultMaterial;

        /// The default model.
        GraphicsAssetResource_Model* m_pDefaultModel;

        /// The default mesh.
        GraphicsAssetResource_Mesh* m_pDefaultMesh;
    };
}

#endif
