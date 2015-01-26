// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GameContext_hpp_
#define __GT_GE_GameContext_hpp_

#include <GTLib/ResultCodes.hpp>
#include <GTLib/Config.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

namespace GT
{
    namespace GE
    {
        class EngineContext;
        class GameDisplay;


        // Error codes.
        static const ResultCode InvalidWindow = (1 << 31) | 1;



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



#if defined(GT_PLATFORM_WINDOWS)
            /// Creates a windowed display from the given window.
            ///
            /// @param hWnd       [in] The window to create the display from.
            /// @param displayOut [in] A reference to the variable that will receive a pointer to the newly created display object.
            ///
            /// @return >=0 if there is no error, <0 if there was an error.
            ResultCode CreateWindowedDisplay(HWND hWnd, GameDisplay* &displayOut);
#endif

            /// Deletes the given display.
            ///
            /// @param display [in] A pointer to the display to delete.
            void DeleteDisplay(GameDisplay* display);



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
