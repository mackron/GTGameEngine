// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_hpp_
#define __GT_GE_GPURenderingDevice_hpp_

#include "Config.hpp"
#include "GPURenderingDeviceInfo.hpp"
#include "Rendering/GPUBufferType.hpp"
#include "Rendering/GPUBufferUsage.hpp"
#include "Rendering/GPUBufferCPUAccessFlags.hpp"
#include "Rendering/GPUBufferMapType.hpp"
#include "Rendering/PrimitiveTopolgoy.hpp"

#include <GTLib/ResultCodes.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

namespace GT
{
    namespace GE
    {
        class Framebuffer;
        class GPUBuffer;


        /// Class representing a rendering GPU device.
        class GPURenderingDevice
        {
        public:

            /// Constructor.
            GPURenderingDevice(const GPURenderingDeviceInfo &info);

            /// Destructor.
            virtual ~GPURenderingDevice();


            /// Retrieves the device information.
            ///
            /// @param infoOut [out] A reference to the object that will receive the info.
            void GetInfo(GPURenderingDeviceInfo &infoOut) const;



            /// Initializes the rendering device.
            ///
            /// @return <0 if an error occurs, >=0 if successful.
            virtual ResultCode Startup() = 0;

            /// Uninitializes the rendering device.
            virtual void Shutdown() = 0;


            /// Retrieves the generation number for this renderer.
            ///
            /// @return An integer representing the generation of rendering technologies this rendering device supports. See remarks.
            ///
            /// @remarks
            ///     This is used in determining whether or not this device can be cased to GPURenderingDevice_Gen1 or GPURenderingDevice_Gen2. 
            ///     @par
            ///     If this returns 1, the rendering device supports Direct3D 9 / OpenGL 2.1 class of functionality.
            ///     @par
            ///     If this returns 2, the rendering device supports Direct3D 11 / OpenGL 4.5 class of functionality.
            virtual unsigned int GetGeneration() const = 0;

            /// Retrieves a code representing the rendering API being used internally by the device.
            ///
            /// @return A code representing the rendering API being used internally by the device.
            virtual RenderingAPI GetRenderingAPI() const = 0;



            /// Sets the swap interval.
            ///
            /// @param swapInterval [in] The new swap interval. See remarks.
            ///
            /// @remarks
            ///     A value of 0 will disable v-sync. A value >0 will enable v-sync.
            virtual void SetSwapInterval(int swapInterval) = 0;


            ////////////////////////////////////////////
            // Drawing

            /// Clears the current framebuffer with the given color.
            ///
            /// @param r [in] The red component.
            /// @param g [in] The green component.
            /// @param b [in] The blue component.
            /// @param a [in] The alpha component.
            virtual void ClearColor(float r, float g, float b, float a) = 0;

            /// Renders the contents of the currently bound vertex buffer using the currently bound index buffer.
            ///
            /// @param indexCount         [in] The number of indices to draw.
            /// @param startIndexLocation [in] The zero-based location of the first index in the index buffer to start drawing from.
            ///
            /// @remarks
            ///     The \c indexCount argument refers to how many indices to iterate over when determining which primitives to draw. For example, when drawing
            ///     a triangle, you would pass the number 3 (number of indices used to build the triangle).
            virtual void Draw(unsigned int indexCount, unsigned int startIndexLocation) = 0;




            /////////////////////////////////////////////
            // State

            /// Sets the primitive topology to use for all future draw calls.
            ///
            /// @param topology [in] The new primitive topology to use for drawing.
            ///
            /// @remarks
            ///     The default topology is triangles.
            virtual void SetPrimitiveTopology(PrimitiveTopology topology) = 0;

            /// Sets the current vertex buffer.
            ///
            /// @param buffer [in] A pointer to the buffer to make current on the vertex buffer bind point.
            virtual void SetCurrentVertexBuffer(GPUBuffer* buffer) = 0;

            /// Sets the current index buffer.
            ///
            /// @param buffer [in] A pointer to the buffer to make current on the index buffer bind point.
            virtual void SetCurrentIndexBuffer(GPUBuffer* buffer) = 0;

            /// Sets the current constant buffer.
            ///
            /// @param buffer [in] A pointer to the buffer to make current on the constant buffer bind point.
            /// @param slot   [in] The zero-based slot number to bind the constant buffer to.
            ///
            /// @remarks
            ///     Every constant/uniform block has an index. When assigning a buffer to a block, you pass the index to the \c slot parameter.
            virtual void SetCurrentConstantBuffer(GPUBuffer* buffer, unsigned int slot) = 0;




            ////////////////////////////////////////////
            // Buffers

            /// Creates a buffer object of the given type.
            ///
            /// @param type           [in]  The buffer type (vertex, index, constant, etc.)
            /// @param usage          [in]  The usage type of the buffer (immutable, dynamic, etc.)
            /// @param cpuAccessFlags [in] Flags specifying how the CPU is allowed to access the buffer's data. This affects map/unmap operations.
            /// @param sizeInBytes    [in]  The size in bytes of the buffer.
            /// @param data           [in]  A pointer to the buffer that contains the initial data.
            /// @param bufferOut      [out] A reference to the variable that will receive a pointer to the new buffer object.
            ///
            /// @return The result code specifying whether or not the buffer was created successfully.
            ///
            /// @remarks
            ///     When \c usage is GPUBufferUsage_Immutable, \c data cannot be null.
            ///     @par
            ///     If \c cpuAccessFlags is set to GPUBufferCPUAccess_None (0), all map and unmap operations will fail.
            virtual ResultCode CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data, GPUBuffer* &bufferOut) = 0;

