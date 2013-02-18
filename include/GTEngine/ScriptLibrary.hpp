// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptLibrary_hpp_
#define __GTEngine_ScriptLibrary_hpp_

namespace GTEngine
{
    class ScriptLibrary
    {
    public:

        /////////////////////////////////////////////////
        // Startup/Shutdown

        /// Starts up the script library.
        static bool Startup();

        /// Shuts down the script library.
        static void Shutdown();

    };
}

#endif