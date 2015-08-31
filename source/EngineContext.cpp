// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>

#include <GTGameEngine/Graphics/GraphicsAPI_Null.hpp>
#if defined(GT_BUILD_VULKAN)
#include <GTGameEngine/Graphics/GraphicsAPI_Vulkan.hpp>
#endif
#if defined(GT_BUILD_D3D12)
#include <GTGameEngine/Graphics/GraphicsAPI_D3D12.hpp>
#endif
#if defined(GT_BUILD_OPENGL)
#include <GTGameEngine/Graphics/GraphicsAPI_OpenGL.hpp>
#endif

#include <GTLib/CommandLine.hpp>
#include <direct.h>

#include "Assets/DefaultAssetAllocator.hpp"
#include "Audio/OpenAL/AudioEngine_OpenAL.hpp"

namespace GT
{
    EngineContext::EngineContext()
        : m_commandLine(),
          m_fileSystem(),
          m_assetLibrary(m_fileSystem),
          m_componentLibrary(),
          m_audioSystem(nullptr), m_audioPlaybackDevice(0), m_audioListener(0), m_soundWorld(*this)
    {
    }

    EngineContext::~EngineContext()
    {
        this->Shutdown();
    }


    ResultCode EngineContext::Startup(int argc, char** argv)
    {
        ResultCode result = 0;


        // Command line.
        m_commandLine.Parse(argc, argv);

        // Make the global current directory that of the executable by default.
        #if defined(_MSC_VER)
            _chdir(m_commandLine.GetApplicationDirectory());
        #else
            chdir(m_commandLine.GetApplicationDirectory());
        #endif


        // Graphics APIs.
        m_pGraphicsAPI_Null = new GraphicsAPI_Null;
        if (!m_pGraphicsAPI_Null->Startup())        // <-- This should never fail for the null API.
        {
            delete m_pGraphicsAPI_Null;
            m_pGraphicsAPI_Null = nullptr;
        }
#if defined(GT_BUILD_VULKAN)
        m_pGraphicsAPI_Vulkan = new GraphicsAPI_Vulkan;
        if (!m_pGraphicsAPI_Vulkan->Startup())
        {
            delete m_pGraphicsAPI_Vulkan;
            m_pGraphicsAPI_Vulkan = nullptr;
        }
#endif
#if defined(GT_BUILD_D3D12)
        m_pGraphicsAPI_D3D12 = new GraphicsAPI_D3D12;
        if (!m_pGraphicsAPI_D3D12->Startup())
        {
            delete m_pGraphicsAPI_D3D12;
            m_pGraphicsAPI_D3D12 = nullptr;
        }
#endif
#if defined(GT_BUILD_OPENGL)
        m_pGraphicsAPI_OpenGL = new GraphicsAPI_OpenGL;
        if (!m_pGraphicsAPI_OpenGL->Startup())
        {
            delete m_pGraphicsAPI_OpenGL;
            m_pGraphicsAPI_OpenGL = nullptr;
        }
#endif



        // File system.
        result = m_fileSystem.Startup();
        if (GT::Failed(result))
        {
            return result;
        }

        m_fileSystem.AddBaseDirectory(m_commandLine.GetApplicationDirectory());



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
#endif



        // Components library
#if defined(GT_BUILD_COMPONENT_EVENTS)
        m_componentLibrary.RegisterDescriptor(m_eventsComponentDescriptor);
#endif
#if defined(GT_BUILD_COMPONENT_SCRIPT)
        //m_componentLibrary.RegisterDescriptor(m_scriptComponentDescriptor);
#endif
#if defined(GT_ENABLE_COMPONENT_GRAPHICS)
        m_componentLibrary.RegisterDescriptor(m_graphicsComponentDescriptor);
#endif
#if defined(GT_BUILD_COMPONENT_DYNAMICS)
        //m_componentLibrary.RegisterDescriptor(m_dynamicsComponentDescriptor);
#endif



        //////////////////////////////////////////
        // Audio System
        //
        // TODO: Add support for multiple audio systems.
        m_audioSystem = new AudioEngine_OpenAL();
        if (m_audioSystem->Startup())
        {

        }
        else
        {
            // TODO: Use the null audio system.
        }

        assert(m_audioSystem != nullptr);
        {
            // TEMP: Print the playback devices.
            size_t playbackDeviceCount = m_audioSystem->GetPlaybackDeviceCount();
            for (size_t iDevice = 0; iDevice < playbackDeviceCount; ++iDevice)
            {
                auto deviceInfo = m_audioSystem->GetPlaybackDeviceInfo(iDevice);

                GTLib::String message;
                message.AppendFormatted("Playback Device (%d) - %s", static_cast<int>(iDevice), deviceInfo.name.c_str());

                printf("%s\n", message.c_str());
            }

            if (playbackDeviceCount > 0)
            {
                m_audioPlaybackDevice = m_audioSystem->OpenPlaybackDevice(0);
                m_audioListener = m_audioSystem->CreateListener(m_audioPlaybackDevice);
            }
        }


        return result;
    }

