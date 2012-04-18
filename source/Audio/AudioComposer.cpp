
#include <GTEngine/Audio/AudioComposer.hpp>
#include "Streamers/SoundStreamer_WAV.hpp"
#include "Streamers/SoundStreamer_OGG.hpp"
#include <al/al.h>
#include <al/alc.h>

#include <GTEngine/Errors.hpp>

#include <GTCore/Threading.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/Strings/Equal.hpp>

namespace GTEngine
{
    ALenum ToOpenALFormat(SoundFormat format)
    {
        switch (format)
        {
        case SoundFormat_Mono8:    return AL_FORMAT_MONO8;
        case SoundFormat_Stereo8:  return AL_FORMAT_STEREO8;
        case SoundFormat_Mono16:   return AL_FORMAT_MONO16;
        case SoundFormat_Stereo16: return AL_FORMAT_STEREO16;

        default: break;
        }

        return AL_FORMAT_MONO8;
    }
}

namespace GTEngine
{
    /// The thread job that will play a sound.
    class PlaySoundThreadJob : public GTCore::Threading::Job
    {
    public:

        /// Constructor.
        PlaySoundThreadJob(SoundStreamer &streamer, ALuint source, ALuint buffers[2])
            : streamer(streamer), source(source), backBuffer(buffers[0]), frontBuffer(buffers[1])
        {
        }

        /// Destructor. The destructor will delete all of the input arguments.
        ~PlaySoundThreadJob()
        {
            AudioComposer::DeleteStreamer(&this->streamer);

            alSourceUnqueueBuffers(this->source, 1, &this->backBuffer);
            alSourceUnqueueBuffers(this->source, 1, &this->frontBuffer);

            alDeleteBuffers(1, &this->backBuffer);
            alDeleteBuffers(1, &this->frontBuffer);
            alDeleteSources(1, &this->source);
        }


        void Run()
        {
            // We need a buffer to store chunk data as it's read from the streamer.
            void* chunkBuffer = malloc(this->streamer.GetChunkSize());

            // Here we make sure the streamer is at the start.
            this->streamer.Seek(0.0);

            // Before entering the loop below, we're going to fill the front buffer with data. Once this is done, we start playback.
            this->streamer.ReadChunk(chunkBuffer);
            alBufferData(this->frontBuffer, ToOpenALFormat(this->streamer.GetFormat()), chunkBuffer, this->streamer.GetChunkSize(), this->streamer.GetSampleRate());
            alSourceQueueBuffers(this->source, 1, &this->frontBuffer);

            alSourcePlay(this->source);

            // Before entering the loop, we're going to read in the first chunk of data for the back buffer.
            this->streamer.ReadChunk(chunkBuffer);
            alBufferData(this->backBuffer, ToOpenALFormat(this->streamer.GetFormat()), chunkBuffer, this->streamer.GetChunkSize(), this->streamer.GetSampleRate());
            alSourceQueueBuffers(this->source, 1, &this->backBuffer);

            // Now we can start looping until we've finished playing the sound.
            for (;;)
            {
                // First we swap the buffers. This will not return until the swap has occured.
                this->SwapBuffers();

                uint32_t size = this->streamer.ReadChunk(chunkBuffer);
                if (size == 0)
                {
                    // We've reached the end of the stream.
                    break;
                }

                // We just retrieved valid data, so we just update the buffer again.
                alBufferData(this->backBuffer, ToOpenALFormat(this->streamer.GetFormat()), chunkBuffer, this->streamer.GetChunkSize(), this->streamer.GetSampleRate());
                alSourceQueueBuffers(this->source, 1, &this->backBuffer);
            }


            // We're done with the chunk buffer...
            free(chunkBuffer);

            // It looks bad, but we actually delete ourselves after we've finished running.
            delete this;
        }


    private:

