// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_D3D12GraphicsInterface_hpp_
#define __GT_D3D12GraphicsInterface_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_D3D12)
#include <GTGameEngine/Graphics/GraphicsInterface_MT.hpp>
#include "D3D12Common.hpp"

#include <GTLib/Map.hpp>

namespace GT
{
    /// D3D12 implementation of the graphics interface.
    class D3D12GraphicsInterface : public GraphicsInterface_MT
    {
    public:

        /// Constructor.
        D3D12GraphicsInterface(HMODULE hDXGI, HMODULE hD3D12, HMODULE hD3DCompiler);

        /// Destructor.
        virtual ~D3D12GraphicsInterface();


        /// @copydoc GraphicsInterface::Startup()
        ResultCode Startup();

        /// @copydoc GraphicsInterface::Shutdown()
        void Shutdown();


        /// Enumerates over each of the supported devices.
        ///
        /// @param iDevice [in]  The index of the device whose info is being retrieved.
        /// @param infoOut [out] A reference to the structure that will receive information about the device.
        ///
        /// @return True if a valid device was enumerated; false otherwise.
        ///
        /// @remarks
        ///     This only iterates over devices that are compatible with the device interface.
        ///     @par
        ///     To iterate over the devices, simply increment iDevice (beginning at 0) until false is returned.
        ///     @par
        ///     <code>
        ///     unsigned int iDevice = 0;
        ///     while (pGraphicsInterface->EnumDevices(iDevice, deviceInfo))
        ///     {
        ///         doSomething(deviceInfo);
        ///     }
        ///     </code>
        bool EnumDevices(unsigned int iDevice, GraphicsDeviceInfo &infoOut) const;

        /// Creates a graphics device object using the given device index.
        ///
        /// @param iDevice [in] The index of the device.
        ///
        /// @return A handle to the object representing the graphics device.
        HGraphicsDevice CreateDevice(unsigned int iDevice);

        /// Deletes the given graphics device.
        ///
        /// @parm hDevice [in] A handle to the device to delete.
        void DeleteDevice(HGraphicsDevice hDevice);



        //////////////////////////////////////////
        // Platform-Specific Methods

        /// @copydoc GraphicsInterface::SwapWindowBuffers()
        void SwapWindowBuffers(HWND hWnd, int swapInterval);

        /// @copydoc GraphicsInterface::InitWindowBuffers()
        ResultCode InitWindowBuffers(HWND hWnd, bool includeDepthStencil);
        ResultCode InitWindowBuffers(HGraphicsDevice hDevice, HCommandQueue hCommandQueue, HDescriptorHeap hDescriptorHeap, unsigned int descriptorIndex, HWND hWnd, bool includeDepthStencil);

        /// @copydoc GraphicsInterface::UninitWindowBuffers()
        void UninitWindowBuffers(HWND hWnd);

        /// @copydoc GraphicsInterface::ResizeWindowBuffers()
        void ResizeWindowBuffers(HWND hWnd);


        //////////////////////////////////////////
        // State

        /// Creates a pipeline state object.
        ///
        /// @param desc [in] A reference to the object that describes the pipeline state.
        HPipelineState CreateGraphicsPipelineState(HGraphicsDevice hDevice, const GraphicsPipelineStateDesc &desc);

        /// Deletes the given pipeline state object.
        void DeleteGraphicsPipelineState(HPipelineState hPipelineState);


        //////////////////////////////////////////
        // Allocators

        HCommandAllocator CreateCommandAllocator(HGraphicsDevice hDevice, CommandListType type);
        void DeleteCommandAllocator(HCommandAllocator hCommandAllocator);


        //////////////////////////////////////////
        // Command Queues and Lists

        HCommandQueue CreateCommandQueue(HGraphicsDevice hDevice, CommandListType type, unsigned int priority);
        void DeleteCommandQueue(HCommandQueue hCommandQueue);

        HCommandList CreateCommandList(HGraphicsDevice hDevice, CommandListType type, HCommandAllocator hCommandAllocator, HPipelineState hInitialGraphicsPipelineState);
        void DeleteCommandList(HCommandList hCommandList);


        //////////////////////////////////////////
        // Heaps

        HDescriptorHeap CreateDescriptorHeap(HGraphicsDevice hDevice, HeapType type, unsigned int descriptorCount, bool shaderVisible);
        void DeleteDescriptorHeap(HDescriptorHeap hHeap);


        //////////////////////////////////////////
        // Synchronization

        HFence CreateFence(HGraphicsDevice hDevice, uint64_t initialValue, FenceFlag flags);
        void DeleteFence(HFence hFence);
        void SignalFence(HFence hFence, uint64_t value);
        void SetEventOnCompletion(HFence hFence, uint64_t value, HANDLE hEvent);
        uint64_t GetCompletedValue(HFence hFence);

        void SignalCommandQueue(HCommandQueue hCommandQueue, HFence hFence, uint64_t value);
        void Wait(HCommandQueue hCommandQueue, HFence hFence, uint64_t value);


        //////////////////////////////////////////
        // Rendering

        void ResetCommandAllocator(HCommandAllocator hCommandAllocator);
        void ResetCommandList(HCommandList hCommandList, HCommandAllocator hCommandAllocator, HPipelineState hPipelineState);
        void Close(HCommandList hCommandList);
        void SetViewport(HCommandList hCommandList, const GraphicsViewport &viewport);

        void ClearWindow(HCommandList hCommandList, HWND hWnd, float* color);
        void ResourceBarrier(HCommandList hCommandList, HWND hWnd, GraphicsResourceUsage stateBefore, GraphicsResourceUsage stateAfter);

        void ExecuteCommandList(HCommandQueue hCommandQueue, HCommandList hCommandList);


    private:

        /// Structure containing information about a window's buffers.
        struct WindowBufferData
        {
            /// A pointer to the device that owns the window's buffers.
            ID3D12Device* pDeviceD3D;

            /// A pointer to the swap chain.
            IDXGISwapChain3* pSwapChain;

            /// The render target resources for the two buffers (front and back).
            ID3D12Resource* pRenderTargets[2];

            /// The descriptor handles for the two buffers (front and back).
            D3D12_CPU_DESCRIPTOR_HANDLE hDescriptorHandles[2];
        };


        /// A handle representing the dxgi DLL.
        HMODULE m_hDXGI;

        /// A handle representing the d3d12 DLL.
        HMODULE m_hD3D12;

        /// A handle representing the D3DCompiler DLL.
        HMODULE m_hD3DCompiler;


        /// A pointer to the DXGI factory for iterating over devices.
        IDXGIFactory4* m_pFactory;


        /// A map containing the data for each window buffer.
        GTLib::Map<HWND, WindowBufferData> m_windowBufferData;


        ///////////////////////////////
        // Function Pointers

        PFN_CreateDXGIFactory2 m_CreateDXGIFactory2;
        PFN_D3D12_CREATE_DEVICE m_D3D12CreateDevice;


        //////////////////////////////////////////////////////
        // GUIDs for the D3D12 API

        static const GUID CustomDataGUID_Generic;


    private:    // No copying.
        D3D12GraphicsInterface(const D3D12GraphicsInterface &);
        D3D12GraphicsInterface & operator=(const D3D12GraphicsInterface &);
    };
}

#endif

#endif
