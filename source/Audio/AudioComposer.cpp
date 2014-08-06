// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Audio/AudioComposer.hpp>
#include <GTEngine/GTEngine.hpp>                // <-- For g_EngineContext.
#include <GTEngine/Errors.hpp>
#include "Streamers/SoundStreamer_WAV.hpp"
#include "Streamers/SoundStreamer_Vorbis.hpp"

#include <GTLib/Threading.hpp>
#include <GTLib/IO.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/Strings/Equal.hpp>

namespace GTEngine
{
    /// The thread job that will play a sound.
    class PlaySoundThreadJob : public GTLib::Threading::Job
    {
    public:

        /// Constructor.
        PlaySoundThreadJob(SoundStreamer &streamer, SoundHandle source, AudioBufferHandle buffers[2])
            : streamer(streamer), source(source), backBuffer(buffers[0]), frontBuffer(buffers[1])
        {
        }

        /// Destructor. The destructor will delete all of the input arguments.
        ~PlaySoundThreadJob()
        {
            AudioComposer::DeleteStreamer(&this->streamer);

            if (g_EngineContext != nullptr)
            {
                g_EngineContext->GetAudioSystem().UnqueueAudioBuffer(this->source);
                g_EngineContext->GetAudioSystem().UnqueueAudioBuffer(this->source);

                g_EngineContext->GetAudioSystem().DeleteAudioBuffer(this->backBuffer);
                g_EngineContext->GetAudioSystem().DeleteAudioBuffer(this->frontBuffer);
                g_EngineContext->GetAudioSystem().DeleteSound(this->source);
            }
        }


        void Run()
        {
            // Here we make sure the streamer is at the start.
            this->streamer.Seek(0.0);

            // Before entering the loop below, we're going to fill the front buffer with data. Once this is done, we start playback.
            //
            // Fill and queue the front buffer.
            size_t chunkBuffer1SizeInBytes = 0;
            const void* chunkBuffer1 = this->streamer.ReadNextChunk(chunkBuffer1SizeInBytes);
            if (chunkBuffer1 != nullptr)
            {
                g_EngineContext->GetAudioSystem().SetAudioBufferData(this->frontBuffer, chunkBuffer1, chunkBuffer1SizeInBytes, this->streamer.GetFormat(), this->streamer.GetSampleRate());
                g_EngineContext->GetAudioSystem().QueueAudioBuffer(this->source, this->frontBuffer);
            }
            

            // Start playing.
            g_EngineContext->GetAudioSystem().PlaySound(this->source);


            // Do an initial fill and queue of the back buffer.
            size_t chunkBuffer2SizeInBytes = 0;
            const void* chunkBuffer2 = this->streamer.ReadNextChunk(chunkBuffer2SizeInBytes);
            if (chunkBuffer2 != nullptr)
            {
                g_EngineContext->GetAudioSystem().SetAudioBufferData(this->backBuffer, chunkBuffer2, chunkBuffer2SizeInBytes, this->streamer.GetFormat(), this->streamer.GetSampleRate());
                g_EngineContext->GetAudioSystem().QueueAudioBuffer(this->source, this->backBuffer);
            }
            


            // Now we can start looping until we've finished playing the sound.
            for (;;)
            {
                // If we no longer have an engine context, just return here.
                if (g_EngineContext == nullptr)
                {
                    break;
                }

                // First we swap the buffers. This will not return until the swap has occured.
                this->SwapBuffers();

                size_t chunkSizeInBytes = 0;
                const void* chunkData = this->streamer.ReadNextChunk(chunkSizeInBytes);
                if (chunkData == nullptr)
                {
                    // We've reached the end of the stream.
                    break;
                }

                // We just retrieved valid data, so we just update the buffer again.
                if (g_EngineContext != nullptr)
                {
                    g_EngineContext->GetAudioSystem().SetAudioBufferData(this->backBuffer, chunkData, chunkSizeInBytes, this->streamer.GetFormat(), this->streamer.GetSampleRate());
                    g_EngineContext->GetAudioSystem().QueueAudioBuffer(this->source, this->backBuffer);
                }
            }


            // It looks bad, but we actually delete ourselves after we've finished running.
            delete this;
        }


    private:

