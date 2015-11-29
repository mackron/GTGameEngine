// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Scripting/Scripting_Rendering.hpp>
#include <GTEngine/Rendering.hpp>

namespace GT
{
    bool LoadRenderingLibrary(GT::Script &script)
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
        int EnableVSync(GT::Script &)
        {
            GTEngine::Renderer::SetSwapInterval(1);
            return 0;
        }

        int DisableVSync(GT::Script &)
        {
            GTEngine::Renderer::SetSwapInterval(0);
            return 0;
        }
    }
}