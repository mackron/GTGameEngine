// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EngineContext
#define GT_EngineContext


#include <GTLib/ResultCodes.hpp>
#include <GTLib/CommandLine.hpp>
#include "Config.hpp"
#include "FileSystem.hpp"
#include "Graphics/GraphicsTypes.hpp"
#include "Assets/AssetLibrary.hpp"
#include "Scene/SceneNodeComponentDescriptorLibrary.hpp"

#if defined(GT_BUILD_COMPONENT_EVENTS)
#include "Scene/SceneNodeComponentDescriptor_Events.hpp"
#endif
#if defined(GT_BUILD_COMPONENT_SCRIPT)
//#include "Scene/SceneNodeComponentDescriptor_Script.hpp"
#endif
#if defined(GT_ENABLE_COMPONENT_GRAPHICS)
#include "Scene/SceneNodeComponentDescriptor_Graphics.hpp"
#endif
#if defined(GT_BUILD_COMPONENT_DYNAMICS)
//#include "Scene/SceneNodeComponentDescriptor_Dynamics.hpp"
#endif

namespace GT
{
    class GraphicsAPI;
    class GraphicsAPI_Null;

#if defined(GT_BUILD_OPENGL)
    class GraphicsAPI_OpenGL;
#endif
#if defined(GT_BUILD_VULKAN)
    class GraphicsAPI_Vulkan;
#endif
#if defined(GT_BUILD_D3D12)
    class GraphicsAPI_D3D12;
#endif


    /// Class representing the engine context.
    ///
    /// Only a single engine context is usually needed for an application, however it is possible to have more.
    class EngineContext
    {
    public:

        /// Constructor.
        EngineContext();

        /// Destructor.
        ~EngineContext();


        /// Starts up the engine context.
        ///
        /// @param argc [in] The number of arguments passed to the command line (same as the argc parameter of main())
        /// @param argv [in] The array of strings making up the command line (same as the argv parameter of main())
        ///
        /// @return A result code specifying whether or not the context was initialized successfully. If the return value is >= 0, it was successful, otherwise it failed.
        ///
        /// @remarks
        ///     The argc and argv parameters should be passed exactly as they were passed from main().
        ///     @par
        ///     This sets the global current directory to that of the directory where the executable is located.
        ResultCode Startup(int argc, char** argv);

        /// Shuts down the engine engine.
        ///
        /// @remarks
        ///     This does not delete the context object.
        void Shutdown();


        /// Retrieves a reference to the object representing the command line.
        ///
        /// @return A reference to the command line object.
        const GTLib::CommandLine & GetCommandLine() const;


        /// Retrieves a pointer to the best graphics API object.
        ///
        /// @remarks
        ///     The prioritization is as follows:
        ///         - Vulkan (Not yet implemented)
        ///         - D3D12 (Not yet implemented)
        ///         - OpenGL
        ///         - Null
        GraphicsAPI* GetBestGraphicsAPI();

        /// Retrieves a pointer to the null graphics API object.
        GraphicsAPI_Null* GetNullGraphicsAPI();

#if defined(GT_BUILD_VULKAN)
        /// Retrieves a pointer to the Vulkan graphics API object.
        GraphicsAPI_Vulkan* GetVulkanGraphicsAPI();
#endif
#if defined(GT_BUILD_D3D12)
        /// Retrieves a pointer to the Vulkan graphics API object.
        GraphicsAPI_D3D12* GetD3D12GraphicsAPI();
#endif
#if defined(GT_BUILD_OPENGL)
        /// Retrieves a pointer to the OpenGL graphics API object.
        GraphicsAPI_OpenGL* GetOpenGLGraphicsAPI();
#endif



        /// Retrieves a reference to the file system.
        ///
        /// @return A reference to the file system.
        FileSystem & GetFileSystem();



        ////////////////////////////////////////////////////////////////////
        // Asset Management

        /// Retrieves a reference to the asset library.
        ///
        /// @return A reference to the asset library.
        AssetLibrary & GetAssetLibrary();

        /// @copydoc AssetLibrary::Load(const char*, AssetType)
        Asset* LoadAsset(const char* filePathOrIdentifier, AssetType explicitAssetType = AssetType_Unknown);

