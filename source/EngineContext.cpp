// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/EngineContext.hpp>
#include <easy_path/easy_path.h>
#include <easy_fs/easy_vfs.h>

namespace GT
{
    namespace Engine
    {
        static const size_t MaxDormantThreads = 64;     ///< The maximum number of threads to allow to lie around dormant.


        typedef struct
        {
            /// The absolute path of the executable's directory.
            char absoluteExeDirPath[EASYVFS_MAX_PATH];

            /// The relative executable path.
            char absoluteExePath[EASYVFS_MAX_PATH];

            /// The relative path of the log file.
            char relativeLogPath[EASYVFS_MAX_PATH];

        } CommandLineData;


        static bool parse_cmdline_proc(const char* key, const char* value, void* pUserData)
        {
            CommandLineData* pData = reinterpret_cast<CommandLineData*>(pUserData);
            assert(pData != NULL);

            if (strcmp(key, "[path]") == 0)
            {
                char exeDirectoryPath[EASYVFS_MAX_PATH];
                easypath_copybasepath(value, exeDirectoryPath, sizeof(exeDirectoryPath));

                _chdir(exeDirectoryPath);
                _getcwd(pData->absoluteExeDirPath, sizeof(pData->absoluteExeDirPath));

                easypath_copyandappend(pData->absoluteExePath, sizeof(pData->absoluteExePath), pData->absoluteExeDirPath, easypath_filename(value));

                return true;
            }

            if (strcmp(key, "logfile") == 0)
            {
                strcpy_s(pData->relativeLogPath, sizeof(pData->relativeLogPath), value);
                return true;
            }

            return true;
        }


        EngineContext::EngineContext(int argc, char** argv)
            : m_cmdline(),
              m_executableDirectoryAbsolutePath(),
              m_pVFS(nullptr),
              m_applicationConfig(),
              m_messageHandler(), m_messageDispatcher(),
              m_pAudioContext(nullptr), m_pAudioPlaybackDevice(nullptr), m_soundWorld(*this),
              m_activeThreads(), m_dormantThreads(), m_threadManagementLock(),
              m_assetLibrary()
        {
            // We need to initialize the virtual file system early so we can do things like create logs and cache files.
            m_pVFS = easyvfs_create_context();


            // Parse the command line.
            CommandLineData cmdlineData;
            strcpy_s(cmdlineData.relativeLogPath, sizeof(cmdlineData.relativeLogPath), "var/logs/engine.html");

            if (easyutil_init_cmdline(&m_cmdline, argc, argv))
            {
                easyutil_parse_cmdline(&m_cmdline, parse_cmdline_proc, &cmdlineData);
            }

            // Make sure the executable's absolute path is clean for future things.
            easypath_clean(cmdlineData.absoluteExeDirPath, m_executableDirectoryAbsolutePath, sizeof(m_executableDirectoryAbsolutePath));
            easypath_clean(cmdlineData.absoluteExePath, m_executableAbsolutePath, sizeof(m_executableDirectoryAbsolutePath));


            // The directory containing the executable needs to be the lowest-priority base path.
            easyvfs_add_base_directory(m_pVFS, m_executableDirectoryAbsolutePath);
            


            // We will need to open the log file as soon as possible, but it needs to be done after ensuring the current directory is set to that of the executable.
            char logpath[EASYVFS_MAX_PATH];
            easypath_copyandappend(logpath, sizeof(logpath), m_executableDirectoryAbsolutePath, cmdlineData.relativeLogPath);
            
            m_messageHandler.OpenLogFile(m_pVFS, logpath);


            // At this point the message handler should be setup, so we'll go ahead and add it to the dispatcher.
            m_messageDispatcher.AddMessageHandler(m_messageHandler);


            // After moving into the application directory, we need to load up the config file and move into the data directory. From
            // there we can read the user configs and setup the log file.
            //
            // This is different from a user configuration (which are located in the 'configs' folder). The application configuration
            // usually remains constant. It defines things like directories. We won't return false if we fail to open, in which case
            // the game will use defaults.
            if (m_applicationConfig.Open(m_pVFS, "config.lua"))
            {
                // The application config will define the data directories where all of the game's data and assets are located. We will
                // move into the directory of the first defined data directory.
                auto &directories = m_applicationConfig.GetDataDirectories();
                if (directories.count > 0)
                {
                    // Here is where we add each of the base directories. Each directory is inserted in the second-to-last position
                    // because we always want the executable's directory to be the lowest priority one.
                    for (size_t i = 0; i < directories.count; ++i)
                    {
                        //GTLib::IO::AddAdditionalSearchPath(directories[i].c_str());
                        easyvfs_insert_base_directory(m_pVFS, directories[i].c_str(), easyvfs_get_base_directory_count(m_pVFS) - 1);
                    }
                }
            }



            //////////////////////////////////////////
            // Audio System
            
            m_pAudioContext = easyaudio_create_context();
            if (m_pAudioContext == NULL) {
                this->PostWarningMessage("Failed to create audio system.");
            }

            // TEMP: Print the playback devices.
            unsigned int playbackDeviceCount = easyaudio_get_output_device_count(m_pAudioContext);
            for (unsigned int iDevice = 0; iDevice < playbackDeviceCount; ++iDevice)
            {
                easyaudio_device_info info;
                if (easyaudio_get_output_device_info(m_pAudioContext, iDevice, &info))
                {
                    GTLib::String message;
                    message.AppendFormatted("Playback Device (%d) - %s", iDevice, info.description);

                    this->PostLogMessage(message.c_str());
                }
            }

            if (playbackDeviceCount > 0) {
                m_pAudioPlaybackDevice = easyaudio_create_output_device(m_pAudioContext, 0);
            }
        }

