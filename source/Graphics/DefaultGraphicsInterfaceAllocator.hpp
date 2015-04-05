// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_DefaultGraphicsInterfaceAllocator_hpp_
#define __GT_DefaultGraphicsInterfaceAllocator_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_DEFAULT_GRAPHICS_INTERFACES)
#include <GTGameEngine/Graphics/GraphicsInterfaceAllocator.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif


namespace GT
{
    /// The default graphics interface allocator.
    class DefaultGraphicsInterfaceAllocator : public GraphicsInterfaceAllocator
    {
    public:

        /// Constructor.
        DefaultGraphicsInterfaceAllocator();

        /// Destructor.
        virtual ~DefaultGraphicsInterfaceAllocator();


        /// @copydoc GraphicsInterfaceAllocator::IsGraphicsInterfaceSupported()
        bool IsGraphicsInterfaceSupported(GraphicsInterfaceType type) const;

        /// @copydoc GraphicsInterfaceAllocator::CreateGraphicsInterface()
        GraphicsInterface* CreateGraphicsInterface(GraphicsInterfaceType type);

        /// @copydoc GraphicsInterfaceAllocator::DeleteGraphicsInterface()
        void DeleteGraphicsInterface(GraphicsInterface* pGraphicsInterface);



    private:

#if defined(GT_BUILD_VULKAN)
        /// Determines whether or not the Vulkan API is supported.
        ///
        /// @return True if the Vulkan API is supported; false otherwise.
        ///
        /// @remarks
        ///     This will initialize Vulkan if it has not been done already.
        bool IsVulkanSupported();

        /// Creates an instantiation of the Vulkan graphics interface.
        ///
        /// @return A pointer to the new interface, or null if Vulkan is not supported.
        ///
        /// @remarks
        ///     This will initialize Vulkan if it has not been done already.
        GraphicsInterface* CreateVulkanInterface();

        /// Starts up the Vulkan API.
        ///
        /// @return True if the Vulkan API is supported; false otherwise.
        bool StartupVulkan();

        /// Shuts down the Vulkan API.
        void ShutdownVulkan();
#endif

#if defined(GT_BUILD_D3D12)
        /// Determines whether or not the D3D12 API is supported.
        ///
        /// @return True if the D3D12 API is supported; false otherwise.
        ///
        /// @remarks
        ///     This will initialize D3D12 if it has not been done already.
        bool IsD3D12Supported();

        /// Creates an instantiation of the D3D12 graphics interface.
        ///
        /// @return A pointer to the new interface, or null if D3D12 is not supported.
        ///
        /// @remarks
        ///     This will initialize D3D12 if it has not been done already.
        GraphicsInterface* CreateD3D12Interface();

        /// Starts up the D3D12 API.
        ///
        /// @return True if the D3D12 API is supported; false otherwise.
        bool StartupD3D12();

        /// Shuts down the D3D12 API.
        void ShutdownD3D12();
#endif

#if defined(GT_BUILD_OPENGL)
        /// Determines whether or not the OpenGL API is supported.
        ///
        /// @return True if the OpenGL API is supported; false otherwise.
        ///
        /// @remarks
        ///     This will initialize OpenGL if it has not been done already.
        bool IsOpenGLSupported();

        /// Creates an instantiation of the OpenGL graphics interface.
        ///
        /// @return A pointer to the new interface, or null if D3D12 is not supported.
        ///
        /// @remarks
        ///     This will initialize OpenGL if it has not been done already.
        GraphicsInterface* CreateOpenGLInterface();

        /// Starts up the OpenGL API.
        ///
        /// @return True if the OpenGL API is supported; false otherwise.
        bool StartupOpenGL();

        /// Shuts down the OpenGL API.
        void ShutdownOpenGL();
#endif



    private:

#if defined(GT_BUILD_VULKAN)
        /// Keeps track of whether or not Vulkan has been initialized.
        bool m_isVulkanInitialized;

        /// Keeps track of whether or not Vulkan is supported.
        bool m_isVulkanSupported;


    #if defined(GT_PLATFORM_WINDOWS)
    #endif

    #if defined(GT_PLATFORM_LINUX)
    #endif
#endif

#if defined(GT_BUILD_D3D12)
        /// Keeps track of whether or not D3D12 has been initialized.
        bool m_isD3D12Initialized;

        /// Keeps track of whether or not D3D12 is supported.
        bool m_isD3D12Supported;


    #if defined(GT_PLATFORM_WINDOWS)
        /// A handle representing the dxgi DLL.
        HMODULE m_hDXGI;

        /// A handle representing the d3d12 DLL.
        HMODULE m_hD3D12;

        /// A handle representing the D3DCompiler DLL.
        HMODULE m_hD3DCompiler;
    #endif
#endif

#if defined(GT_BUILD_OPENGL)
        /// Keeps track of whether or not OpenGL has been initialized.
        bool m_isOpenGLInitialized;

        /// Keeps track of whether or not OpenGL is supported.
        bool m_isOpenGLSupported;


    #if defined(GT_PLATFORM_WINDOWS)
    #endif

    #if defined(GT_PLATFORM_LINUX)
    #endif
#endif


    private:    // No copying.
        DefaultGraphicsInterfaceAllocator(const DefaultGraphicsInterfaceAllocator &);
        DefaultGraphicsInterfaceAllocator & operator=(const DefaultGraphicsInterfaceAllocator &);
    };
}

#endif

#endif
