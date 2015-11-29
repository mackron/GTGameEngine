// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Scripting/Scripting_Audio.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/GTEngine.hpp>

namespace GT
{
    bool LoadAudioLibrary(GT::Script &script)
    {
        script.GetGlobal("GTEngine");
        assert(script.IsTable(-1));
        {
            script.Push("Audio");
            script.PushNewTable();
            {
                script.SetTableFunction(-1, "Play", AudioFFI::Play);
            }
            script.SetTableValue(-3);
        }

        return true;
    }

    namespace AudioFFI
    {
        int Play(GT::Script &script)
        {
            auto fileName = script.ToString(1);
            auto position = ToVector3(script, 2);
            auto relative = script.ToBoolean(3);

            g_EngineContext->GetSoundWorld().PlaySound(fileName, position, relative);
            return 0;
        }

        int SetListenerPosition(GT::Script &script)
        {
            glm::vec3 pos = ToVector3(script, 1);
            easyaudio_set_listener_position(g_EngineContext->GetAudioPlaybackDevice(), pos.x, pos.y, pos.z);

            return 0;
        }

        int SetListenerOrientation(GT::Script &script)
        {
            glm::vec3 forward = ToVector3(script, 1);
            glm::vec3 up = ToVector3(script, 1);
            easyaudio_set_listener_orientation(g_EngineContext->GetAudioPlaybackDevice(), forward.x, forward.y, forward.z, up.x, up.y, up.z);

            return 0;
        }
    }
}