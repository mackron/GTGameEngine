// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GameDisplayRenderingCallback_hpp_
#define __GT_GE_GameDisplayRenderingCallback_hpp_

namespace GT
{
    namespace GE
    {
        class GameDisplay;


        /// Class for handling the rendering of a game display.
        class GameDisplayRenderingCallback
        {
        public:

            /// Constructor.
            GameDisplayRenderingCallback();

            /// Destructor.
            virtual ~GameDisplayRenderingCallback();


            /// Performs the rendering operation on the given display.
            ///
            /// @param display         [in] A reference to the display being rendered.
            /// @param commandPipeline [in] A reference to the RenderingCommandPipeline object where rendering commands will be posted to.
            virtual void Render(GameDisplay &display/*, RenderingCommandPipeline &commandPipeline*/) = 0;

        };
    }
}

#endif
