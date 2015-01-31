// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_EngineContext_hpp_
#define __GT_GE_EngineContext_hpp_

#include <GTLib/ResultCodes.hpp>
#include "HardwarePlatform.hpp"

namespace GT
{
    namespace GE
    {
        class GPURenderingDevice;


        /// Class representing the engine context.
        ///
        /// Only a single engine context is usually needed for an application, however it is possible to have more.
        class EngineContext
        {
        public:

            /// Constructor.
            EngineContext();

            /// Destructor.
            ~EngineContext();


            /// Starts up the engine context.
            ///
            /// @param argc [in] The number of arguments passed to the command line (same as the argc parameter of main())
            /// @param argv [in] The array of strings making up the command line (same as the argv parameter of main())
            ///
            /// @return A result code specifying whether or not the context was initialized successfully. If the return value is >= 0, it was successful, otherwise it failed.
            ///
            /// @remarks
            ///     The argc and argv parameters should be passed exactly as they were passed from main().
            ResultCode Startup(int argc, char** argv);

            /// Shuts down the engine engine.
            ///
            /// @remarks
            ///     This does not delete the context object.
            void Shutdown();


            /// Creates a GPU rendering device.
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

            /// Deletes a GPU rendering device object that was created with DeleteGPURenderingDevice.
            ///
            /// @param renderingDevice [in] A pointer to the GPU device to delete.
            void DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice);



        private:




        private:

            /// The hardware platform object for detecting the available hardware on the running system. This object can be used to iterate over devices
            /// and specify which ones should be used for particular tasks.
            HardwarePlatform m_hardwarePlatform;



        private:    // No copying.
            EngineContext(const EngineContext &);
            EngineContext & operator=(const EngineContext &);
        };
    }
}


#endif