        /// @copydoc AssetLibrary::Load(Asset*)
        Asset* LoadAsset(Asset* pAsset);

        /// @copydoc AssetLibrary::Unload()
        void UnloadAsset(Asset* pAsset);

        /// @copydoc AssetLibrary::Reload()
        void ReloadAsset(const char* filePathOrIdentifier);

        /// @copydoc AssetLibrary::RegisterAssetAllocator()
        void RegisterAssetAllocator(AssetAllocator &allocator);



        ////////////////////////////////////////////////////////////////////
        // Scene Node Component Management

        /// @copydoc SceneNodeComponentDescriptorLibrary::RegisterDescriptor()
        void RegisterSceneNodeComponentDescriptor(const SceneNodeComponentDescriptor &descriptor);

        /// @copydoc SceneNodeComponentDescriptorLibrary::GetDescriptorByType()
        const SceneNodeComponentDescriptor* GetSceneNodeDescriptorByType(SceneNodeComponentTypeID type) const;

        /// @copydoc SceneNodeComponentDescriptorLibrary::CreateComponent()
        SceneNodeComponent* CreateSceneNodeComponent(SceneNodeComponentTypeID type);

        template <typename T>
        T* CreateSceneNodeComponent()
        {
            return reinterpret_cast<T*>(CreateSceneNodeComponent(T::GetTypeID()));
        }


        /// @copydoc SceneNodeComponentDescriptorLibrary::DeleteComponent()
        void DeleteSceneNodeComponent(SceneNodeComponent* pComponent);



    private:

        /// An object representing the command line that was used to initialise the game engine.
        GTLib::CommandLine m_commandLine;


        // Supported Graphics APIs.

        /// A pointer to the null graphics API. This API does nothing and is intended for a placeholder for when graphics are not needed
        /// or when none of the APIs are supported.
        GraphicsAPI_Null* m_pGraphicsAPI_Null;

#if defined(GT_BUILD_OPENGL)
        /// A pointer to the OpenGL graphics API object. If OpenGL is not supported, this will be set to null.
        GraphicsAPI_OpenGL* m_pGraphicsAPI_OpenGL;
#endif
#if defined(GT_BUILD_VULKAN)
        /// A pointer to the Vulkan graphics API object. If Vulkan is not supported, this will be set to null.
        GraphicsAPI_Vulkan* m_pGraphicsAPI_Vulkan;
#endif
#if defined(GT_BUILD_D3D12)
        /// A pointer to the D3D12 graphics API object. If D3D12 is not supported, this will be set to null.
        GraphicsAPI_D3D12* m_pGraphicsAPI_D3D12;
#endif


        /// The virtual file system for handling file reading and writing.
        FileSystem m_fileSystem;


        /// The asset library for managing assets.
        AssetLibrary m_assetLibrary;

        /// A pointer to the default asset allocator. This will be null if the default asset allocator is not being used. An application may choose to
        /// exclude the default allocator so that they can do custom asset loading.
        AssetAllocator* m_pDefaultAssetAllocator;


#if defined(GT_BUILD_COMPONENT_EVENTS)
        /// The Events component descriptor.
        SceneNodeComponentDescriptor_Events m_eventsComponentDescriptor;
#endif
#if defined(GT_BUILD_COMPONENT_SCRIPT)
        /// The Script component descriptor.
        //SceneNodeComponentDescriptor_Script m_scriptComponentDescriptor;
#endif
#if defined(GT_ENABLE_COMPONENT_GRAPHICS)
        /// The Graphics component descriptor.
        SceneNodeComponentDescriptor_Graphics m_graphicsComponentDescriptor;
#endif
#if defined(GT_BUILD_COMPONENT_DYNAMICS)
        /// The Dynamics component descriptor.
        //SceneNodeComponentDescriptor_Dynamics m_dynamicsComponentDescriptor;
#endif

        /// The list of registered scene node component descriptors.
        SceneNodeComponentDescriptorLibrary m_componentLibrary;


    private:    // No copying.
        EngineContext(const EngineContext &);
        EngineContext & operator=(const EngineContext &);
    };
}


#endif
