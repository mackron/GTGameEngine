// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Scripting_Rendering_hpp_
#define __GTEngine_Scripting_Rendering_hpp_

#include "../GameScript.hpp"

namespace GT
{
    /// Loads the rendering library.
    ///
    /// @param script [in] A reference to the script to load the library into.
    ///
    /// @return True if the library is loaded successfully; false otherwise.
    bool LoadRenderingLibrary(GT::Script &script);



    namespace RenderingFFI
    {
        /// Enables v-sync on the renderer.
        int EnableVSync(GT::Script &script);

        /// Disables v-sync on the renderer.
        int DisableVSync(GT::Script &script);
    }
}

#endif