// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsInterface_MT_hpp_
#define __GT_GraphicsInterface_MT_hpp_

#include "GraphicsInterface.hpp"

namespace GT
{
	/// Base class for modern, multi-threaded rendering APIs.
	///
	/// This is suited for Vulkan/D3D12/Metal style APIs.
	class GraphicsInterface_MT : public GraphicsInterface
	{
	public:

		/// Constructor.
		GraphicsInterface_MT(GraphicsInterfaceType type);

		/// Destructor.
		virtual ~GraphicsInterface_MT();


		/// GraphicsInterface::GetClass()
		GraphicsInterfaceClass GetClass() const;



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
        virtual bool EnumDevices(unsigned int iDevice, GraphicsDeviceInfo &infoOut) const = 0;

        /// Creates a graphics device object using the given device index.
        ///
        /// @param iDevice [in] The index of the device.
        ///
        /// @return A handle to the object representing the graphics device.
        virtual HGraphicsDevice CreateDevice(unsigned int iDevice) = 0;

        /// Deletes the given graphics device.
        ///
        /// @parm hDevice [in] A handle to the device to delete.
        virtual void DeleteDevice(HGraphicsDevice hDevice) = 0;




        virtual ResultCode InitWindowBuffers(HGraphicsDevice hDevice, HCommandQueue hCommandQueue, HDescriptorHeap hDescriptorHeap, unsigned int descriptorIndex, HWND hWnd, bool includeDepthStencil) = 0;


        //////////////////////////////////////////
        // State

        /// Creates a pipeline state object.
        ///
        /// @param desc [in] A reference to the object that describes the pipeline state.
        virtual HPipelineState CreateGraphicsPipelineState(HGraphicsDevice hDevice, const GraphicsPipelineStateDesc &desc) = 0;

        /// Deletes the given pipeline state object.
        virtual void DeleteGraphicsPipelineState(HPipelineState hPipelineState) = 0;


        //////////////////////////////////////////
        // Allocators

        virtual HCommandAllocator CreateCommandAllocator(HGraphicsDevice hDevice, CommandListType type) = 0;
        virtual void DeleteCommandAllocator(HCommandAllocator hCommandAllocator) = 0;


        //////////////////////////////////////////
        // Command Queues and Lists

        virtual HCommandQueue CreateCommandQueue(HGraphicsDevice hDevice, CommandListType type, unsigned int priority) = 0;
        virtual void DeleteCommandQueue(HCommandQueue hCommandQueue) = 0;
        virtual void SignalCommandQueue(HCommandQueue hCommandQueue, HFence hFence, uint64_t value) = 0;
        virtual void Wait(HCommandQueue hCommandQueue, HFence hFence, uint64_t value) = 0;

        virtual HCommandList CreateCommandList(HGraphicsDevice hDevice, CommandListType type, HCommandAllocator hCommandAllocator, HPipelineState hInitialGraphicsPipelineState) = 0;
        virtual void DeleteCommandList(HCommandList hCommandList) = 0;


        //////////////////////////////////////////
        // Heaps

        virtual HDescriptorHeap CreateDescriptorHeap(HGraphicsDevice hDevice, HeapType type, unsigned int descriptorCount, bool shaderVisible) = 0;
        virtual void DeleteDescriptorHeap(HDescriptorHeap hHeap) = 0;


        //////////////////////////////////////////
        // Synchronization

        virtual HFence CreateFence(HGraphicsDevice hDevice, uint64_t initialValue, FenceFlag flags) = 0;
        virtual void DeleteFence(HFence hFence) = 0;
        virtual void SignalFence(HFence hFence, uint64_t value) = 0;
        virtual void SetEventOnCompletion(HFence hFence, uint64_t value, HANDLE hEvent) = 0;
        virtual uint64_t GetCompletedValue(HFence hFence) = 0;
        


        //////////////////////////////////////////
        // Rendering

        virtual void ResetCommandAllocator(HCommandAllocator hCommandAllocator) = 0;
        virtual void ResetCommandList(HCommandList hCommandList, HCommandAllocator hCommandAllocator, HPipelineState hPipelineState) = 0;
        virtual void Close(HCommandList hCommandList) = 0;
        virtual void SetViewport(HCommandList hCommandList, const GraphicsViewport &viewport) = 0;

        // TEMP
        virtual void ClearWindow(HCommandList hCommandList, HWND hWnd, float* color) = 0;
        virtual void ResourceBarrier(HCommandList hCommandList, HWND hWnd, GraphicsResourceUsage stateBefore, GraphicsResourceUsage stateAfter) = 0;

        virtual void ExecuteCommandList(HCommandQueue hCommandQueue, HCommandList hCommandList) = 0;
	};
}

#endif

