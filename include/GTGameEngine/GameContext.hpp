// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GameContext_hpp_
#define __GT_GE_GameContext_hpp_

#include "Config.hpp"
#include <GTLib/ResultCodes.hpp>
#include <GTLib/Vector.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

namespace GT
{
    namespace GE
    {
        class EngineContext;
        class GameDisplay;
        class GameDisplay_Windowed;
        class GPURenderingDevice;

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
            /// @param renderingDevice [in] The rendering device that will be used for rendering to the display.
            /// @param hWnd            [in] The window to create the display from.
            /// @param displayOut      [in] A reference to the variable that will receive a pointer to the newly created display object.
            ///
            /// @return >=0 if there is no error, <0 if there was an error.
            ResultCode CreateWindowedDisplay(GPURenderingDevice &renderingDevice, HWND hWnd, GameDisplay* &displayOut);
#endif

            /// Deletes the given display.
            ///
            /// @param display [in] A pointer to the display to delete.
            void DeleteDisplay(GameDisplay* display);



            /// Steps the game by one frame.
            ///
            /// @remarks
            ///     This will update and render the game.
            void Step();


        private:

            /// A reference to the engine context that owns this game context.
            EngineContext &m_engineContext;

            /// The list of window displays.
            GTLib::Vector<GameDisplay_Windowed*> m_windowedDisplays;


            /// A set of flags for boolean variables.
            uint32_t m_flags;


        private:

            ///////////////////////////////////////////////////////////////
            // State Flags

            static const uint32_t IsSingleThreaded = 0x00000001;        //< Whether or not the game should run in single- or multi-threaded mode.



        public:

            ///////////////////////////////////////////////////////////////
            // Error Codes

            static const ResultCode InvalidWindow = (1 << 31) | 1;


        private:    // No copying.
            GameContext(const GameContext &);
            GameContext & operator=(const GameContext &);
        };
    }
}

#endif