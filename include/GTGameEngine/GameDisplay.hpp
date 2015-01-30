// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GameDisplay_hpp_
#define __GT_GE_GameDisplay_hpp_

namespace GT
{
    namespace GE
    {
        class GPURenderingDevice;


        enum GameDisplayType
        {
            GameDisplayType_Windowed,
            GameDisplayType_Fullscreen,
            GameDisplayType_Texture
        };


        /// Class representing a game display.
        class GameDisplay
        {
        public:

            /// Constructor.
            GameDisplay(GPURenderingDevice &renderingDevice);

            /// Destructor.
            virtual ~GameDisplay();


            /// Retrieves the type of the game display (windowed, fullscreen or textured)
            ///
            /// @return The type of the game display.
            virtual GameDisplayType GetType() const = 0;


        private:

            /// A reference to the GPU rendering device that will be executing the drawing commands.
            GPURenderingDevice &m_renderingDevice;


        private:    // No copying.
            GameDisplay(const GameDisplay &);
            GameDisplay & operator=(const GameDisplay &);
        };
    }
}

#endif // !__GT_GE_GameDisplay_hpp_
