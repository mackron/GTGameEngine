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


#if defined(GT_GE_BUILD_OPENGL4)
#include "Rendering/OpenGL/OpenGL.hpp"
#endif


namespace GT
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
        ///
        /// @return A pointer to the new rendering device, or null if an error occurs.
        ///
        /// @remarks
        ///     To use the primary rendering device, specify 0 for deviceIndex.
        ///     @par
        ///     renderingAPIs should contain a list of rendering APIs to use, in order of preference. If none of the APIs are supported, an error will be returned.
        GPURenderingDevice* CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount);

        /// Deletes a GPU rendering device object that was created with CreateGPURenderingDevice().
        ///
        /// @param renderingDevice [in] A pointer to the GPU rendering device to delete.
        void DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice);



    private:

        /// The list of info structures for each rendering device.
        GTLib::Vector<GPURenderingDeviceInfo> m_renderingDevices;


#if defined(GT_GE_BUILD_D3D11)
        /// A handle representing the DXGI DLL.
        HMODULE m_hDXGI;

        /// A handle representing the D3D11 DLL.
        HMODULE m_hD3D11;

        /// A handle representing the D3DCompiler DLL.
        HMODULE m_hD3DCompiler;
#endif

#if defined(GT_GE_BUILD_OPENGL4)
        /// The OpenGL helper context.
        OpenGL4Context m_gl;
#endif


    public:

        //////////////////////////////////////////////////////
        // Error Codes

        static const ResultCode InvalidDeviceIndex       = (1 << 31) | 0x01;
        static const ResultCode RenderingAPINotSupported = (1 << 31) | 0x02;
        static const ResultCode UnknownRenderingAPI      = (1 << 31) | 0x03;



    private:    // No copying.
        HardwarePlatform_GPU(const HardwarePlatform_GPU &);
        HardwarePlatform_GPU & operator=(const HardwarePlatform_GPU &);
    };
}

#endif
