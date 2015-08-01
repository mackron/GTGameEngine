// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_EngineContext_hpp_
#define __GT_GE_EngineContext_hpp_

#include <GTLib/ResultCodes.hpp>
#include <GTLib/CommandLine.hpp>
#include "HardwarePlatform.hpp"
#include "FileSystem.hpp"
#include "Graphics/GraphicsTypes.hpp"
#include "Assets/AssetLibrary.hpp"

namespace GT
{
    class GPURenderingDevice;	// TODO: Remove this.

	class GraphicsInterface;
	class GraphicsInterfaceAllocator;


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

#if defined(GT_BUILD_OPENGL)
        /// Retrieves a pointer to the OpenGL graphics API object.
        GraphicsAPI_OpenGL* GetOpenGLGraphicsAPI();
#endif
#if defined(GT_BUILD_VULKAN)
        /// Retrieves a pointer to the Vulkan graphics API object.
        GraphicsAPI_Vulkan* GetVulkanGraphicsAPI();
#endif
#if defined(GT_BUILD_D3D12)
        /// Retrieves a pointer to the Vulkan graphics API object.
        GraphicsAPI_D3D12* GetD3D12GraphicsAPI();
#endif

		/// Registers a graphics interface allocator.
		///
		/// @param allocator [in] A reference to the graphics interface allocator to register.
		///
		/// @remarks
		///		Once registered, an allocator cannot be unregistered.
		///		@par
		///		Allocators are registered such that the most recent one is given the highest priority.
		///		@par
		///		The default allocator is the first one to be registered, but it is only registered if there is at least 1 built-in graphics interface enabled in the build.
		void RegisterGraphicsInterfaceAllocator(GraphicsInterfaceAllocator &allocator);

        /// Creates an instance of a graphics interface of the given type.
        ///
        /// @param type [in] The graphics interface type.
        ///
        /// @return A pointer to the new graphics interface, or null if the graphics interface is not supported.
        GraphicsInterface* CreateGraphicsInterface(GraphicsInterfaceType type);

        /// Deletes the given graphics interface.
        ///
        /// @param pGraphicsInterface [in] A pointer to the graphics interface to delete.
        void DeleteGraphicsInterface(GraphicsInterface* pGraphicsInterface);



        /// Retrieves the number of GPU rendering devices available to the application.
        ///
        /// @return The number of GPU devices that are available to the application for rendering.
        unsigned int GetGPURenderingDeviceCount() const;

        /// Retrieves the GPU rendering device info for the device at the given index.
        ///
        /// @param deviceIndex   [in]  The index of the device whose information is being retrieved.
        /// @param deviceInfoOut [out] A reference to the object that will receive the device info.
        ///
        /// @return If an error occurs, the return value will be <0. Otherwise, the return value will be >=0.
        ResultCode GetGPURenderingDeviceInfo(unsigned int deviceIndex, GPURenderingDeviceInfo &deviceInfoOut) const;


        /// @copydoc GT::GE::HardwarePlatform_GPU::CreateGPURenderingDevice()
        GPURenderingDevice* CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount);

        /// Helper for creating a GPU rendering device for the given rendering API.
        ///
        /// @param deviceIndex  [in]  The index of the device to create the new device instance from.
        /// @param renderingAPI [in]  The rendering API to use.
        ///
        /// @return A pointer to the new device, or null if an error occured.
        GPURenderingDevice* CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPI);

        /// Helper for creating a GPU rendering device with the best available rendering API.
        ///
        /// @param deviceIndex  [in]  The index of the device to create the new device instance from.
        /// @param deviceOut    [out] A reference to the variable that will receive the new device object.
        ///
        /// @return A pointer to the new device, or null if an error occured.
        GPURenderingDevice* CreateGPURenderingDevice(unsigned int deviceIndex);

        /// The highest level helper for creating a GPU rendering device.
        ///
        /// @return A pointer to the GPU rendering device, or null if an error occurs.
        ///
        /// @remarks
        ///     This will create the rendering device from the primary adapter (device index of 0) and will use the best available rendering API.
        GPURenderingDevice* CreateGPURenderingDevice();


        /// Deletes a GPU rendering device object that was created with DeleteGPURenderingDevice.
        ///
        /// @param renderingDevice [in] A pointer to the GPU device to delete.
        void DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice);



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


    private:





    private:

        /// An object representing the command line that was used to initialise the game engine.
        GTLib::CommandLine m_commandLine;

        /// The hardware platform object for detecting the available hardware on the running system. This object can be used to iterate over devices
        /// and specify which ones should be used for particular tasks.
        HardwarePlatform m_hardwarePlatform;


		/// The list of graphics interface allocators.
		GTLib::Vector<GraphicsInterfaceAllocator*> m_graphicsInterfaceAllocators;

        /// A pointer to the default graphics interface allocator. This will be null if the default allocator is not being used. An application may
        /// chose to exclude the default allocator so that they can implement custom graphics interface.
        GraphicsInterfaceAllocator* m_pDefaultGraphicsInterfaceAllocator;


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


        


    private:    // No copying.
        EngineContext(const EngineContext &);
        EngineContext & operator=(const EngineContext &);
    };
}


#endif
