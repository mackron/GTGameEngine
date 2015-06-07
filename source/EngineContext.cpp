// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/Config.hpp>
#include <GTGameEngine/Graphics/GraphicsInterface.hpp>

#include <GTLib/CommandLine.hpp>
#include <GTLib/IO.hpp>

#include "Graphics/DefaultGraphicsInterfaceAllocator.hpp"
#include "Assets/DefaultAssetAllocator.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTGameEngine/WindowManager_Win32.hpp>
#endif

namespace GT
{
    EngineContext::EngineContext(WindowManager* pWindowManager)
        : m_commandLine(),
          m_hardwarePlatform(),
          m_fileSystem(),
          m_pWindowManager(pWindowManager), m_ownsWindowManager(false),
          m_assetLibrary(m_fileSystem),
          m_pDefaultAssetAllocator(nullptr)
    {
        if (m_pWindowManager == nullptr)
        {
#if defined (GT_PLATFORM_WINDOWS)
            m_pWindowManager = new WindowManager_Win32;
#endif
#if defined (GT_PLATFORM_LINUX)
            m_pWindowManager = new WindowManager_X11;
#endif

            m_ownsWindowManager = true;
        }
    }

    EngineContext::~EngineContext()
    {
        delete m_pDefaultGraphicsInterfaceAllocator;
    }


    ResultCode EngineContext::Startup(int argc, char** argv)
    {
        ResultCode result = 0;


        // Command line.
        m_commandLine.Parse(argc, argv);

        // Make the global current directory that of the executable by default.
        GTLib::IO::SetCurrentDirectory(m_commandLine.GetApplicationDirectory());



#if defined (GT_BUILD_DEFAULT_GRAPHICS_INTERFACES)
        // Create and register the default graphics interface allocator.
        m_pDefaultGraphicsInterfaceAllocator = new DefaultGraphicsInterfaceAllocator();
        this->RegisterGraphicsInterfaceAllocator(*m_pDefaultGraphicsInterfaceAllocator);
#endif


        // Hardware platform.
        result = m_hardwarePlatform.Startup();
        if (GT::Failed(result))
        {
            //return result;
        }



        // File system.
        result = m_fileSystem.Startup();
        if (GT::Failed(result))
        {
            return result;
        }

        m_fileSystem.AddLowPriorityBaseDirectory(m_commandLine.GetApplicationDirectory());



        // Asset library.
        result = m_assetLibrary.Startup();
        if (GT::Failed(result))
        {
            return result;
        }

#if defined(GT_BUILD_DEFAULT_ASSETS)
        // Create and register the default allocator.
        m_pDefaultAssetAllocator = new DefaultAssetAllocator();
        m_assetLibrary.RegisterAllocator(*m_pDefaultAssetAllocator);

        // Register extensions.
        GT::AssetExtensionDesc extensions[] =
        {
    #if defined(GT_BUILD_PNG)
            {"png",  GT::AssetClass_Image, AssetType_Image_PNG},
    #endif
    #if defined(GT_BUILD_TGA)
            {"tga",  GT::AssetClass_Image, AssetType_Image_TGA},
    #endif
    #if defined(GT_BUILD_JPG)
            {"jpg",  GT::AssetClass_Image, AssetType_Image_JPG},
            {"jpeg", GT::AssetClass_Image, AssetType_Image_JPG},
    #endif
    #if defined(GT_BUILD_PSD)
            {"psd",  GT::AssetClass_Image, AssetType_Image_PSD},
    #endif

    #if defined(GT_BUILD_OBJ)
            {"obj",  GT::AssetClass_Model, AssetType_Model_OBJ},
    #endif
    #if defined(GT_BUILD_MD2)
            {"md2",  GT::AssetClass_Model, AssetType_Model_MD2},
    #endif
    #if defined(GT_BUILD_OGEX)
            {"ogex", GT::AssetClass_Model, AssetType_Model_OGEX},
    #endif

    #if defined(GT_BUILD_WAV)
            {"wav",  GT::AssetClass_Sound, AssetType_Sound_WAV},
    #endif
    #if defined(GT_BUILD_OGG)
            {"ogg",  GT::AssetClass_Sound, AssetType_Sound_OGG},
    #endif
        };
        m_assetLibrary.RegisterExtensions(extensions, sizeof(extensions) / sizeof(extensions[0]));
#endif


        return result;
    }

    void EngineContext::Shutdown()
    {
        // Asset library.
        m_assetLibrary.Shutdown();
        delete m_pDefaultAssetAllocator;


        // File system.
        m_fileSystem.Shutdown();


        // Hardware platform.
        m_hardwarePlatform.Shutdown();


        // Window manager.
        if (m_ownsWindowManager)
        {
            delete m_pWindowManager;
            m_pWindowManager = nullptr;
        }
    }



