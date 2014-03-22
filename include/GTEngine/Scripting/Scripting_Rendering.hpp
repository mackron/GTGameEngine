// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Rendering_hpp_
#define __GTEngine_Scripting_Rendering_hpp_

#include "../GameScript.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the rendering library.
        ///
        /// @param script [in] A reference to the script to load the library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        bool LoadRenderingLibrary(GTLib::Script &script);



        namespace RenderingFFI
        {
            /// Enables v-sync on the renderer.
            int EnableVSync(GTLib::Script &script);

            /// Disables v-sync on the renderer.
            int DisableVSync(GTLib::Script &script);
        }
    }
}

#endif