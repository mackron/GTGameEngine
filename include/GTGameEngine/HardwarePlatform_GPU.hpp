// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_HardwarePlatform_GPU_hpp_
#define __GT_GE_HardwarePlatform_GPU_hpp_

#include "Config.hpp"
#include <GTLib/ResultCodes.hpp>
#include <GTLib/Vector.hpp>
#include <GTGameEngine/GPURenderingDeviceInfo.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

namespace GT
{
    namespace GE
    {
        class GPURenderingDevice;

        /// Class representing all of the usable GPU devices on the running system.
        class HardwarePlatform_GPU
        {
        public:

            /// Constructor.
            HardwarePlatform_GPU();

            /// Destructor.
            ~HardwarePlatform_GPU();


            /// Starts up the hardware platform object.
            ///
            /// @return A result code that determines whether or not initialization was successful, and if not, what the error was.
            ResultCode Startup();

            /// Shuts down the platform object.
            void Shutdown();


            /// Retrieves the number of GPU rendering devices available to the application.
            ///
            /// @return The number of available GPU rendering devices.
            ///
            /// @remarks
            ///     This will return 0 if there was any kind of error.
            unsigned int GetGPURenderingDeviceCount() const;

            /// Retrieves information about the GPU rendering device at the given index.
            ///
            /// @param deviceIndex   [in]  The index of the rendering device.
            /// @param deviceInfoOut [out] A reference to the GPURenderingDeviceInfo object that will receive information about the device.
            ResultCode GetGPURenderingDeviceInfo(unsigned int deviceIndex, GPURenderingDeviceInfo &deviceInfoOut) const;


            /// Creates an instance of a rendering device from the given device index.
            ///
            /// @param deviceIndex        [in] The index of the device to create the device instance from.
            /// @param renderingAPIs      [in] A buffer containing a list of rendering APIs to use, in order of preference.
            /// @param renderingAPIsCount [in] The number of items in the renderingAPIs argument.
            /// @param deviceOut          [in] A reference to the variable that will receive a pointer to the new device instance.
            ///
            /// @return <0 if an error occurs, otherwise >=0.
            ///
            /// @remarks
            ///     To use the primary rendering device, specify 0 for deviceIndex.
            ///     @par
            ///     renderingAPIs should contain a list of rendering APIs to use, in order of preference. If none of the APIs are supported, an error will be returned.
            ResultCode CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount, GPURenderingDevice* &deviceOut);

            /// Deletes a GPU rendering device object that was created with CreateGPURenderingDevice().
            ///
            /// @param renderingDevice [in] A pointer to the GPU rendering device to delete.
            void DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice);


            /// Creates a GPU compute device.
            ///
            /// @param computeDevice [out] A reference to the variable that will receive a pointer to the new compute device.
            ///
            /// @return A result code specifying whether or not the device was created successfully. The return value will be >=0 if there was no error.
            ResultCode CreateGPUComputeDevice(GPURenderingDevice* &computeDeviceOut);

            /// Deletes a GPU compute device object that was created with CreateGPUComputeDevice().
            ///
            /// @param computeDevice [in] A pointer to the GPU compute device to delete.
            void DeleteGPUComputeDevice(GPURenderingDevice* computeDevice);



        private:

            /// The list of info structures for each rendering device.
            GTLib::Vector<GPURenderingDeviceInfo> m_renderingDevices;


#if defined(GT_PLATFORM_WINDOWS)
            /// A handle representing the DXGI DLL.
            HMODULE m_hDXGI;

            /// A handle representing the D3D11 DLL.
            HMODULE m_hD3D11;

            /// A handle representing the D3DCompiler DLL.
            HMODULE m_hD3DCompiler;

            /// A handle representing the OpenGL32 DLL.
            HMODULE m_hOpenGL32;


            /// The dummy window for creating the OpenGL rendering context.
            HWND m_hOpenGLDummyWindow;

            /// The device context of the OpenGL dummy window.
            HDC m_hOpenGLDummyDC;

            /// The pixel format that must used for each OpenGL window.
            int m_OpenGLPixelFormat;

            /// The primary OpenGL rendering context. All other rendering contexts are created from this.
            HGLRC m_hOpenGLRC;
#endif


        private:    // No copying.
            HardwarePlatform_GPU(const HardwarePlatform_GPU &);
            HardwarePlatform_GPU & operator=(const HardwarePlatform_GPU &);
        };
    }
}

#endif
