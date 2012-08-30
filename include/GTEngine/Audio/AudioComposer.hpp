
#ifndef __GTEngine_Audio_AudioComposer_hpp_
#define __GTEngine_Audio_AudioComposer_hpp_

#include "../Math.hpp"
#include "SoundStreamer.hpp"

namespace GTEngine
{
    /// Static class for playing sounds.
    class AudioComposer
    {
    // Startup/Shutdown.
    public:

        /// Starts the composer.
        static bool Startup();

        /// Shuts down the compose.
        static void Shutdown();



    // Playback.
    public:

        /// Plays the sound given by the specified file name.
        ///
        /// @param fileName [in] The file name of the sound to play.
        static bool Play(const char* fileName);
        static bool Play(const char* fileName, float x, float y, float z);
        static bool Play(const char* fileName, const glm::vec3 &position) { return Play(fileName, position.x, position.y, position.z); }



    // Listener.
    public:

        /// Sets the position of the listener.
        static void SetListenerPosition(float x, float y, float z);
        static void SetListenerPosition(const glm::vec3 &position) { SetListenerPosition(position.x, position.y, position.z); }

        /// Sets the orientation of the listener.
        static void SetListenerOrientation(const glm::quat &orientation);



    // Streaming.
    public:

        /// Creates a streamer from the given file.
        ///
        /// @param fileName [in] The name of the file to stream.
        ///
        /// @return A pointer to the new streamer, or null if it could not be created.
        ///
        /// @remarks
        ///     The file's extension will be used in determining the kind of streamer to use (WAV, OGG, etc).
        static SoundStreamer* CreateStreamer(const char* fileName);

        /// Deletes the given streamer.
        static void DeleteStreamer(SoundStreamer* streamer);



    // Misc.
    public:

        /// Determines whether or not the given extension is supported by the composer.
        ///
        /// @param extension [in] The extension to check. For example: "wav", "ogg"
        static bool IsExtensionSupported(const char* extension);
    };
}


#endif