    void EngineContext::RegisterGraphicsInterfaceAllocator(GraphicsInterfaceAllocator &allocator)
    {
        m_graphicsInterfaceAllocators.RemoveFirstOccuranceOf(&allocator);
        m_graphicsInterfaceAllocators.InsertAt(&allocator, 0);
    }

    GraphicsInterface* EngineContext::CreateGraphicsInterface(GraphicsInterfaceType type)
    {
        for (size_t iAllocator = 0; iAllocator < m_graphicsInterfaceAllocators.GetCount(); ++iAllocator)
        {
            auto pAllocator = m_graphicsInterfaceAllocators[iAllocator];
            assert(pAllocator != nullptr);
            {
                if (pAllocator->IsGraphicsInterfaceSupported(type))
                {
                    return pAllocator->CreateGraphicsInterface(type);
                }
            }
        }

        return nullptr;
    }

    void EngineContext::DeleteGraphicsInterface(GraphicsInterface* pGraphicsInterface)
    {
        if (pGraphicsInterface != nullptr)
        {
            for (size_t iAllocator = 0; iAllocator < m_graphicsInterfaceAllocators.GetCount(); ++iAllocator)
            {
                auto pAllocator = m_graphicsInterfaceAllocators[iAllocator];
                assert(pAllocator != nullptr);
                {
                    if (pAllocator->IsGraphicsInterfaceSupported(pGraphicsInterface->GetType()))
                    {
                        pAllocator->DeleteGraphicsInterface(pGraphicsInterface);
                    }
                }
            }
        }
    }


    unsigned int EngineContext::GetGPURenderingDeviceCount() const
    {
        return m_hardwarePlatform.GetGPURenderingDeviceCount();
    }

    ResultCode EngineContext::GetGPURenderingDeviceInfo(unsigned int deviceIndex, GPURenderingDeviceInfo &deviceInfoOut) const
    {
        return m_hardwarePlatform.GetGPURenderingDeviceInfo(deviceIndex, deviceInfoOut);
    }



    GPURenderingDevice* EngineContext::CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount)
    {
        return m_hardwarePlatform.CreateGPURenderingDevice(deviceIndex, renderingAPIs, renderingAPIsCount);
    }

    GPURenderingDevice* EngineContext::CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPI)
    {
        switch (renderingAPI)
        {
        case RenderingAPI_Any:
            {
                RenderingAPI renderingAPIs[] = 
                {
                    RenderingAPI_D3D11,
                    RenderingAPI_OpenGL4
                };
                    
                return this->CreateGPURenderingDevice(deviceIndex, renderingAPIs, sizeof(renderingAPIs) / sizeof(renderingAPIs[0]));
            }

        default:
            {
                return this->CreateGPURenderingDevice(deviceIndex, &renderingAPI, 1);
            }
        }
    }

    GPURenderingDevice* EngineContext::CreateGPURenderingDevice(unsigned int deviceIndex)
    {
        return this->CreateGPURenderingDevice(deviceIndex, RenderingAPI_Any);
    }

    GPURenderingDevice* EngineContext::CreateGPURenderingDevice()
    {
        return this->CreateGPURenderingDevice(0);
    }

    void EngineContext::DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice)
    {
        m_hardwarePlatform.DeleteGPURenderingDevice(renderingDevice);
    }



    FileSystem & EngineContext::GetFileSystem()
    {
        return m_fileSystem;
    }


    ////////////////////////////////////////////////////////////////////
    // Window Management
    WindowManager & EngineContext::GetWindowManager()
    {
        assert(m_pWindowManager != nullptr);

        return *m_pWindowManager;
    }


    //////////////////////////////////////////////////////////
    // Asset Management

    AssetLibrary & EngineContext::GetAssetLibrary()
    {
        return m_assetLibrary;
    }

    Asset* EngineContext::LoadAsset(const char* filePathOrIdentifier, AssetType explicitAssetType)
    {
        return m_assetLibrary.Load(filePathOrIdentifier, explicitAssetType);
    }

    Asset* EngineContext::LoadAsset(Asset* pAsset)
    {
        return m_assetLibrary.Load(pAsset);
    }

    void EngineContext::UnloadAsset(Asset* pAsset)
    {
        m_assetLibrary.Unload(pAsset);
    }

    void EngineContext::ReloadAsset(const char* filePathOrIdentifier)
    {
        m_assetLibrary.Reload(filePathOrIdentifier);
    }

    void EngineContext::RegisterAssetAllocator(AssetAllocator &allocator)
    {
        m_assetLibrary.RegisterAllocator(allocator);
    }

    void EngineContext::RegisterAssetExtensions(AssetExtensionDesc* extensions, size_t extensionsCount)
    {
        m_assetLibrary.RegisterExtensions(extensions, extensionsCount);
    }


    //////////////////////////////////////////////////////////
    // Private
}
