// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGameEngine/Audio/AudioComposer.hpp>


namespace GT
{
    // Playback.
    bool AudioComposer::Play(const char* fileName, bool relativePositioning)
    {
        return AudioComposer::Play(fileName, 0, 0, 0, relativePositioning);
    }

    bool AudioComposer::Play(const char* fileName, float x, float y, float z, bool relativePositioning)
    {
        return g_EngineContext->GetSoundWorld().PlaySound(fileName, vec3(x, y, z), relativePositioning);
    }


    // Listener.
    void AudioComposer::SetListenerPosition(float x, float y, float z)
    {
        g_EngineContext->GetAudioSystem().SetListenerPosition(g_EngineContext->GetAudioListener(), x, y, z);
    }

    void AudioComposer::SetListenerOrientation(const quat &orientation)
    {
        g_EngineContext->GetAudioSystem().SetListenerOrientation(g_EngineContext->GetAudioListener(), orientation);
    }
}