        /// Swaps the back and front buffers.
        void SwapBuffers()
        {
            // We need to wait for the front buffer to finish processing before returning. We'll through ourselves into a slow loop for this.
            for (;;)
            {
                // Check our state. If we need to restart, do so.
                ALuint state = 0;
                alGetSourcei(this->source, AL_SOURCE_STATE, (ALint *)&state);

                // Here is where we'll check if the buffer needs to be restarted.
                if (state != AL_PLAYING)
                {
                    // We need to check the number of buffers that are queued. If we have some, it means
                    // that we couldn't fill the buffers quick enough with data and so the source must
                    // be restarted.
                    ALint queued_buffers;
                    alGetSourcei(this->source, AL_BUFFERS_QUEUED, &queued_buffers);

                    if (queued_buffers > 0)
                    {
                        // We have buffers queued, so we need to restart.
                        alSourcePlay(this->source);
                    }
                }


                // Determines if we have swapped the buffers.
                bool swapped = false;

                int num_processed;
                alGetSourcei(this->source, AL_BUFFERS_PROCESSED, &num_processed);

                // We do the loop just in case both the front and back buffers have been processed.
                while (num_processed--)
                {
                    ALuint buffer;
                    alSourceUnqueueBuffers(this->source, 1, &buffer);

                    auto temp         = this->backBuffer;
                    this->backBuffer  = this->frontBuffer;
                    this->frontBuffer = temp;

                    swapped = true;
                }

                if (swapped)
                {
                    // We ended up swapping, so we need to return our new back buffer.
                    return;
                }

                // Don't use up too much CPU...
                GTCore::Threading::Sleep(10);
            }
        }


    private:

        /// The streamer that the sound data will be read from.
        SoundStreamer &streamer;

        /// The OpenAL source to play.
        ALuint source;

        /// The two buffers to use for streaming.
        ALuint backBuffer;
        ALuint frontBuffer;
    };
}


// Globals.
namespace GTEngine
{
    static ALCdevice*  Device  = nullptr;
    static ALCcontext* Context = nullptr;

    static float ListenerX = 0.0f;
    static float ListenerY = 0.0f;
    static float ListenerZ = 0.0f;
}


// Startup/Shutdown.
namespace GTEngine
{
    bool AudioComposer::Startup()
    {
        Device = alcOpenDevice(nullptr);
        if (Device == nullptr)
        {
            GTEngine::PostError("AudioComposer - Failed to open device.");
            return false;
        }

        // Create our context.
        Context = alcCreateContext(Device, nullptr);
        if (Context == nullptr)
        {
            GTEngine::PostError("AudioComposer - Failed to create context.");
            return false;
        }

        // Now we just need to make our context current and then return.
        alcMakeContextCurrent(Context);

        return true;
    }

    void AudioComposer::Shutdown()
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(Context);
        alcCloseDevice(Device);
    }
}

// Playback.
namespace GTEngine
{
    bool AudioComposer::Play(const char* fileName)
    {
        return AudioComposer::Play(fileName, ListenerX, ListenerY, ListenerZ);
    }

    bool AudioComposer::Play(const char* fileName, float x, float y, float z)
    {
        (void)x;
        (void)y;
        (void)z;

        // The objects here will be deleted by the destructor of the playback thread job.

        auto streamer = AudioComposer::CreateStreamer(fileName);
        if (streamer != nullptr && streamer->Open())
        {
            // The streamer has been created, so now we will create our OpenAL source object.
            ALuint source;
            alGenSources(1, &source);

            // This sets the position of the source.
            alSource3f(source, AL_POSITION, x, y, z);

            // We create two buffers. One for the back which is currently playing, and another for the front which is being filled by the streamer.
            ALuint buffers[2];
            alGenBuffers(2, buffers);

            // Here we start running a thread that will play the sound.
            GTCore::Thread(*(new PlaySoundThreadJob(*streamer, source, buffers)));

            return true;
        }
        else
        {
            GTEngine::PostError("Could not play file: %s.", fileName);
            return false;
        }
    }
}


// Listener.
namespace GTEngine
{
    void AudioComposer::SetListenerPosition(float x, float y, float z)
    {
        ListenerX = x;
        ListenerY = y;
        ListenerZ = z;

        alListener3f(AL_POSITION, x, y, z);
    }

    void AudioComposer::SetListenerOrientation(const glm::quat &orientationIn)
    {
        glm::vec3 forward = orientationIn * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up      = orientationIn * glm::vec3(0.0f, 1.0f,  0.0f);

        ALfloat orientation[6];
        orientation[0] = forward.x;
        orientation[1] = forward.y;
        orientation[2] = forward.z;
        orientation[3] = up.x;
        orientation[4] = up.y;
        orientation[5] = up.z;

        alListenerfv(AL_ORIENTATION, orientation);
    }
}


// Streaming.
namespace GTEngine
{
    SoundStreamer* AudioComposer::CreateStreamer(const char* fileName)
    {
        const char* ext = GTCore::Path::Extension(fileName);

        if (GTCore::Strings::Equal<false>(ext, "wav"))
        {
            return new SoundStreamer_WAV(fileName);
        }

        return nullptr;
    }

    void AudioComposer::DeleteStreamer(SoundStreamer* streamer)
    {
        delete streamer;
    }
}
