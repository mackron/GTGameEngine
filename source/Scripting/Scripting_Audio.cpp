// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Audio.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>
#include <GTEngine/Audio.hpp>

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

                AudioComposer::Play(fileName, position, relative);
                return 0;
            }

            int SetListenerPosition(GTLib::Script &script)
            {
                AudioComposer::SetListenerPosition(Scripting::ToVector3(script, 1));
                return 0;
            }

            int SetListenerOrientation(GTLib::Script &script)
            {
                AudioComposer::SetListenerOrientation(Scripting::ToQuaternion(script, 1));
                return 0;
            }
        }
    }
}