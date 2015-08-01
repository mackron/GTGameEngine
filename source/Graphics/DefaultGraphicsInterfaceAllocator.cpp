// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_DEFAULT_GRAPHICS_INTERFACES)
#include "DefaultGraphicsInterfaceAllocator.hpp"
#include <GTGameEngine/Graphics/GraphicsInterface.hpp>

#if defined(GT_BUILD_D3D12)
#include "D3D12/D3D12GraphicsInterface.hpp"
#endif

namespace GT
{
    DefaultGraphicsInterfaceAllocator::DefaultGraphicsInterfaceAllocator()
        : GraphicsInterfaceAllocator()
#if defined(GT_BUILD_VULKAN)
        , m_isVulkanInitialized(false),
          m_isVulkanSupported(false)
#endif
#if defined(GT_BUILD_D3D12)
        , m_isD3D12Initialized(false),
          m_isD3D12Supported(false)
    #if defined(GT_PLATFORM_WINDOWS)
        , m_hDXGI(NULL),
          m_hD3D12(NULL),
          m_hD3DCompiler(NULL)
    #endif
#endif
#if defined(GT_BUILD_OPENGL)
        , m_isOpenGLInitialized(false),
          m_isOpenGLSupported(false)
#endif
    {
    }

    DefaultGraphicsInterfaceAllocator::~DefaultGraphicsInterfaceAllocator()
    {
#if defined(GT_BUILD_VULKAN)
        this->ShutdownVulkan();
#endif

#if defined(GT_BUILD_D3D12)
        this->ShutdownD3D12();
#endif

#if defined(GT_BUILD_OPENGL)
        this->ShutdownOpenGL();
#endif
    }


    bool DefaultGraphicsInterfaceAllocator::IsGraphicsInterfaceSupported(GraphicsInterfaceType type) const
    {
        // You will see here that the return value is just Startup*(). If an API has already been successfully initialized, these
        // will immediately return true - thus, it is safe to call these repeatedly.

        switch (type)
        {
#if defined(GT_BUILD_VULKAN)
        case GraphicsInterfaceType_Vulkan:
            {
                return const_cast<DefaultGraphicsInterfaceAllocator*>(this)->IsVulkanSupported();
            }
#endif

#if defined(GT_BUILD_D3D12)
        case GraphicsInterfaceType_D3D12:
            {
                return const_cast<DefaultGraphicsInterfaceAllocator*>(this)->IsD3D12Supported();
            }
#endif

#if defined(GT_BUILD_OPENGL)
        case GraphicsInterfaceType_OpenGL:
            {
                return const_cast<DefaultGraphicsInterfaceAllocator*>(this)->IsOpenGLSupported();
            }
#endif

        default: return false;
        }
    }

    GraphicsInterface* DefaultGraphicsInterfaceAllocator::CreateGraphicsInterface(GraphicsInterfaceType type)
    {
        switch (type)
        {
#if defined(GT_BUILD_VULKAN)
        case GraphicsInterfaceType_Vulkan:
            {
                return this->CreateVulkanInterface();
            }
#endif

#if defined(GT_BUILD_D3D12)
        case GraphicsInterfaceType_D3D12:
            {
                return this->CreateD3D12Interface();
            }
#endif

#if defined(GT_BUILD_OPENGL)
        case GraphicsInterfaceType_OpenGL:
            {
                return this->CreateOpenGLInterface();
            }
#endif

        default: return nullptr;
        }
    }

    void DefaultGraphicsInterfaceAllocator::DeleteGraphicsInterface(GraphicsInterface* pGraphicsInterface)
    {
        delete pGraphicsInterface;
    }




    ////////////////////////////////////
    // Private
#if defined(GT_BUILD_VULKAN)
        bool DefaultGraphicsInterfaceAllocator::IsVulkanSupported()
        {
            if (!m_isVulkanInitialized)
            {
                this->StartupVulkan();
            }

            return m_isVulkanSupported;
        }

        GraphicsInterface* DefaultGraphicsInterfaceAllocator::CreateVulkanInterface()
        {
            if (!m_isVulkanInitialized)
            {
                this->StartupVulkan();
            }

            if (m_isVulkanSupported)
            {
                // Not yet implemented.
                return nullptr;
            }
            else
            {
                // Vulkan is not supported.
                return nullptr;
            }
        }

        bool DefaultGraphicsInterfaceAllocator::StartupVulkan()
        {
            if (!m_isVulkanInitialized)
            {
                m_isVulkanInitialized = true;

                // Not yet implemented.
                m_isVulkanSupported = false;
            }

            return m_isVulkanSupported;
        }

        void DefaultGraphicsInterfaceAllocator::ShutdownVulkan()
        {
            m_isVulkanInitialized = false;
            m_isVulkanSupported   = false;
        }
#endif

#if defined(GT_BUILD_D3D12)
        bool DefaultGraphicsInterfaceAllocator::IsD3D12Supported()
        {
            if (!m_isD3D12Initialized)
            {
                this->StartupD3D12();
            }

            return m_isD3D12Supported;
        }

