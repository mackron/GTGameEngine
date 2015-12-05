// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GTEngine.hpp>
#include <GTGE/Rendering.hpp>
#include <GTGE/ShaderLibrary.hpp>
#include <GTGE/Texture2DLibrary.hpp>
#include <GTGE/MaterialLibrary.hpp>
#include <GTGE/ModelLibrary.hpp>
#include <GTGE/VertexArrayLibrary.hpp>
#include <GTGE/PrefabLibrary.hpp>
#include <GTGE/ParticleSystemLibrary.hpp>
#include <GTGE/ScriptLibrary.hpp>
#include <GTGE/MessageDispatcher.hpp>


namespace GT
{
    /// The global engine context. TODO: Remove this and have applications create this themselves. This global object is only used during the transition phase.
    GT::Context* g_Context = nullptr;


    void Shutdown(Context* pContext)
    {
        if (pContext != nullptr)
        {
            pContext->Shutdown();
        }


        /// Delete the engine context for real.
        delete g_Context;
        g_Context = nullptr;
    }
}
