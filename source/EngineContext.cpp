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
#include <GTLib/IO.hpp>

#include "Assets/DefaultAssetAllocator.hpp"

namespace GT
{
    EngineContext::EngineContext()
        : m_commandLine(),
          m_fileSystem(),
          m_assetLibrary(m_fileSystem)
    {
    }

    EngineContext::~EngineContext()
    {
    }


    ResultCode EngineContext::Startup(int argc, char** argv)
    {
        ResultCode result = 0;


        // Command line.
        m_commandLine.Parse(argc, argv);

        // Make the global current directory that of the executable by default.
        GTLib::IO::SetCurrentDirectory(m_commandLine.GetApplicationDirectory());


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


        return result;
    }

    void EngineContext::Shutdown()
    {
        // Asset library.
        m_assetLibrary.Shutdown();

#if defined(GT_BUILD_DEFAULT_ASSETS)
        delete m_pDefaultAssetAllocator;
        m_pDefaultAssetAllocator = nullptr;
#endif



        // File system.
        m_fileSystem.Shutdown();


        // Graphics APIs.
        delete m_pGraphicsAPI_Null;
        m_pGraphicsAPI_Null = nullptr;
#if defined(GT_BUILD_VULKAN)
        delete m_pGraphicsAPI_Vulkan;
        m_pGraphicsAPI_Vulkan = nullptr;
#endif
#if defined(GT_BUILD_D3D12)
        delete m_pGraphicsAPI_D3D12;
        m_pGraphicsAPI_D3D12 = nullptr;
#endif
#if defined(GT_BUILD_OPENGL)
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

    //////////////////////////////////////////////////////////
    // Private
}