        GraphicsInterface* DefaultGraphicsInterfaceAllocator::CreateD3D12Interface()
        {
            if (!m_isD3D12Initialized)
            {
                this->StartupD3D12();
            }

            if (m_isD3D12Supported)
            {
                return new D3D12GraphicsInterface(m_hDXGI, m_hD3D12, m_hD3DCompiler);
            }
            else
            {
                // D3D12 is not supported.
                return nullptr;
            }
        }

        bool DefaultGraphicsInterfaceAllocator::StartupD3D12()
        {
            if (!m_isD3D12Initialized)
            {
                m_isD3D12Initialized = true;

                m_hDXGI = LoadLibraryW(L"dxgi.dll");
                if (m_hDXGI != 0)
                {
                    m_hD3D12 = LoadLibraryW(L"d3d12.dll");
                    if (m_hD3D12 != 0)
                    {
                        const wchar_t* compilerDLLs[] = {
                            L"d3dcompiler_47.dll",
                            L"d3dcompiler_46.dll",
                            L"d3dcompiler_45.dll",
                            L"d3dcompiler_44.dll",
                            L"d3dcompiler_43.dll",
                            L"d3dcompiler_42.dll",
                            L"d3dcompiler_41.dll",
                            L"d3dcompiler_40.dll",
                            L"d3dcompiler_39.dll",
                            L"d3dcompiler_38.dll",
                            L"d3dcompiler_37.dll",
                            L"d3dcompiler_36.dll",
                            L"d3dcompiler_35.dll",
                            L"d3dcompiler_34.dll",
                            L"d3dcompiler_33.dll",
                        };

                        int compilerDLLCount = sizeof(compilerDLLs) / sizeof(compilerDLLs[0]);
                        for (int i = 0; i < compilerDLLCount; ++i)
                        {
                            m_hD3DCompiler = LoadLibraryW(compilerDLLs[i]);
                            if (m_hD3DCompiler != 0)
                            {
                                break;
                            }
                        }
                    }
                }

                // We will first try the DXGI API for finding the device information.
                if (m_hDXGI != 0 && m_hD3D12 != 0 && m_hD3DCompiler != 0)
                {
                    PFN_CreateDXGIFactory2 _CreateDXGIFactory2 = reinterpret_cast<PFN_CreateDXGIFactory2>(GetProcAddress(m_hDXGI, "CreateDXGIFactory2"));
                    if (_CreateDXGIFactory2 != nullptr)
                    {
                        IDXGIFactory4* pDXGIFactory;
                        HRESULT hr = _CreateDXGIFactory2(0, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(&pDXGIFactory));
                        if (SUCCEEDED(hr))
                        {
                            // One of the adapters must support D3D12.

                            PFN_D3D12_CREATE_DEVICE _D3D12CreateDevice = reinterpret_cast<PFN_D3D12_CREATE_DEVICE>(GetProcAddress(m_hD3D12, "D3D12CreateDevice"));
                            if (_D3D12CreateDevice != nullptr)
                            {
                                UINT iAdapter = 0;
                                IDXGIAdapter1* pAdapter;
                                while (SUCCEEDED(pDXGIFactory->EnumAdapters1(iAdapter, &pAdapter)))
                                {
                                    ID3D12Device* pD3DDevice = nullptr;
                                    hr = _D3D12CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, D3D12_CREATE_DEVICE_NONE, D3D_FEATURE_LEVEL_11_1, D3D12_SDK_VERSION, __uuidof(ID3D12Device), reinterpret_cast<void**>(&pD3DDevice));
                                    if (SUCCEEDED(hr))
                                    {
                                        // D3D12 is supported.
                                        pD3DDevice->Release();

                                        m_isD3D12Supported = true;
                                        break;
                                    }

                                    iAdapter += 1;
                                }
                            }

                            pDXGIFactory->Release();
                        }
                    }
                }
            }

            return m_isD3D12Supported;
        }

        void DefaultGraphicsInterfaceAllocator::ShutdownD3D12()
        {
            CloseHandle(m_hD3DCompiler);
            CloseHandle(m_hD3D12);
            CloseHandle(m_hDXGI);

            m_isD3D12Initialized = false;
            m_isD3D12Supported   = false;
        }
#endif

#if defined(GT_BUILD_OPENGL)
        bool DefaultGraphicsInterfaceAllocator::IsOpenGLSupported()
        {
            if (!m_isOpenGLInitialized)
            {
                this->StartupOpenGL();
            }

            return m_isOpenGLSupported;
        }

        GraphicsInterface* DefaultGraphicsInterfaceAllocator::CreateOpenGLInterface()
        {
            if (!m_isOpenGLInitialized)
            {
                this->StartupOpenGL();
            }

            if (m_isOpenGLSupported)
            {
                // Not yet implemented.
                return nullptr;
            }
            else
            {
                // OpenGL is not supported.
                return nullptr;
            }
        }

        bool DefaultGraphicsInterfaceAllocator::StartupOpenGL()
        {
            if (!m_isOpenGLInitialized)
            {
                m_isOpenGLInitialized = true;

                // Not yet implemented.
                m_isOpenGLSupported = false;
            }

            return m_isOpenGLSupported;
        }

        void DefaultGraphicsInterfaceAllocator::ShutdownOpenGL()
        {
            m_isOpenGLInitialized = false;
            m_isOpenGLSupported   = false;
        }
#endif
}

#endif
