// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_EngineContext_hpp_
#define __GT_GE_EngineContext_hpp_

#include <GTLib/ResultCodes.hpp>
#include <GTLib/CommandLine.hpp>
#include "HardwarePlatform.hpp"
#include "FileSystem.hpp"
#include "Assets/AssetLibrary.hpp"

namespace GT
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



        /// Retrieves the number of GPU rendering devices available to the application.
        ///
        /// @return The number of GPU devices that are available to the application for rendering.
        unsigned int GetGPURenderingDeviceCount() const;

        /// Retrieves the GPU rendering device info for the device at the given index.
        ///
        /// @param deviceIndex   [in]  The index of the device whose information is being retrieved.
        /// @param deviceInfoOut [out] A reference to the object that will receive the device info.
        ///
        /// @return If an error occurs, the return value will be <0. Otherwise, the return value will be >=0.
        ResultCode GetGPURenderingDeviceInfo(unsigned int deviceIndex, GPURenderingDeviceInfo &deviceInfoOut) const;


        /// @copydoc GT::GE::HardwarePlatform_GPU::CreateGPURenderingDevice()
        GPURenderingDevice* CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount);

        /// Helper for creating a GPU rendering device for the given rendering API.
        ///
        /// @param deviceIndex  [in]  The index of the device to create the new device instance from.
        /// @param renderingAPI [in]  The rendering API to use.
        ///
        /// @return A pointer to the new device, or null if an error occured.
        GPURenderingDevice* CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPI);

        /// Helper for creating a GPU rendering device with the best available rendering API.
        ///
        /// @param deviceIndex  [in]  The index of the device to create the new device instance from.
        /// @param deviceOut    [out] A reference to the variable that will receive the new device object.
        ///
        /// @return A pointer to the new device, or null if an error occured.
        GPURenderingDevice* CreateGPURenderingDevice(unsigned int deviceIndex);

        /// The highest level helper for creating a GPU rendering device.
        ///
        /// @return A pointer to the GPU rendering device, or null if an error occurs.
        ///
        /// @remarks
        ///     This will create the rendering device from the primary adapter (device index of 0) and will use the best available rendering API.
        GPURenderingDevice* CreateGPURenderingDevice();


        /// Deletes a GPU rendering device object that was created with DeleteGPURenderingDevice.
        ///
        /// @param renderingDevice [in] A pointer to the GPU device to delete.
        void DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice);



        /// Retrieves a reference to the file system.
        ///
        /// @return A reference to the file system.
        FileSystem & GetFileSystem();

        /// Retrieves a reference to the asset library.
        ///
        /// @return A reference to the asset library.
        AssetLibrary & GetAssetLibrary();


    private:





    private:

        /// An object representing the command line that was used to initialise the game engine.
        GTLib::CommandLine m_commandLine;

        /// The hardware platform object for detecting the available hardware on the running system. This object can be used to iterate over devices
        /// and specify which ones should be used for particular tasks.
        HardwarePlatform m_hardwarePlatform;

        /// The virtual file system for handling file reading and writing.
        FileSystem m_fileSystem;

        /// The asset library for managing assets.
        AssetLibrary m_assetLibrary;


    private:    // No copying.
        EngineContext(const EngineContext &);
        EngineContext & operator=(const EngineContext &);
    };
}


#endif
