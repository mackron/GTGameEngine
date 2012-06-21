
#ifndef __GTEngine_Editor3DViewportEventHandler_hpp_
#define __GTEngine_Editor3DViewportEventHandler_hpp_

#include <GTGUI/ElementEventHandler.hpp>

namespace GTEngine
{
    class Game;
    class SceneViewport;

    /// GUI element event handler for 3D viewports.
    class Editor3DViewportEventHandler : public GTGUI::ElementEventHandler
    {
    public:

        /// Constructor.
        Editor3DViewportEventHandler(Game &game, SceneViewport &viewport);

        /// Destructor.
        ~Editor3DViewportEventHandler();


        //////////////////////////////////////////////////////////////////////////////////
        // Events.

        /// OnSize.
        void OnSize(GTGUI::Element &element);

        /// OnDraw.
        void OnDraw(GTGUI::Element &element);

        /// OnLMBDown.
        void OnLMBDown(GTGUI::Element &element, int x, int y);

        /// OnLMBUp.
        void OnLMBUp(GTGUI::Element &element, int x, int y);

        /// OnRMBDown.
        void OnRMBDown(GTGUI::Element &element, int x, int y);

        /// OnRMBUp.
        void OnRMBUp(GTGUI::Element &element, int x, int y);

        /// OnMouseMove
        void OnMouseMove(GTGUI::Element &element, int x, int y);

        /// OnMouseWhee
        void OnMouseWheel(GTGUI::Element &element, int delta, int x, int y);



    private:

        /// A reference to the game object that viewport operations will be happening on.
        Game &game;

        /// A reference ot the scene viewport that viewport operatiosn will be happening on.
        SceneViewport &viewport;



    private:    // No copying.
        Editor3DViewportEventHandler(const Editor3DViewportEventHandler &);
        Editor3DViewportEventHandler & operator=(const Editor3DViewportEventHandler &);
    };
}

#endif