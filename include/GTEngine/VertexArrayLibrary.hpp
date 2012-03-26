
#ifndef __GTEngine_VertexArrayLibrary_hpp_
#define __GTEngine_VertexArrayLibrary_hpp_

#include "Rendering/VertexArray.hpp"

namespace GTEngine
{
    class VertexArrayLibrary
    {
    public:

        /**
        *   \brief  Retrieves a P2 vertex array that can used for fullscreen quads.
        */
        static VertexArray * GetFullscreenQuadVA();

    };
}

#endif