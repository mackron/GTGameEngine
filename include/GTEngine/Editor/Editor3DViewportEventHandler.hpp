// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Editor3DViewportEventHandler
#define GT_Editor3DViewportEventHandler

#include <GTEngine/GUI/GUIElementEventHandler.hpp>
#include "../Rendering/VertexArray.hpp"

namespace GT
{
    class Game;
    class SceneViewport;

    /// GUI element event handler for 3D viewports.
    class Editor3DViewportEventHandler : public GTGUI::GUIElementEventHandler
    {
    public:

        /// Constructor.
        Editor3DViewportEventHandler(Game &game, SceneViewport &viewport);

        /// Destructor.
        ~Editor3DViewportEventHandler();


        /// Retrieves the position of the mouse, relative to the viewport.
        void GetMousePosition(int &mousePosXOut, int &mousePosYOut) const
        {
            mousePosXOut = this->mousePosX;
            mousePosYOut = this->mousePosY;
        }

        /// Enables mouse capture.
        void EnableMouseControls();

        /// Disables mouse capture.
        void DisableMouseControls();



        //////////////////////////////////////////////////////////////////////////////////
        // Events.

        /// OnSize.
        virtual void OnSize(GTGUI::GUIElement &element);

        /// OnDraw.
        virtual void OnDraw(GTGUI::GUIElement &element);
        virtual bool ImplementsOnDraw() const { return true; }

        /// OnMouseMove
        virtual void OnMouseMove(GTGUI::GUIElement &element, int x, int y);

        /// OnMouseWhee
        virtual void OnMouseWheel(GTGUI::GUIElement &element, int delta, int x, int y);



    private:

        /// A reference to the game object that viewport operations will be happening on.
        Game &game;

        /// A reference ot the scene viewport that viewport operatiosn will be happening on.
        SceneViewport &viewport;


        /// The mouse position on the x axis.
        int mousePosX;

        /// The mouse position on the y axis.
        int mousePosY;


        /// Whether or not mouse capture is enabled.
        bool isMouseControlsEnabled;


    private:    // No copying.
        Editor3DViewportEventHandler(const Editor3DViewportEventHandler &);
        Editor3DViewportEventHandler & operator=(const Editor3DViewportEventHandler &);
    };
}

#endif