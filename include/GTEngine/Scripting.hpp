
#ifndef __GTEngine_Scripting_hpp_
#define __GTEngine_Scripting_hpp_

#include "GameScript.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the GTEngine script library into the given script.
        ///
        /// @param script [in] The script to load the GTEngine script library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        ///
        /// @remarks
        ///     This only loads the content in the GTEngine namespace/table. This will NOT load any game-specific functionality.
        bool LoadGTEngineScriptLibrary(GTCore::Script &script);



        namespace FFI
        {
            /// Retrieves the executable directory.
            int GetExecutableDirectory(GTCore::Script &script);


            namespace RendererFFI
            {
                /// Enables v-sync on the renderer.
                int EnableVSync(GTCore::Script &script);

                /// Disables v-sync on the renderer.
                int DisableVSync(GTCore::Script &script);
            }


            namespace AudioFFI
            {
                /// Plays a sound.
                ///
                /// @remarks
                ///     Argument 1: The file name of the sound to play.
                int Play(GTCore::Script &script);
            }
        }
    }
}

#endif