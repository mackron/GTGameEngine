// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GameDisplay_hpp_
#define __GT_GE_GameDisplay_hpp_

namespace GT
{
    namespace GE
    {
        class GPURenderingDevice;
        class GameDisplayRenderingCallback;


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


            /// Retrieves a reference to the rendering device that was passed to the constructor of the display.
            ///
            /// @return A reference to the associated rendering device.
            GPURenderingDevice & GetRenderingDevice();


            /// Sets the rendering callback.
            ///
            /// @param renderingCallback [in] A pointer to the new rendering callback, or null if the current one should be removed.
            ///
            /// @remarks
            ///     The rendering callback is where rendering commands are routed to. When the display needs to be drawn, it is done so through this object.
            void SetRenderingCallback(GameDisplayRenderingCallback* renderingCallback);
            void SetRenderingCallback(GameDisplayRenderingCallback &renderingCallback) { this->SetRenderingCallback(&renderingCallback); }

            /// Retrieve a pointer to the rendering callback, if any.
            ///
            /// @return A pointer to the current rendering callback, or null if there is none currently assigned.
            GameDisplayRenderingCallback* GetRenderingCallback();



        private:

            /// A reference to the GPU rendering device that will be executing the drawing commands.
            GPURenderingDevice &m_renderingDevice;

            /// A pointer to the current rendering command callback object. This can be changed dynamically, but it is up to caller to manage resource cleanup and the like.
            GameDisplayRenderingCallback* m_renderingCallback;



        private:    // No copying.
            GameDisplay(const GameDisplay &);
            GameDisplay & operator=(const GameDisplay &);
        };
    }
}

#endif // !__GT_GE_GameDisplay_hpp_