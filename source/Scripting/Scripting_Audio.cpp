// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Audio.hpp>
#include <GTEngine/Audio.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadAudioLibrary(GTCore::Script &script)
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
            int Play(GTCore::Script &script)
            {
                AudioComposer::Play(script.ToString(1));
                return 0;
            }
        }
    }
}