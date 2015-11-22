// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Audio/AudioComposer.hpp>
#include <GTEngine/GTEngine.hpp>                // <-- For g_EngineContext.

// Playback.
namespace GTEngine
{
    bool AudioComposer::Play(const char* fileName, bool relativePositioning)
    {
        return AudioComposer::Play(fileName, 0.0f, 0.0f, 0.0f, relativePositioning);
    }

    bool AudioComposer::Play(const char* fileName, float x, float y, float z, bool relativePositioning)
    {
        return g_EngineContext->GetSoundWorld().PlaySound(fileName, glm::vec3(x, y, z), relativePositioning);
    }
}


// Listener.
namespace GTEngine
{
    void AudioComposer::SetListenerPosition(float x, float y, float z)
    {
        g_EngineContext->GetAudioSystem().SetListenerPosition(g_EngineContext->GetAudioPlaybackDevice(), x, y, z);
    }

    void AudioComposer::SetListenerOrientation(glm::vec3 forward, glm::vec3 up)
    {
        g_EngineContext->GetAudioSystem().SetListenerOrientation(g_EngineContext->GetAudioPlaybackDevice(), forward, up);
    }
}


// Misc.
namespace GTEngine
{
    bool AudioComposer::IsFileExtensionSupported(const char* extension)
    {
        return GTLib::Strings::Equal<false>(extension, "wav") || GTLib::Strings::Equal<false>(extension, "ogg");
    }
}
