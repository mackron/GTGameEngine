
#ifndef __GTEngine_GUIRenderer_hpp_
#define __GTEngine_GUIRenderer_hpp_

namespace GTGUI
{
    class Server;
};

namespace GTEngine
{
    /**
    *   \brief  Static class for rendering GUI's from GTGUI.
    */
    class GUIRenderer
    {
    public:

        /**
        *   \brief  Initialises the GUI renderer.
        */
        static void Initialise();

        /**
        *   \brief  Uninitialises the GUI renderer.
        */
        static void Uninitialise();


    public:

        /**
        *   \brief  Main function for drawing a GUI.
        */
        static void DrawGUI(const GTGUI::Server &gui);

        /**
        *   \brief  Places the renderer into a state that's correct for drawing GUI elements.
        *
        *   \remarks
        *       This function is needed in case an application wants to changed the renderer state during a call to an element's OnDraw event.
        */
        static void EnableGUIState();
    };
}

#endif