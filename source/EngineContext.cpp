// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#include <GTEngine/EngineContext.hpp>
#include "Audio/OpenAL/AudioEngine_OpenAL.hpp"

namespace GT
{
    namespace Engine
    {
        EngineContext::EngineContext(int argc, char** argv)
            : m_commandLine(argc, argv),
              m_executableDirectoryAbsolutePath(),
              m_applicationConfig(),
              m_messageHandler(), m_messageDispatcher(),
              m_audioSystem(nullptr), m_audioPlaybackDevice(0), m_audioListener(0)
        {
            // First this is to more into the applications directory. We get this from the command line.
            GTLib::IO::SetCurrentDirectory(m_commandLine.GetApplicationDirectory());

            // We need to keep hold of the executable directory for GetExecutableDirectory().
            m_executableDirectoryAbsolutePath = GTLib::IO::ToAbsolutePath(m_commandLine.GetApplicationDirectory(), GTLib::IO::GetCurrentDirectory());

            // After moving into the application directory, we need to load up the config file and move into the data directory. From
            // there we can read the user configs and setup the log file.
            //
            // This is different from a user configuration (which are located in the 'configs' folder). The application configuration
            // usually remains constant. It defines things like directories. We won't return false if we fail to open, in which case
            // the game will use defaults.
            if (m_applicationConfig.Open("config.lua"))
            {
                // The application config will define the data directories where all of the game's data and assets are located. We will
                // move into the directory of the first defined data directory.
                auto &directories = m_applicationConfig.GetDataDirectories();
                if (directories.count > 0)
                {
                    GTLib::IO::SetCurrentDirectory(directories[0].c_str());

                    // Here we are going to set additional search directories which will make GTLib search these directories if it can not
                    // open a file from the current directory. We intentionally don't include the first directory.
                    for (size_t i = 1; i < directories.count; ++i)
                    {
                        GTLib::IO::AddAdditionalSearchPath(directories[i].c_str());
                    }
                }
            }


            // We will need to open the log file as soon as possible, but it needs to be done after ensuring the current directory is set to that of the executable.
            const char** cmdLine_logfile = m_commandLine.GetArgument("logfile");
            if (cmdLine_logfile != nullptr)
            {
                m_messageHandler.OpenLogFile(cmdLine_logfile[0]);
            }
            else
            {
                m_messageHandler.OpenLogFile("var/logs/engine.html");
            }


            // At this point the message handler should be setup, so we'll go ahead and add it to the dispatcher.
            m_messageDispatcher.AddMessageHandler(m_messageHandler);



            //////////////////////////////////////////
            // Audio System
            //
            // TODO: Add support for multiple audio systems.
            m_audioSystem = new GTEngine::AudioEngine_OpenAL();
            if (m_audioSystem->Startup())
            {

            }
            else
            {
                // TODO: Use the null audio system.
                this->PostWarningMessage("Failed to create audio system. Defaulting to null audio system.");
            }

            assert(m_audioSystem != nullptr);
            {
                // TEMP: Print the playback devices.
                size_t playbackDeviceCount = m_audioSystem->GetPlaybackDeviceCount();
                for (size_t iDevice = 0; iDevice < playbackDeviceCount; ++iDevice)
                {
                    auto deviceInfo = m_audioSystem->GetPlaybackDeviceInfo(iDevice);

                    GTLib::String message;
                    message.AppendFormatted("Playback Device (%d) - %s", static_cast<int>(iDevice), deviceInfo.name.c_str());

                    this->PostLogMessage(message.c_str());
                }

                m_audioPlaybackDevice = m_audioSystem->OpenPlaybackDevice(0);
                m_audioListener = m_audioSystem->CreateListener(m_audioPlaybackDevice);
            }
        }

        EngineContext::~EngineContext()
        {
            //////////////////////////////////////////
            // Audio System

            m_audioSystem->DeleteListener(m_audioListener);
            m_audioListener = 0;

            m_audioSystem->ClosePlaybackDevice(m_audioPlaybackDevice);
            m_audioPlaybackDevice = 0;

            delete m_audioSystem;
            m_audioSystem = nullptr;
        }



        ////////////////////////////////////////////////////
        // Command Line

        const GTLib::CommandLine & EngineContext::GetCommandLine() const
        {
            return m_commandLine;
        }



        ////////////////////////////////////////////////////
        // Command Line

        const ApplicationConfig & EngineContext::GetApplicationConfig() const
        {
            return m_applicationConfig;
        }



        ////////////////////////////////////////////////////
        // File System Management

        const char* EngineContext::GetExecutableDirectoryAbsolutePath() const
        {
            return m_executableDirectoryAbsolutePath.c_str();
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

        GTEngine::AudioEngine & EngineContext::GetAudioSystem()
        {
            assert(m_audioSystem != nullptr);
            return *m_audioSystem;
        }

        GTEngine::PlaybackDeviceHandle EngineContext::GetAudioPlaybackDevice()
        {
            assert(m_audioPlaybackDevice != 0);
            return m_audioPlaybackDevice;
        }

        GTEngine::ListenerHandle EngineContext::GetAudioListener()
        {
            assert(m_audioListener != 0);
            return m_audioListener;
        }
    }
}