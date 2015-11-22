// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_Engine_EngineContext
#define __GT_Engine_EngineContext

#include "ApplicationConfig.hpp"
#include "MessageDispatcher.hpp"
#include "DefaultMessageHandler.hpp"
#include "Audio/AudioEngine.hpp"
#include "Audio/SoundWorld.hpp"
#include "AssetLibrary.hpp"
#include <GTLib/CommandLine.hpp>
#include <GTLib/Vector.hpp>
#include <GTLib/Threading.hpp>

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
            // Threading

            /// Retrieves a thread.
            ///
            /// @return A pointer to the new thread.
            ///
            /// @remarks
            ///     The returned thread should be unacquired when it is no longer in use.
            ///     @par
            ///     This function is thread-safe.
            GTLib::Thread* AcquireThread();

            /// Unacquires a thread that will previously acquired with AcquireThread().
            ///
            /// @param thread [in] A pointer to the thread to unacquire.
            ///
            /// @remarks
            ///     This function will not wait until the thread is finished executing it's current function.
            ///     @par
            ///     This function is thread-safe.
            void UnacquireThread(GTLib::Thread* thread);
            void UnacquireThread(GTLib::Thread &thread) { this->UnacquireThread(&thread); }

            /// Unacquires a thread without locking.
            ///
            /// @param thread [in] A pointer to the thread to unacquire.
            ///
            /// @remarks
            ///     This function is the same as UnacquireThread(), except that it is not thread safe.
            void UnacquireThreadNoLock(GTLib::Thread* thread);

            /// Unacquires every acquired thread.
            ///
            /// @remarks
            ///     This is mainly intended for the shutdown process so that threads can complete execution before the engine is shutdown.
            void UnacquireAllThreads();



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

            /// Retrieves a handle to the playback device.
            ///
            /// @return A handle to the playback device.
            GTEngine::HPlaybackDevice GetAudioPlaybackDevice();

            /// Retrieves a reference to the global sound world.
            ///
            /// @return A reference to the global sound world for when a sound needs to be played globally, outside of a scene.
            SoundWorld & GetSoundWorld();



            ////////////////////////////////////////////////////
            // Assets

            /// Returns a reference to the internal asset library.
            AssetLibrary & GetAssetLibrary();



        private:

            /// The command line object. This is constructed from the argc and argv parameters in the constructor.
            GTLib::CommandLine m_commandLine;


            /// The absolute path of the directory the application's executable is located in.
            GTLib::String m_executableDirectoryAbsolutePath;


            /// The list of every active thread that is owned by the engine. When a thread is created, it'll be added to this list. When a thread is
            /// deleted, it will not actually be destructed - instead it will be added to the dormant thread list for re-use. This is done this way
            /// so that rapid thread creation and deletion is efficient.
            GTLib::Vector<GTLib::Thread*> m_activeThreads;

            /// The list of every dormant thread that can be reused.
            GTLib::Vector<GTLib::Thread*> m_dormantThreads;

            /// The mutex for controlling access to the thread acquiring/unacquiring procedures.
            GTLib::Mutex m_threadManagementLock;


            /// The application config.
            ApplicationConfig m_applicationConfig;


            /// The default message handler.
            DefaultMessageHandler m_messageHandler;

            /// The message dispatcher.
            MessageDispatcher m_messageDispatcher;


            /// The audio system. This should never be null, but it needs to be a pointer because it's a virtual class.
            GTEngine::AudioEngine* m_audioSystem;

            /// The playback device handle for audio.
            GTEngine::HPlaybackDevice m_audioPlaybackDevice;

            /// The global sound world.
            SoundWorld m_soundWorld;


            /// The asset library.
            AssetLibrary m_assetLibrary;


        private:    // No copying.
            EngineContext(const EngineContext &);
            EngineContext & operator=(const EngineContext &);
        };
    }
}

#endif
