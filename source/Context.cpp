// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Context.hpp>
#include <easy_path/easy_path.h>
#include <easy_fs/easy_vfs.h>

namespace GT
{
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
            easypath_copy_base_path(value, exeDirectoryPath, sizeof(exeDirectoryPath));

            _chdir(exeDirectoryPath);
            _getcwd(pData->absoluteExeDirPath, sizeof(pData->absoluteExeDirPath));

            easypath_copy_and_append(pData->absoluteExePath, sizeof(pData->absoluteExePath), pData->absoluteExeDirPath, easypath_file_name(value));

            return true;
        }

        if (strcmp(key, "logfile") == 0)
        {
            strcpy_s(pData->relativeLogPath, sizeof(pData->relativeLogPath), value);
            return true;
        }

        return true;
    }


    typedef struct
    {
        /// A pointer to the main context.
        Context* pContext;

        /// A pointer to the file to read the config data from.
        easyvfs_file* pFile;

    } AppConfigData;

    static unsigned int app_config_read(void* pUserData, void* pDataOut, unsigned int bytesToRead)
    {
        AppConfigData* pData = reinterpret_cast<AppConfigData*>(pUserData);
        assert(pData != NULL);

        unsigned int bytesRead;
        if (easyvfs_read(pData->pFile, pDataOut, bytesToRead, &bytesRead)) {
            return bytesRead;
        }

        return 0;
    }

    static void app_config_pair(void* pUserData, const char* key, const char* value)
    {
        AppConfigData* pData = reinterpret_cast<AppConfigData*>(pUserData);
        assert(pData != NULL);

        if (strcmp(key, "BaseDirectory") == 0)
        {
            pData->pContext->AddBaseDirectoryRelativeToExe(value);
            return;
        }
    }

    static void app_config_error(void* pUserData, const char* message, unsigned int line)
    {
        AppConfigData* pData = reinterpret_cast<AppConfigData*>(pUserData);
        assert(pData != NULL);

        pData->pContext->LogErrorf("config.cfg:%d %s", line, message);
    }




    Context::Context(int argc, char** argv)
        : m_cmdline(),
          m_executableDirectoryAbsolutePath(),
          m_pVFS(nullptr),
          m_pLogFile(nullptr),
          m_pAudioContext(nullptr), m_pAudioPlaybackDevice(nullptr), m_soundWorld(*this),
          m_assetLibrary()
    {
        // We need to initialize the virtual file system early so we can do things like create logs and cache files.
        m_pVFS = easyvfs_create_context();


        // Parse the command line.
        CommandLineData cmdlineData;
        strcpy_s(cmdlineData.relativeLogPath, sizeof(cmdlineData.relativeLogPath), "var/logs/engine.txt");

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
        easypath_copy_and_append(logpath, sizeof(logpath), m_executableDirectoryAbsolutePath, cmdlineData.relativeLogPath);

        m_pLogFile = easyvfs_open(m_pVFS, logpath, EASYVFS_WRITE, 0);
        if (m_pLogFile == NULL) {
            printf("WARNING: Failed to open log file.\n");
        }




        // After moving into the application directory, we need to load up the config file and move into the data directory. From
        // there we can read the user configs and setup the log file.
        //
        // This is different from a user configuration (which are located in the 'configs' folder). The application configuration
        // usually remains constant. It defines things like directories. We won't return false if we fail to open, in which case
        // the game will use defaults.
        AppConfigData cfg;
        cfg.pContext = this;
        cfg.pFile    = easyvfs_open(m_pVFS, "config.cfg", EASYVFS_READ, 0);
        if (cfg.pFile != NULL) {
            easyutil_parse_key_value_pairs(app_config_read, app_config_pair, app_config_error, &cfg);
            easyvfs_close(cfg.pFile);
        }
        



        //// Asset Library ////
        if (!m_assetLibrary.Startup(m_pVFS))
        {
            this->LogError("Failed to initialize asset library.");
        }




        //////////////////////////////////////////
        // Audio System
            
        m_pAudioContext = easyaudio_create_context();
        if (m_pAudioContext == NULL) {
            this->LogError("Failed to create audio system.");
        }

        // TEMP: Print the playback devices.
        unsigned int playbackDeviceCount = easyaudio_get_output_device_count(m_pAudioContext);
        for (unsigned int iDevice = 0; iDevice < playbackDeviceCount; ++iDevice)
        {
            easyaudio_device_info info;
            if (easyaudio_get_output_device_info(m_pAudioContext, iDevice, &info))
            {
                this->Logf("Playback Device (%d) - %s", iDevice, info.description);
            }
        }

        if (playbackDeviceCount > 0) {
            m_pAudioPlaybackDevice = easyaudio_create_output_device(m_pAudioContext, 0);
        }

        m_soundWorld.Startup();
    }

    Context::~Context()
    {
        //////////////////////////////////////////
        // Audio System

        m_soundWorld.Shutdown();

        easyaudio_delete_output_device(m_pAudioPlaybackDevice);
        m_pAudioPlaybackDevice = nullptr;

        easyaudio_delete_context(m_pAudioContext);
        m_pAudioContext = nullptr;
    }



    ////////////////////////////////////////////////////
    // File System Management

    const char* Context::GetExecutableAbsolutePath() const
    {
        return m_executableAbsolutePath;
    }

    const char* Context::GetExecutableDirectoryAbsolutePath() const
    {
        return m_executableDirectoryAbsolutePath;
    }

    void Context::AddBaseDirectoryRelativeToExe(const char* relativePath)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        easypath_to_absolute(relativePath, this->GetExecutableDirectoryAbsolutePath(), absolutePath, sizeof(absolutePath));

        easyvfs_insert_base_directory(m_pVFS, absolutePath, easyvfs_get_base_directory_count(m_pVFS) - 1);
    }



    ////////////////////////////////////////////////////
    // Threading