            /// Deletes a buffer object that was created with CreateBuffer().
            ///
            /// @param buffer [in] A pointer to the buffer to delete.
            virtual void DeleteBuffer(GPUBuffer* buffer) = 0;

            /// Maps the given buffer's data for use on the CPU side.
            ///
            /// @param buffer  [in]  A pointer to the buffer object whose data is being mapped.
            /// @param mapType [in]  The type of mapping to perform. See remarks.
            /// @param dataOut [out] A reference to the variable that will receive a pointer to the mapped data.
            ///
            /// @remarks
            ///     This does not, conceptually, allocate a buffer - it simply outputs a pointer to the internal buffer.
            ///     @par
            ///     The buffer must be unmapped with UnmapBuffer().
            ///     @par
            ///     This cannot be used when the buffer was created with GPUBufferUsage_Immutable.
            ///     @par
            ///     When \c mapType is \c GPUBufferMapType_Read or \c GPUBufferMapType_ReadWrite, the buffer must have been created with \c GPUBufferCPUAccess_Read.
            ///     @par
            ///     When \c mapType is \c GPUBufferMapType_Write, \c GPUBufferMapType_ReadWrite, \c GPUBufferMapType_Write_Discard or \c GPUBufferMapType_Write_NoOverwrite, the buffer must have been created with \c GPUBufferCPUAccess_Write.
            virtual ResultCode MapBuffer(GPUBuffer* buffer, GPUBufferMapType mapType, void* &dataOut) = 0;

            /// Unmaps the given buffer's data.
            ///
            /// @param buffer [in] A pointer to the buffer object whose data is being unmapped.
            ///
            /// @remarks
            ///     This should be paired with a prior call to MapBuffer(). After this is called, the pointer returned by MapBuffer will become invalid.
            virtual void UnmapBuffer(GPUBuffer* buffer) = 0;

            /// Sets the data for the given buffer.
            ///
            /// @param buffer        [in] A pointer to the buffer object whose data is being updated.
            /// @param offsetInBytes [in] The offset in bytes to start the data placement.
            /// @param sizeInBytes   [in] The size in bytes of the new data.
            /// @param data          [in] A pointer to the buffer containing the new data.
            ///
            /// @remarks
            ///     This will fail if the buffer was created with GPUBufferUsage_Immutable.
            ///     @par
            ///     This will fail if the buffer is currently mapped.
            ///     @par
            ///     When using the Direct3D 11 API, this will use map/unmap internally when the buffer was created with GPUBufferUsage_Dynamic.
            virtual ResultCode SetBufferData(GPUBuffer* buffer, size_t offsetInBytes, size_t sizeInBytes, const void* data) = 0;



            ///////////////////////////////////////////
            // Framebuffers

            /// Creates a framebuffer object.
            ///
            /// @param framebufferOut [in] A reference to the variable that will receive a pointer to the new framebuffer, if successful.
            virtual ResultCode CreateFramebuffer(Framebuffer* &framebuffer) = 0;



            //////////////////////////////////////////
            // Platform-Specific Methods

            /// Swaps the buffers for the current window.
            virtual void SwapBuffers() = 0;

#if defined(GT_PLATFORM_WINDOWS)
            /// Win32: Creates a framebuffer for the given window so that it can be drawn to.
            ///
            /// @param hWnd                [in] A handle to the window whose framebuffer is being initialized.
            /// @param includeDepthStencil [in] Whether or not a depth/stencil buffer should also be created.
            virtual ResultCode InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil) = 0;

            /// Win32: Uninitialises the framebuffer of the given window.
            ///
            /// @param hWnd [in] A handle to the window whose framebuffer is being unitialized.
            virtual void UninitWindowFramebuffer(HWND hWnd) = 0;

            /// Win32: Sets the current window where rendering will be output to.
            ///
            /// @param hWnd [in] A handle to the window to make current.
            ///
            /// @return <0 if there was an error making the window current. >=0 if the window was set successfully.
            ///
            /// @remarks
            ///     Removing the current binding by pass NULL.
            ///     @par
            ///     InvalidWindowRenderTarget will be returned if the window was not first initialized with InitWindowFramebuffer().
            virtual ResultCode SetCurrentWindow(HWND hWnd) = 0;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif



        protected:

            /// The device info structure.
            GPURenderingDeviceInfo m_info;



        public:

            //////////////////////////////////////////////////////
            // Error Codes

            static const ResultCode RenderingAPINotSupported          = (1 << 31) | 0x00000001;
            static const ResultCode InvalidWindowRenderTarget         = (1 << 31) | 0x000000F0;      //< Fired when a window is attempted to be made current, but the window was never initialized with a framebuffer.
            static const ResultCode UnknownGPUBufferType              = (1 << 31) | 0x000000F1;
            static const ResultCode UnknownGPUBufferUsage             = (1 << 31) | 0x000000F2;
            static const ResultCode UnsupportedGPUBufferType          = (1 << 31) | 0x000000F3;
            static const ResultCode UnsupportedGPUBufferUsage         = (1 << 31) | 0x000000F4;
            static const ResultCode NoDataSpecifiedForImmutableBuffer = (1 << 31) | 0x000000F5;
            static const ResultCode GPUBufferIsImmutable              = (1 << 31) | 0x000000F6;
            static const ResultCode FailedToMapGPUBuffer              = (1 << 31) | 0x000000F7;
            static const ResultCode UnknownGPUBufferMapType           = (1 << 31) | 0x000000F8;


        private:    // No copying.
            GPURenderingDevice(const GPURenderingDevice &);
            GPURenderingDevice & operator=(const GPURenderingDevice &);
        };
    }
}

#endif