        EngineContext::~EngineContext()
        {
            // All sounds need to be stopped so we can clean up the threads.
            m_soundWorld.StopAllSounds();


            //////////////////////////////////////////
            // Threading
            //
            // All threads need to be unacquired, and we need to wait for them all to finish executing.

            this->UnacquireAllThreads();

            GTLib::Vector<GTLib::Thread*> threadsToDelete;
            m_threadManagementLock.Lock();
            {
                threadsToDelete = m_dormantThreads;
            }
            m_threadManagementLock.Unlock();

            for (size_t iThread = 0; iThread < threadsToDelete.GetCount(); ++iThread)
            {
                auto thread = threadsToDelete[iThread];
                assert(thread != nullptr);
                {
                    thread->Stop();
                    thread->Wait();

                    delete thread;
                }
            }



            //////////////////////////////////////////
            // Audio System

            easyaudio_delete_output_device(m_pAudioPlaybackDevice);
            m_pAudioPlaybackDevice = nullptr;

            easyaudio_delete_context(m_pAudioContext);
            m_pAudioContext = nullptr;
        }



        ////////////////////////////////////////////////////
        // Command Line

        const ApplicationConfig & EngineContext::GetApplicationConfig() const
        {
            return m_applicationConfig;
        }



        ////////////////////////////////////////////////////
        // File System Management

        const char* EngineContext::GetExecutableAbsolutePath() const
        {
            return m_executableAbsolutePath;
        }

        const char* EngineContext::GetExecutableDirectoryAbsolutePath() const
        {
            return m_executableDirectoryAbsolutePath;
        }



        ////////////////////////////////////////////////////
        // Threading

        GTLib::Thread* EngineContext::AcquireThread()
        {
            GTLib::Thread* thread = nullptr;

            m_threadManagementLock.Lock();
            {
                // First try getting a dormant thread. Technically, there's a small chance a dormant thread is still running, in which case we don't
                // want to consider those for the sake of efficiency.
                if (m_dormantThreads.GetCount() > 0)
                {
                    // Loop from the back.
                    size_t iThread = m_dormantThreads.GetCount();
                    do
                    {
                        --iThread;

                        thread = m_dormantThreads[iThread];
                        if (!thread->Busy())
                        {
                            m_dormantThreads.Remove(iThread);
                        }
                    } while (iThread > 0);
                }

                // If we haven't got a thread at this point it means there was no available dormant thread and thus we need to create a new one.
                if (thread == nullptr)
                {
                    thread = new GTLib::Thread();
                }


                // The thread needs to be added to the active threads list.
                assert(thread != nullptr);
                {
                    m_activeThreads.PushBack(thread);
                }
            }
            m_threadManagementLock.Unlock();


            return thread;
        }

        void EngineContext::UnacquireThread(GTLib::Thread* thread)
        {
            if (thread != nullptr)
            {
                m_threadManagementLock.Lock();
                {
                    this->UnacquireThreadNoLock(thread);
                }
                m_threadManagementLock.Unlock();
            }
        }

        void EngineContext::UnacquireThreadNoLock(GTLib::Thread* thread)
        {
            assert(thread != nullptr);

            m_activeThreads.RemoveFirstOccuranceOf(thread);

            if (m_dormantThreads.GetCount() < MaxDormantThreads)
            {
                m_dormantThreads.PushBack(thread);
            }
            else
            {
                delete thread;
            }
        }

        void EngineContext::UnacquireAllThreads()
        {
            m_threadManagementLock.Lock();
            {
                while (m_activeThreads.GetCount() > 0)
                {
                    this->UnacquireThreadNoLock(m_activeThreads.GetBack());
                }
            }
            m_threadManagementLock.Unlock();
        }



        ////////////////////////////////////////////////////
        // Messages

        void EngineContext::PostMessage(const Message &message)
        {
            m_messageDispatcher.PostMessage(message);
        }

        void EngineContext::PostMessage(MessageType type, const char* message)
        {
            m_messageDispatcher.PostMessage(type, message);
        }

        void EngineContext::PostErrorMessage(const char* message)
        {
            m_messageDispatcher.PostErrorMessage(message);
        }

        void EngineContext::PostWarningMessage(const char* message)
        {
            m_messageDispatcher.PostWarningMessage(message);
        }

        void EngineContext::PostLogMessage(const char* message)
        {
            m_messageDispatcher.PostLogMessage(message);
        }



        ////////////////////////////////////////////////////
        // Audio

        easyaudio_context* EngineContext::GetAudioContext()
        {
            return m_pAudioContext;
        }
        
        easyaudio_device* EngineContext::GetAudioPlaybackDevice()
        {
            return m_pAudioPlaybackDevice;
        }

        SoundWorld & EngineContext::GetSoundWorld()
        {
            return m_soundWorld;
        }



        ////////////////////////////////////////////////////
        // Assets

        AssetLibrary & EngineContext::GetAssetLibrary()
        {
            return m_assetLibrary;
        }
    }
}