        /// Swaps the back and front buffers.
        void SwapBuffers()
        {
            // We need to wait for the front buffer to finish processing before returning. We'll throw ourselves into a slow loop for this.
            for (;;)
            {
                // If we no longer have an engine context, just return here.
                if (g_EngineContext == nullptr)
                {
                    break;
                }

                // We may need to restart playback. If the sound is not playing it may need to be restarted.
                if (!g_EngineContext->GetAudioSystem().IsSoundPlaying(this->source))
                {
                    // If we have some buffer's queue, it means we couldn't fill the buffer's quick enough with data and so the source must be restarted.
                    if (g_EngineContext->GetAudioSystem().GetQueuedAudioBufferCount(this->source) > 0)
                    {
                        g_EngineContext->GetAudioSystem().PlaySound(this->source);
                    }
                }


                // Determines if we have swapped the buffers. 
                bool swapped = false;

                // We do the swapping in a look just in case both the front and back buffers have been processed.
                int processedBufferCount = g_EngineContext->GetAudioSystem().GetProcessedQueuedAudioBufferCount(this->source);
                while (processedBufferCount--)
                {
                    g_EngineContext->GetAudioSystem().UnqueueAudioBuffer(this->source);

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
                GTLib::Threading::Sleep(10);
            }
        }


    private:

        /// The streamer that the sound data will be read from.
        SoundStreamer &streamer;

        /// The sound handle.
        SoundHandle source;

        /// The two buffers to use for streaming.
        AudioBufferHandle backBuffer;
        AudioBufferHandle frontBuffer;
    };
}


// Playback.
namespace GTEngine
{
    bool AudioComposer::Play(const char* fileName, bool relativePositioning)
    {
        return AudioComposer::Play(fileName, 0.0f, 0.0f, 0.0f, relativePositioning);
    }

    bool AudioComposer::Play(const char* fileName, float x, float y, float z, bool relativePositioning)
    {
        (void)x;
        (void)y;
        (void)z;

        // The objects here will be deleted by the destructor of the playback thread job.

        auto streamer = AudioComposer::CreateStreamer(fileName);
        if (streamer != nullptr && streamer->Open())
        {
            // The streamer has been created, so now we will create our audio objects.
            PlaybackDeviceHandle playbackDevice = g_EngineContext->GetAudioPlaybackDevice();
            

            // Sound source.
            SoundHandle source = g_EngineContext->GetAudioSystem().CreateSound(playbackDevice);
            g_EngineContext->GetAudioSystem().SetSoundPosition(source, x, y, z);
            g_EngineContext->GetAudioSystem().SetIsSoundPositionRelative(source, relativePositioning);


            // Sound buffers.
            AudioBufferHandle buffers[2];
            buffers[0] = g_EngineContext->GetAudioSystem().CreateAudioBuffer(playbackDevice);
            buffers[1] = g_EngineContext->GetAudioSystem().CreateAudioBuffer(playbackDevice);


            // Here we start running a thread that will play the sound.
            GTLib::Thread(*(new PlaySoundThreadJob(*streamer, source, buffers)));

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
        g_EngineContext->GetAudioSystem().SetListenerPosition(g_EngineContext->GetAudioListener(), x, y, z);
    }

    void AudioComposer::SetListenerOrientation(const glm::quat &orientation)
    {
        g_EngineContext->GetAudioSystem().SetListenerOrientation(g_EngineContext->GetAudioListener(), orientation);
    }
}


// Streaming.
namespace GTEngine
{
    SoundStreamer* AudioComposer::CreateStreamer(const char* fileName)
    {
        // We want the absolute path to make sure we get the correctly prioritized sound.
        GTLib::String absolutePath;
        if (GTLib::IO::FindAbsolutePath(fileName, absolutePath))
        {
            auto ext = GTLib::Path::Extension(fileName);

            if (GTLib::Strings::Equal<false>(ext, "wav"))
            {
                return new SoundStreamer_WAV(absolutePath.c_str());
            }

            if (GTLib::Strings::Equal<false>(ext, "ogg"))
            {
                return new GT::Engine::SoundStreamer_Vorbis(absolutePath.c_str());
            }
        }

        return nullptr;
    }

    void AudioComposer::DeleteStreamer(SoundStreamer* streamer)
    {
        delete streamer;
    }
}


// Misc.
namespace GTEngine
{
    bool AudioComposer::IsFileExtensionSupported(const char* extension)
    {
        return GTLib::Strings::Equal<false>(extension, "wav") || GTLib::Strings::Equal<false>(extension, "ogg");;
    }
}
