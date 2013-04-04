// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Rendering.hpp>
#include <GTEngine/Rendering.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadRenderingLibrary(GTCore::Script &script)
        {
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("Rendering");
                script.PushNewTable();
                {
                    script.SetTableFunction(-1, "EnableVSync",  RenderingFFI::EnableVSync);
                    script.SetTableFunction(-1, "DisableVSync", RenderingFFI::DisableVSync);
                }
                script.SetTableValue(-3);
            }

            return true;
        }


        namespace RenderingFFI
        {
            int EnableVSync(GTCore::Script &)
            {
                Renderer::SetSwapInterval(1);
                return 0;
            }

            int DisableVSync(GTCore::Script &)
            {
                Renderer::SetSwapInterval(0);
                return 0;
            }
        }
    }
}