// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_HardwarePlatform_GPU_hpp_
#define __GT_GE_HardwarePlatform_GPU_hpp_

#include <GTLib/Config.hpp>
#include <GTLib/ResultCodes.hpp>
#include <GTLib/Vector.hpp>

namespace GT
{
    namespace GE
    {
        class GPURenderingDevice;

        // Rendering API codes.
        static const int RenderingAPI_Any      = 0;
        static const int RenderingAPI_Any_Gen1 = 1;
        static const int RenderingAPI_Any_Gen2 = 2;
        static const int RenderingAPI_OpenGL21 = (1 << 8) | RenderingAPI_Any_Gen1;
        static const int RenderingAPI_OpenGL45 = (1 << 8) | RenderingAPI_Any_Gen2;
        static const int RenderingAPI_D3D9     = (2 << 8) | RenderingAPI_Any_Gen1;
        static const int RenderingAPI_D3D11    = (2 << 8) | RenderingAPI_Any_Gen2;

        struct GPURenderingDeviceInfo
        {
            /// The description of the device.
            char description[256];

            /// The supported APIs of the given device. This will be terminated with a 0.
            int supportedAPIs[16];


            /// The identifier that is used for identifying the device for the OpenGL API. With OpenGL, we can only use the primary display, so for
            /// this one, we set it to 1 for the rendering device and 0 for the others.
            int identifier_OpenGL;

            /// The identifier that is used for identifying the device for the D3D11 API. This is just the index 
            int identifier_D3D11;
        };


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
            ResultCode GetGPURenderingDeviceInfo(GPURenderingDeviceInfo &deviceInfoOut);


            /// Creates a rendering device for the given API code.
            ///
            /// @param apiCode            [in]  A code identifying the rendering API to use as the back-end.
            /// @param renderingDeviceOut [out] A reference to the variable that will receive a pointer to the new rendering device.
            ///
            /// @return A result code specifying whether or not the device was created successfully. The return value will be >=0 if there was no error.
            ///
            /// @remarks
            ///     Specify a value of 0 for apiCode to automatically choose the best API back-end. On Windows this will try the highest available version of Direct3D. On Linux
            ///     this will pick the highest available version of OpenGL.
            ResultCode CreateGPURenderingDevice(int apiCode, GPURenderingDevice* &renderingDeviceOut);

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

            /// Creates the best available GPU rendering device.
            ///
            /// @param renderingDeviceOut [in] A reference to the variable that will receive a pointer to the new rendering devivce.
            ///
            /// @return >=0 if there is no error.
            ResultCode CreateGPURenderingDevice_Any(GPURenderingDevice* &renderingDeviceOut);

            /// Creates the best available generation 1 GPU rendering device.
            ///
            /// @param renderingDeviceOut [in] A reference to the variable that will receive a pointer to the new rendering devivce.
            ///
            /// @return >=0 if there is no error.
            ResultCode CreateGPURenderingDevice_Gen1(GPURenderingDevice* &renderingDeviceOut);

            /// Creates the best available generation 2 GPU rendering device.
            ///
            /// @param renderingDeviceOut [in] A reference to the variable that will receive a pointer to the new rendering devivce.
            ///
            /// @return >=0 if there is no error.
            ResultCode CreateGPURenderingDevice_Gen2(GPURenderingDevice* &renderingDeviceOut);

            /// Creates the best available OpenGL 2.1 GPU rendering device.
            ///
            /// @param renderingDeviceOut [in] A reference to the variable that will receive a pointer to the new rendering devivce.
            ///
            /// @return >=0 if there is no error.
            ResultCode CreateGPURenderingDevice_OpenGL21(GPURenderingDevice* &renderingDeviceOut);

            /// Creates the best available OpenGL 4.5 GPU rendering device.
            ///
            /// @param renderingDeviceOut [in] A reference to the variable that will receive a pointer to the new rendering devivce.
            ///
            /// @return >=0 if there is no error.
            ResultCode CreateGPURenderingDevice_OpenGL45(GPURenderingDevice* &renderingDeviceOut);

            /// Creates the best available D3D9 GPU rendering device.
            ///
            /// @param renderingDeviceOut [in] A reference to the variable that will receive a pointer to the new rendering devivce.
            ///
            /// @return >=0 if there is no error.
            ResultCode CreateGPURenderingDevice_D3D9(GPURenderingDevice* &renderingDeviceOut);

            /// Creates the best available D3D11 GPU rendering device.
            ///
            /// @param renderingDeviceOut [in] A reference to the variable that will receive a pointer to the new rendering devivce.
            ///
            /// @return >=0 if there is no error.
            ResultCode CreateGPURenderingDevice_D3D11(GPURenderingDevice* &renderingDeviceOut);



        private:

            /// The list of info structures for each rendering device.
            GTLib::Vector<GPURenderingDeviceInfo> m_renderingDevices;


#if defined(GT_PLATFORM_WINDOWS)
            /// The IDXGIFactory object that was used to iterate over each of the GPU adapters.
            void* m_pDXGIFactory;

            /// 
#endif


        private:    // No copying.
            HardwarePlatform_GPU(const HardwarePlatform_GPU &);
            HardwarePlatform_GPU & operator=(const HardwarePlatform_GPU &);
        };
    }
}

#endif
