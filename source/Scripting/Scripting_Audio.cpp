// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Scripting/Scripting_Audio.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/GTEngine.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadAudioLibrary(GTLib::Script &script)
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
            int Play(GTLib::Script &script)
            {
                auto fileName = script.ToString(1);
                auto position = Scripting::ToVector3(script, 2);
                auto relative = script.ToBoolean(3);

                g_EngineContext->GetSoundWorld().PlaySound(fileName, position, relative);
                return 0;
            }

            int SetListenerPosition(GTLib::Script &script)
            {
                g_EngineContext->GetAudioSystem().SetListenerPosition(g_EngineContext->GetAudioPlaybackDevice(), Scripting::ToVector3(script, 1));
                return 0;
            }

            int SetListenerOrientation(GTLib::Script &script)
            {
                g_EngineContext->GetAudioSystem().SetListenerOrientation(g_EngineContext->GetAudioPlaybackDevice(), Scripting::ToVector3(script, 1), Scripting::ToVector3(script, 2));
                return 0;
            }
        }
    }
}