#if 0
    Thread* Context::AcquireThread()
    {
        Thread* thread = nullptr;

        easyutil_lock_mutex(m_threadManagementLock);
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
                thread = new Thread();
            }


            // The thread needs to be added to the active threads list.
            assert(thread != nullptr);
            {
                m_activeThreads.PushBack(thread);
            }
        }
        easyutil_unlock_mutex(m_threadManagementLock);


        return thread;
    }

    void Context::UnacquireThread(Thread* thread)
    {
        if (thread != nullptr)
        {
            easyutil_lock_mutex(m_threadManagementLock);
            {
                this->UnacquireThreadNoLock(thread);
            }
            easyutil_unlock_mutex(m_threadManagementLock);
        }
    }

    void Context::UnacquireThreadNoLock(Thread* thread)
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

    void Context::UnacquireAllThreads()
    {
        easyutil_lock_mutex(m_threadManagementLock);
        {
            while (m_activeThreads.GetCount() > 0)
            {
                this->UnacquireThreadNoLock(m_activeThreads.GetBack());
            }
        }
        easyutil_unlock_mutex(m_threadManagementLock);
    }
#endif


    ////////////////////////////////////////////////////
    // Messages

    void Context::Log(const char* message)
    {
        // Write to the log file.
        if (m_pLogFile != NULL)
        {
            char dateTime[64];
            easyutil_datetime_short(easyutil_now(), dateTime, sizeof(dateTime));

            easyvfs_write_string(m_pLogFile, "[");
            easyvfs_write_string(m_pLogFile, dateTime);
            easyvfs_write_string(m_pLogFile, "]");
            easyvfs_write_line  (m_pLogFile, message);
            easyvfs_flush(m_pLogFile);
        }

        // Post to the terminal.
        printf("%s\n", message);
    }

    void Context::Logf(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        {
            char msg[4096];
            vsnprintf(msg, sizeof(msg), format, args);
        
            this->Log(msg);
        }
        va_end(args);
    }

    void Context::LogMessage(const char* message)
    {
        this->Logf("[MESSAGE] %s", message);
    }

    void Context::LogMessagef(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        {
            char msg[4096];
            vsnprintf(msg, sizeof(msg), format, args);
        
            this->LogMessage(msg);
        }
        va_end(args);
    }

    void Context::LogWarning(const char* message)
    {
        this->Logf("[ERROR] %s", message);
    }

    void Context::LogWarningf(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        {
            char msg[4096];
            vsnprintf(msg, sizeof(msg), format, args);
        
            this->LogWarning(msg);
        }
        va_end(args);
    }

    void Context::LogError(const char* message)
    {
        this->Logf("[ERROR] %s", message);
    }

    void Context::LogErrorf(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        {
            char msg[4096];
            vsnprintf(msg, sizeof(msg), format, args);
        
            this->LogError(msg);
        }
        va_end(args);
    }



    ////////////////////////////////////////////////////
    // Audio

    easyaudio_context* Context::GetAudioContext()
    {
        return m_pAudioContext;
    }
        
    easyaudio_device* Context::GetAudioPlaybackDevice()
    {
        return m_pAudioPlaybackDevice;
    }

    GT::SoundWorld & Context::GetSoundWorld()
    {
        return m_soundWorld;
    }



    ////////////////////////////////////////////////////
    // Assets

    AssetLibrary & Context::GetAssetLibrary()
    {
        return m_assetLibrary;
    }
}
