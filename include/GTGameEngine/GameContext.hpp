// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GameContext_hpp_
#define __GT_GE_GameContext_hpp_

#include <GTLib/ResultCodes.hpp>

namespace GT
{
    namespace GE
    {
        class EngineContext;


        /// Class representing a game context.
        ///
        /// An application can have multiple game context's per engine context.
        class GameContext
        {
        public:

            /// Constructor.
            GameContext(EngineContext &engineContext);

            /// Destructor.
            ~GameContext();


            /// Starts up the game context.
            ///
            /// @return A result code specifying whether or not the context was initialized successfully. If the return value is >= 0, it was successful, otherwise it failed.
            ResultCode Startup();

            /// Shuts down the game context.
            void Shutdown();



        private:

            /// A reference to the engine context that owns this game context.
            EngineContext &m_engineContext;


        private:    // No copying.
            GameContext(const GameContext &);
            GameContext & operator=(const GameContext &);
        };
    }
}

#endif
