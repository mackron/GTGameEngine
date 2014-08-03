// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#ifndef __GT_Engine_EngineContext
#define __GT_Engine_EngineContext

#include "ApplicationConfig.hpp"
#include "MessageDispatcher.hpp"
#include "DefaultMessageHandler.hpp"
#include "Audio/AudioEngine.hpp"
#include <GTLib/CommandLine.hpp>

#undef GetCommandLine

namespace GT
{
    namespace Engine
    {
        /// Class representing the engine context.
        class EngineContext
        {
        public:

            /// Constructor.
            EngineContext(int argc, char** argv);

            /// Destructor.
            ~EngineContext();



            ////////////////////////////////////////////////////
            // Command Line

            /// Retrieve a reference to the internal command line object.
            ///
            /// @return A reference to the internal command line object.
            const GTLib::CommandLine & GetCommandLine() const;



            ////////////////////////////////////////////////////
            // Config

            /// Retrieves a reference to the application config object.
            ///
            /// @return A reference to the internal application config object.
            const ApplicationConfig & GetApplicationConfig() const;



            ////////////////////////////////////////////////////
            // File System Management

            /// Retrieves the executable's absolute file path.
            const char* GetExecutableDirectoryAbsolutePath() const;



            ////////////////////////////////////////////////////
            // Messages

            /// @copydoc GT::MessageDispatcher::PostMessage(const Message &)
            void PostMessage(const Message &message);

            /// @copydoc GT::MessageDispatcher::PostMessage(MessageType, const char*)
            void PostMessage(MessageType type, const char* message);

            /// @copydoc GT::MessageDispatcher::PostErrorMessage(const char*)
            void PostErrorMessage(const char* message);

            /// @copydoc GT::MessageDispatcher::PostWarningMessage(const char*)
            void PostWarningMessage(const char* message);

            /// @copydoc GT::MessageDispatcher::PostLogMessage(const char*)
            void PostLogMessage(const char* message);



            ////////////////////////////////////////////////////
            // Audio

            /// Retrieves a reference to the internal audio system.
            ///
            /// @return A reference to the internal audio system.
            GTEngine::AudioEngine & GetAudioSystem();



        private:

            /// The command line object. This is constructed from the argc and argv parameters in the constructor.
            GTLib::CommandLine m_commandLine;

            /// The absolute path of the directory the application's executable is located in.
            GTLib::String m_executableDirectoryAbsolutePath;

            /// The application config.
            ApplicationConfig m_applicationConfig;

            /// The default message handler.
            DefaultMessageHandler m_messageHandler;

            /// The message dispatcher.
            MessageDispatcher m_messageDispatcher;


            /// The audio system. This should never be null, but it needs to be a pointer because it's a virtual class.
            GTEngine::AudioEngine* m_audioSystem;




        private:    // No copying.
            EngineContext(const EngineContext &);
            EngineContext & operator=(const EngineContext &);
        };
    }
}

#endif
