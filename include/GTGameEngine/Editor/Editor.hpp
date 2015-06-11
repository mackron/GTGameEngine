// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_Editor_hpp_
#define __GT_Editor_hpp_

#include "../Config.hpp"

#if defined(GT_BUILD_EDITOR)
#include "../Core/GUI/GUIContext.hpp"
#include "../WindowManager.hpp"
#include "EditorEventHandler.hpp"
#include "EditorGUIEventHandler.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include "EditorGUIRenderer_GDI.hpp"
#endif

#include <GTLib/Keyboard.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/Vector.hpp>

namespace GT
{
    class GameContext;

    /// Class representing the editor.
    class Editor
    {
    public:

        /// Constructor.
        Editor(GameContext &gameContext);

        /// Destructor.
        ~Editor();


        /// Starts up the editor.
        ///
        /// @return True if the editor is initialised successfully; false otherwise.
        ///
        /// @remarks
        ///     This must be called before using the editor for the first time.
        bool Startup();

        /// Shuts down the editor.
        void Shutdown();


        /// Opens the editor.
        bool Open();

        /// Closes the editor.
        void Close();

        /// Determines whether or not the editor is open.
        bool IsOpen() const;


        /// Steps the editor.
        ///
        /// @param deltaTimeInSeconds [in] The time between frames in seconds.
        ///
        /// @remarks
        ///     This is never called externally unless the editor is running in a real-time loop.
        ///     @par
        ///     The editor itself will call this whenever a viewport needs to be updated.
        ///     @par
        ///     Stepping the editor will update and render all visible 3D viewports.
        void Step(double deltaTimeInSeconds);


        /// Attaches an event handler.
        ///
        /// @param eventHandler [in] A reference to the event handler to attach.
        void AttachEventHandler(EditorEventHandler &eventHandler);

        /// Detaches an event handler.
        ///
        /// @param eventHandler [in] A reference to the event handler to detach.
        void DetachEventHandler(EditorEventHandler &eventHandler);


        ////////////////////////////////////
        // Inbound Events

        /// @copydoc GameContext::OnWantToClose()
        bool OnWantToClose();

        /// @copydoc GameContext::OnWindowResized()
        void OnWindowResized(HWindow hWindow, unsigned int width, unsigned int height);

        /// @copydoc GameContext::OnWindowMoved()
        void OnWindowMoved(HWindow hWindow, int xPos, int yPos);

        /// @copydoc GameContext::OnMouseMove()
        void OnMouseMove(HWindow hWindow, int xPos, int yPos);

        /// @copydoc GameContext::OnMouseButtonPressed()
        void OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos);

        /// @copydoc GameContext::OnMouseButtonReleased()
        void OnMouseButtonReleased(HWindow hWindow, int button, int xPos, int yPos);

        /// @copydoc GameContext::OnMouseButtonDoubleClicked()
        void OnMouseButtonDoubleClicked(HWindow hWindow, int button, int xPos, int yPos);

        /// @copydoc GameContext::OnKeyPressed()
        void OnKeyPressed(HWindow hWindow, GTLib::Key key);

        /// @copydoc GameContext::OnKeyReleased()
        void OnKeyReleased(HWindow hWindow, GTLib::Key key);

        /// @copydoc GameContext::OnPrintableKeyDown()
        void OnPrintableKeyDown(HWindow hWindow, char32_t character);

        /// @copydoc GameContext::OnPaintWindow()
        void OnPaintWindow(HWindow hWindow, const GTLib::Rect<int> &rect);



    private:

        /// Helper method for retrieving the surface that is associated with the given window.
        ///
        /// @param hWindow [in] A handle to the window whose associated surface is being retrieved.
        ///
        /// @return A handle to the surface, or 0 if the window does not have a surface associated with it.
        HGUISurface GetWindowSurface(HWindow hWindow) const;

        /// Creates a GUI surface for the given window.
        ///
        /// @param hWindow [in] A handle to the window that should have a surface created for it.
        ///
        /// @return A handle to the new surface.
        ///
        /// @remarks
        ///     When the surface is created it will be mapped to the window and can be retrieved with GetWindowSurface().
        HGUISurface CreateWindowSurface(HWindow hWindow);

        /// Deletse the GUI surface of the given window.
        ///
        /// @param hWindow [in] A handle to the window whose surface is being deleted.
        void DeleteWindowSurface(HWindow hWindow);


        //////////////////////////////////////
        // Posting Events

        /// Posts the OnEditorOpened() event to every attached event handler.
        void PostEvent_OnEditorOpened();

        /// Posts the OnEditorClosed() event to every attached event handler.
        void PostEvent_OnEditorClosed();


    private:

        /// A reference to the game context.
        GameContext &m_gameContext;

        /// The GUI renderer.
#if defined(GT_PLATFORM_WINDOWS)
        EditorGUIRenderer_GDI m_guiRenderer;
#endif

        /// The GUI context.
        GUIContext m_gui;

        /// The global event handler.
        EditorGUIEventHandler m_globalGUIEventHandler;


        /// The main window.
        HWindow m_hMainWindow;


        /// Every window in the editor has a GUI surface associated with it. This maps a window to a surface.
        GTLib::Map<HWindow, HGUISurface> m_windowSurfaceMap;


        /// The list of event handlers.
        GTLib::Vector<EditorEventHandler*> m_eventHandlers;


        /// Keeps track of whether or not the editor is open.
        bool m_isOpen;



        // Test window.
        HGUIElement m_hTestElement;


    private:    // No copying.
        Editor(const Editor &);
        Editor & operator=(const Editor &);
    };
}

#endif

#endif
