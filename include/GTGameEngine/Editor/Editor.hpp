// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_Editor_hpp_
#define __GT_Editor_hpp_

#include "../Config.hpp"

#if defined(GT_BUILD_EDITOR)
#include "../GUI/GUIContext.hpp"
#include "../WindowManager.hpp"
#include "EditorEventHandler.hpp"
#include "EditorGUIEventHandler.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include "EditorGUIRenderer_GDI.hpp"
#include "../GUI/GUIFontManager_GDI.hpp"
#endif

#include <GTLib/Keyboard.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/Vector.hpp>

namespace GT
{
    class GameContext;
    class EditorHeader;
    class EditorBody;
    class EditorFooter;

    /// Class representing the editor.
    ///
    /// The editor's GUI is made up of controls. Each control is made up of a number of other child controls.
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


        /// Retrieves a reference to the editor's GUI context.
        ///
        /// @return A reference to the editor's GUI context.
              GUIContext & GetGUI()       { return m_gui; }
        const GUIContext & GetGUI() const { return m_gui; }


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


        /// Creates a window and an associated GUI surface.
        ///
        /// @param hParentWindow [in] The parent window.
        /// @param windowType    [in] The type of window to create.
        /// @param xPos          [in] The position of the window on the x axis relative to the parent's top left corner.
        /// @param yPos          [in] The position of the window on the y axis relative to the parent's top left corner.
        /// @param width         [in] The width of the window.
        /// @param height        [in] The height of the window.
        /// @param hElement      [in] (Optional) The GUI element to use as the main element. This can be null, in which case a new element will be created.
        HWindow CreateWindow(HWindow hParentWindow, WindowType windowType, int xPos, int yPos, unsigned int width, unsigned int height, HGUIElement hElement = NULL);

        /// Deletes the given window that was created with CreateWindow().
        ///
        /// @param hWindow [in] The window to delete.
        ///
        /// @remarks
        ///     This should only be called for a window that was created with Editor::CreateWindow()
        void DeleteWindow(HWindow hWindow);

        /// Shows the given window.
        ///
        /// @param hWindow [in] The window to show.
        void ShowWindow(HWindow hWindow);

        /// Hides the given window.
        ///
        /// @param hWindow [in] The window to hide.
        void HideWindow(HWindow hWindow);

        /// Sets the position of the given window.
        ///
        /// @param hWindow [in] A handle to the window whose position is being set.
        /// @param xPos    [in] The position of the window on the x axis relative to the top-left corner of the parent window.
        /// @param yPos    [in] The position of the window on the y axis relative to the top-left corner of the parent window.
        void SetWindowPosition(HWindow hWindow, int xPos, int yPos);

        /// Sets the size of the given window.
        ///
        /// @param hWindow [in] A handle to the window whose size is being set.
        /// @param width   [in] The new width of the window.
        /// @param height  [in] The new height of the window.
        void SetWindowSize(HWindow hWindow, unsigned int width, unsigned int height);


        /// Retrieves the surface that is associated with the given window.
        ///
        /// @param hWindow [in] A handle to the window whose associated surface is being retrieved.
        ///
        /// @return A handle to the window's surface, or null if there was an error.
        HGUISurface GetWindowSurface(HWindow hWindow) const;

        /// Retrieves the element that is associated with the given window.
        ///
        /// @param hWindow [in] A handle to the window whose associated window is being retrieved.
        ///
        /// @return A handle to the window's element, or null if there was an error.
        HGUIElement GetWindowElement(HWindow hWindow) const;

        /// Retrieves the window that is associated with the given surface.
        ///
        /// @param hSurface [in] A handle to the surface whose associated window is being retrieved.
        ///
        /// @return A handle to the window that is associated with the given surface, if any.
        HWindow GetSurfaceWindow(HGUISurface hSurface) const;


        ////////////////////////////////////
        // Inbound Events

        /// @copydoc GameContext::OnWantToClose()
        bool OnWantToClose();

        /// @copydoc GameContext::OnWindowCreated()
        void OnWindowCreated(HWindow hWindow);

        /// @copydoc GameContext::OnWindowDeleting()
        void OnWindowDeleting(HWindow hWindow);

        /// @copydoc GameContext::OnWindowResized()
        void OnWindowResized(HWindow hWindow, unsigned int width, unsigned int height);

        /// @copydoc GameContext::OnWindowMoved()
        void OnWindowMoved(HWindow hWindow, int xPos, int yPos);

        /// @copydoc GameContext::OnMouseMove()
        void OnMouseMove(HWindow hWindow, int xPos, int yPos);

        /// @copydoc GameContext::OnMouseEnter()
        void OnMouseEnter(HWindow hWindow);

        /// @copydoc GameContext::OnMouseLeave()
        void OnMouseLeave(HWindow hWindow);

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

        /// Every window in the editor is associated with a surface, which also has a root level element. This structure contains that information.
        struct WindowGUISurfaceAndElement
        {
            /// Constructor.
            WindowGUISurfaceAndElement()
                : hSurface(NULL), hElement(NULL)
            {
            }


            /// The window's surface.
            HGUISurface hSurface;

            /// The window's root level element.
            HGUIElement hElement;
        };


        /// Helper method for retrieving the surface that is associated with the given window.
        ///
        /// @param hWindow              [in]      A handle to the window whose associated surface is being retrieved.
        /// @param surfaceAndElementOut [in, out] A reference to the object that will receive the surface and element.
        ///
        /// @return True if the surface and element was retrieved successfully.
        ///
        /// @remarks
        ///     To use an already created surfance and/or element, set them in \c surfaceAndElementOut before calling.
        bool GetWindowSurfaceAndElement(HWindow hWindow, WindowGUISurfaceAndElement &surfaceAndElementOut) const;

        /// Creates a GUI surface for the given window.
        ///
        /// @param hWindow              [in]  A handle to the window that should have a surface created for it.
        /// @param surfaceAndElementOut [out] A reference to the object that will receive the surface and element.
        ///
        /// @return True if the surface and element was created successfully.
        ///
        /// @remarks
        ///     When the surface is created it will be mapped to the window and can be retrieved with GetWindowSurface().
        bool CreateWindowSurfaceAndElement(HWindow hWindow, WindowGUISurfaceAndElement &surfaceAndElementOut);
        bool CreateWindowSurfaceAndElement(HWindow hWindow);

        /// Deletse the GUI surface of the given window.
        ///
        /// @param hWindow [in] A handle to the window whose surface is being deleted.
        void DeleteWindowSurfaceAndElement(HWindow hWindow);


        


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

        /// The GUI font manager.
#if defined(GT_PLATFORM_WINDOWS)
        GUIFontManager_GDI m_guiFontManager;
#endif

        /// The GUI context.
        GUIContext m_gui;

        /// The global event handler.
        EditorGUIEventHandler m_globalGUIEventHandler;


        /// The main window.
        HWindow m_hMainWindow;


        /// Every window in the editor has a GUI surface associated with it. This maps a window to a surface/element.
        GTLib::Map<HWindow, WindowGUISurfaceAndElement> m_windowSurfaceMap;


        /// The list of event handlers.
        GTLib::Vector<EditorEventHandler*> m_eventHandlers;


        /// Keeps track of whether or not the editor is open.
        bool m_isOpen;


        /// The header control.
        EditorHeader* m_pHeaderControl;

        /// The body control.
        EditorBody* m_pBodyControl;

        /// The footer control.
        EditorFooter* m_pFooterControl;


    private:    // No copying.
        Editor(const Editor &);
        Editor & operator=(const Editor &);
    };
}

#endif

#endif
