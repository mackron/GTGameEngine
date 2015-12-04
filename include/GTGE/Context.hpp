// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Context
#define GT_Context

#include "MessageDispatcher.hpp"
#include "Audio/SoundWorld.hpp"
#include "Assets/AssetLibrary.hpp"
#include <GTGE/Core/Vector.hpp>
#include <easy_util/easy_util.h>
#include <easy_fs/easy_vfs.h>

#undef GetCommandLine

namespace GT
{
    /// Class representing the engine context.
    class Context
    {
    public:

        /// Constructor.
        Context(int argc, char** argv);

        /// Destructor.
        ~Context();



        ////////////////////////////////////////////////////
        // Command Line

        /// Retrieve a reference to the internal command line object.
        ///
        /// @return A pointer to the internal command line object.
        easyutil_cmdline & GetCommandLine() { return m_cmdline; }



        ////////////////////////////////////////////////////
        // File System Management

        /// Retrieves a pointer to the virtual file system object.
        easyvfs_context* GetVFS() { return m_pVFS; }

        /// Retrieves the executable's absolute file path.
        const char* GetExecutableAbsolutePath() const;

        /// Retrieves the absolute path of the executable's directory.
        ///
        /// @remarks
        ///     This does not include the executable's directory
        const char* GetExecutableDirectoryAbsolutePath() const;

        /// Adds a base directory relative to the executable.
        void AddBaseDirectoryRelativeToExe(const char* relativePath);



        ////////////////////////////////////////////////////
        // Messages

        /// Posts a simple log message.
        void Log(const char* message);

        /// Posts a generic, formatted message.
        void Logf(const char* format, ...);

        /// Posts a message.
        void LogMessage(const char* message);
        void LogMessagef(const char* format, ...);

        /// Posts a warning message.
        void LogWarning(const char* message);
        void LogWarningf(const char* format, ...);

        /// Posts an error message.
        void LogError(const char* message);
        void LogErrorf(const char* format, ...);




        ////////////////////////////////////////////////////
        // Audio

        /// Retrieves a pointer to the easy_audio context.
        easyaudio_context* GetAudioContext();

        /// Retrieves a pointer to the audio playback device.
        easyaudio_device* GetAudioPlaybackDevice();

        /// Retrieves a reference to the global sound world.
        ///
        /// @return A reference to the global sound world for when a sound needs to be played globally, outside of a scene.
        GT::SoundWorld & GetSoundWorld();



        ////////////////////////////////////////////////////
        // Assets

        /// Returns a reference to the internal asset library.
        AssetLibrary & GetAssetLibrary();



    private:

        /// The command line object.
        easyutil_cmdline m_cmdline;

        /// The absolute path of the executable.
        char m_executableAbsolutePath[EASYVFS_MAX_PATH];

        /// The absolute path of the directory of the application's executable.
        char m_executableDirectoryAbsolutePath[EASYVFS_MAX_PATH];


        /// A pointer to the object representing the virtual file system. This is where base directories are added.
        easyvfs_context* m_pVFS;

        /// The log file.
        easyvfs_file* m_pLogFile;


        /// A pointer to the easy_audio context for audio playback.
        easyaudio_context* m_pAudioContext;

        /// A pointer to the device for audio playback.
        easyaudio_device* m_pAudioPlaybackDevice;

        /// The global sound world.
        GT::SoundWorld m_soundWorld;


        /// The asset library.
        AssetLibrary m_assetLibrary;




        




    private:    // No copying.
        Context(const Context &);
        Context & operator=(const Context &);
    };
}

#endif