    void EngineContext::Shutdown()
    {
        // Audio system.
        m_soundWorld.StopAllSounds();

        m_audioSystem->DeleteListener(m_audioListener);
        m_audioListener = 0;

        m_audioSystem->ClosePlaybackDevice(m_audioPlaybackDevice);
        m_audioPlaybackDevice = 0;

        delete m_audioSystem;
        m_audioSystem = nullptr;



        // Asset library.
        m_assetLibrary.Shutdown();

#if defined(GT_BUILD_DEFAULT_ASSETS)
        delete m_pDefaultAssetAllocator;
        m_pDefaultAssetAllocator = nullptr;
#endif



        // File system.
        m_fileSystem.Shutdown();


        // Graphics APIs.
        m_pGraphicsAPI_Null->Shutdown();
        delete m_pGraphicsAPI_Null;
        m_pGraphicsAPI_Null = nullptr;
#if defined(GT_BUILD_VULKAN)
        m_pGraphicsAPI_Vulkan->Shutdown();
        delete m_pGraphicsAPI_Vulkan;
        m_pGraphicsAPI_Vulkan = nullptr;
#endif
#if defined(GT_BUILD_D3D12)
        m_pGraphicsAPI_D3D12->Shutdown();
        delete m_pGraphicsAPI_D3D12;
        m_pGraphicsAPI_D3D12 = nullptr;
#endif
#if defined(GT_BUILD_OPENGL)
        m_pGraphicsAPI_OpenGL->Shutdown();
        delete m_pGraphicsAPI_OpenGL;
        m_pGraphicsAPI_OpenGL = nullptr;
#endif
    }


    const GTLib::CommandLine & EngineContext::GetCommandLine() const
    {
        return m_commandLine;
    }


    GraphicsAPI* EngineContext::GetBestGraphicsAPI()
    {
#if defined(GT_BUILD_VULKAN)
        if (m_pGraphicsAPI_Vulkan != nullptr)
        {
            return m_pGraphicsAPI_Vulkan;
        }
#endif
#if defined(GT_BUILD_D3D12)
        if (m_pGraphicsAPI_D3D12 != nullptr)
        {
            return m_pGraphicsAPI_D3D12;
        }
#endif
#if defined(GT_BUILD_OPENGL)
        if (m_pGraphicsAPI_OpenGL != nullptr)
        {
            return m_pGraphicsAPI_OpenGL;
        }
#endif

        // If we get here it means none of the other API's are supported.
        return m_pGraphicsAPI_Null;
    }

    GraphicsAPI_Null* EngineContext::GetNullGraphicsAPI()
    {
        return m_pGraphicsAPI_Null;
    }

#if defined(GT_BUILD_VULKAN)
    GraphicsAPI_Vulkan* EngineContext::GetVulkanGraphicsAPI()
    {
        return m_pGraphicsAPI_Vulkan;
    }
#endif
#if defined(GT_BUILD_D3D12)
    GraphicsAPI_D3D12* EngineContext::GetD3D12GraphicsAPI()
    {
        return m_pGraphicsAPI_D3D12;
    }
#endif
#if defined(GT_BUILD_OPENGL)
    GraphicsAPI_OpenGL* EngineContext::GetOpenGLGraphicsAPI()
    {
        return m_pGraphicsAPI_OpenGL;
    }
#endif


    FileSystem & EngineContext::GetFileSystem()
    {
        return m_fileSystem;
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

    

    ////////////////////////////////////////////////////////////////////
    // Scene Node Component Management

    void EngineContext::RegisterSceneNodeComponentDescriptor(const SceneNodeComponentDescriptor &descriptor)
    {
        m_componentLibrary.RegisterDescriptor(descriptor);
    }

    const SceneNodeComponentDescriptor* EngineContext::GetSceneNodeDescriptorByType(SceneNodeComponentTypeID type) const
    {
        return m_componentLibrary.GetDescriptorByType(type);
    }

    SceneNodeComponent* EngineContext::CreateSceneNodeComponent(SceneNodeComponentTypeID type)
    {
        return m_componentLibrary.CreateComponent(type);
    }

    void EngineContext::DeleteSceneNodeComponent(SceneNodeComponent* pComponent)
    {
        m_componentLibrary.DeleteComponent(pComponent);
    }



    ////////////////////////////////////////////////////
    // Audio

    void EngineContext::PlaySound(const char* fileName, bool relativePositioning)
    {
        m_soundWorld.PlaySound(fileName, vec3(0, 0, 0), relativePositioning);
    }

    void EngineContext::SetListenerPosition(float x, float y, float z)
    {
        m_audioSystem->SetListenerPosition(m_audioListener, x, y, z);
    }

    void EngineContext::SetListenerRotation(const quat &orientation)
    {
        m_audioSystem->SetListenerOrientation(m_audioListener, orientation);
    }
}
