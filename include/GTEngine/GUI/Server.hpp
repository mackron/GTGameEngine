// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_Server
#define GT_GUI_Server

#include "ScriptServer.hpp"
#include "StyleServer.hpp"
#include "ServerEventHandler.hpp"
#include "MarkupLoader.hpp"
#include "Element.hpp"
#include "Caret.hpp"
#include "EventQueue.hpp"
#include "GarbageCollector.hpp"
#include "LayoutManager.hpp"
#include "Rendering.hpp"
#include "ImageManager.hpp"
#include "FontGlyphMapManager.hpp"
#include "FontCache.hpp"
#include "ElementTree.hpp"
#include <GTEngine/Core/Point.hpp>
#include <GTEngine/Core/Rect.hpp>
#include <GTEngine/Core/Dictionary.hpp>
#include <GTEngine/Core/SortedVector.hpp>
#include <GTEngine/Core/Timing.hpp>
#include <GTEngine/Core/Threading.hpp>
#include <GTEngine/Core/FontServer.hpp>

/*
--- Implementation Details ---

Mouse Events:

We use the notion of invalidating/validating the mouse. As the layout of elements change, the mouse is invalidated because it is possible that the new
layout has changed the elements that are under the mouse. At the beginning of each step, we validate the mouse, which will post the appropriate events.
*/

namespace GTGUI
{
    static const int OperationMode_Delayed   = 0;
    static const int OperationMode_Immediate = 1;
    

    struct EventContext
    {
        /// The element to post keyboard events to.
        Element* keyEventsTarget;
    };

    
    /**
    *   \brief  Class representing a server.
    *
    *   GTGUI follows a Client/Server model. The native application code is the client. The server manages the GUI scripts, styles
    *   and element markup.
    *
    *   An application must create a server before it can start using or building a GUI. The application (client) communicates with the
    *   GUI elements via the server.
    *
    *   A server is initialised during it's constructor. There is no "Initialise()" method. You can use IsInitialised() to determine
    *   if the server has been initialised correctly.
    *
    *   Font management is done via GTType and it's FontServer system. You can specify your own font server in the constructor, or
    *   set it to null to let the server manage the fonts itself. It's recommended to pass your own font server so that you can do
    *   application-specific font management.
    *
    *   An application can attach event handlers to the server. Methods on any attached event handler will be called in different
    *   situations. For example, errors and warnings will be sent through the event handlers. Event handlers are created from the
    *   ServerEventHandler class.
    *
    *   Do not inherit from this class. The destructor is non-virtual to promote this.
    */
    class Server
    {
    public:
    
        /**
        *   \brief  Constructor.
        *   \param  script     [in] A pointer to the script to use for scripting. Can be null, in which case GTGUI will create it's own script object.
        */
        Server(GT::Script* script, ImageManager* imageManagerIn = nullptr);
    
        /**
        *   \brief  Destructor.
        */
        ~Server();
    
        /**
        *   \brief  Determines if the server is initialised.
        *   \return True if the server is initialised and useable; false otherwise.
        */
        bool IsInitialised() const;
        
        
        /// Sets the operation mode.
        ///
        /// @param mode [in] The new operation mode. Can be OpereationMode_Delayed or OperationMode_Immediate.
        void SetOperationMode(int mode);
        
        /// Retrieves the current operation mode.
        ///
        /// @return The current operation mode - either OperationMode_Delayed or OperationMode_Immediate.
        int GetOperationMode() const;
        
        /// A helper method for determining whether or not the server is currently in immediate mode.
        bool IsInImmediateMode() const { return this->operationMode == OperationMode_Immediate; }
        
        /// A helper method for determining whether or not the server is currently in delayed mode.
        bool IsInDelayedMode() const { return this->operationMode == OperationMode_Delayed; }
        
        
        /**
        *   \brief  Sets the event handler.
        */
        void SetEventHandler(ServerEventHandler &eventHandler);
        
        /**
        *   \brief  Retrieves a reference to the event handler.
        */
        ServerEventHandler & GetEventHandler();
        
        /**
        *   \brief  Retrieves a reference to the script server.
        */
        ScriptServer & GetScriptServer();
        
        /**
        *   \brief  Retrieves a reference to the style server.
        */
        StyleServer & GetStyleServer();
        
        
        /**
        *   \brief                     Loads an XML script.
        *   \param  xml          [in]  The XML script to load. This is \b not a file name - it's raw XML text.
        *   \param  absDirectory [in]  The base path to use when constructing absolute directories from relative paths. Can be null.
        *   \param  topID        [out] A pointer to a C-style string that will receive a pointer to the ID of the top element.
        *   \return                    True if the XML script is loaded successfully; false otherwise.
        *
        *   \remarks
        *       This function is thread-safe through the use of a global mutex. The mutex is used even across
        *       multiple instantiations. This is because we use a global error handler function for XML (due
        *       to how we handle errors in rapidxml). Without the mutex, thread safety could not be achieved,
        *       even across seperate instantiations. Typically there will only be a single Server instantiation
        *       for an entire application, so it shouldn't too much of an issue.
        *       \par
        *       If <absDirectory> is null, the current path and additional search directories defined in GTLib will be used
        *       when constructing absolute paths.
        *       \par
        *       The topID parameter can be used to get a pointer to that element. This is required so we can do
        *       a return value for the GTGUI.Server.New() script function. Delete this string with Strings::Delete().
        */
        bool Load(const char* xml, const char* absDirectory, GT::String &topID);
        bool Load(const char* xml, const char* absDirectory = nullptr);
        bool Load(const char* xml, GT::String &topID);
        
        /**
        *   \brief                Loads and parses an XML script from a file.
        *   \param  fileName [in] The name of the file to load.
        */
        bool LoadFromFile(const char* filename);

        /**
        *   \brief  Determines whether or not the given file is loaded.
        */
        bool IsFileLoaded(const char* filename);


        /// Executes the given script.
        ///
        /// @param script [in] The script string to execute.
        ///
        /// @return True if the script is loaded successfully; false otherwise.
        bool ExecuteScript(const char* script, int returnValueCount = -1);

        /// Executes a style script.
        ///
        /// @param script      [in] The style script to execute.
        /// @param baseURLPath [in] The base path to use with relative paths when loading things like images.
        ///
        /// @return True if the script is loaded successfully; false otherwise.
        bool ExecuteStyleScript(const char* script, const char* baseURLPath = nullptr);


        /// Sets the image manager.
        ///
        /// @remarks
        ///     Only a single image manager can be attached to a server at a time.
        void SetImageManager(ImageManager* imageManager);
        void SetImageManager(ImageManager &imageManager) { this->SetImageManager(&imageManager); }
        
        /// Retrieves a pointer to the current image manager.
              ImageManager* GetImageManager()       { return m_imageManager; }
        const ImageManager* GetImageManager() const { return m_imageManager; }


        /// Sets the renderer that will receive the rendering operations.
        ///
        /// @remarks
        ///     Only a single renderer can be attached to a server at a time.
        ///     @par
        ///     This is a simple setter so it is the responsibility of the client application for cleanup and whatnot.
        void SetRenderer(Renderer* renderer);
        void SetRenderer(Renderer &renderer) { this->SetRenderer(&renderer); }

        /// Retrieves a pointer to the current renderer.
              Renderer* GetRenderer()       { return m_renderer; }
        const Renderer* GetRenderer() const { return m_renderer; }



        /// A helper method for loading an XML script and returning a pointer to the top element.
        ///
        /// @param xml          [in] The XML script to use to create the element.
        /// @param absDirectory [in] The absolute directory to use when resolving URLs and file paths.
        ///
        /// @return The top-most element in the given XML script.
        Element* CreateElement(const char* xml, const char* absDirectory = nullptr);

        /// Creates an empty element.
        ///
        /// @param id            [in] The ID of the element.
        /// @param parentElement [in] A reference to the parent element.
        ///
        /// @return A pointer to the new element.
        Element* CreateElement(const char* id, Element* parentElement);
        Element* CreateElement(const char* id, Element &parentElement) { return CreateElement(id, &parentElement); }
        Element* CreateElement(Element* parentElement);
        Element* CreateElement(Element &parentElement) { return CreateElement(&parentElement); }

        /// Deletes an element. This will also remove any references in the scripting environment.
        ///
        /// @param  elementToDelete        [in] A pointer ot the element being deleted.
        /// @param  deleteChildren         [in] Whether or not children should also be deleted. True by default.
        /// @param  invalidateParentLayout [in] Whether or not the parent's layout should be invalidated after removal.
        ///
        /// @remarks
        ///     This will detach all event handlers.
        void DeleteElement(Element* elementToDelete, bool deleteChildren = true, bool invalidateParentLayout = true);


        /// Retrieves a string that can be used for an anonymous element.
        ///
        /// @param destBuffer            [in] A pointer to the buffer that will receive the ID.
        /// @param destBufferSizeInBytes [in] The size in bytes of 'destBuffer'.
        ///
        /// @remarks
        ///     This will simply copy the string into the given buffer.
        ///     @par
        ///     This will mutate an internal counter such that the ID's should be sequential.
        void NextAnonymousID(char* destBuffer, size_t destBufferSizeInBytes);
        

        /**
        *   \brief              Sets the size of the GUI working area.
        *   \param  width  [in] The width of the main GUI working area.
        *   \param  height [in] The height of the main GUI working area.
        */
        void SetViewportSize(unsigned int width, unsigned int height);

        /**
        *   \brief               Retrieves the size of the GUI working area.
        *   \param  width  [out] A reference to the variable that will receive the width of the GUI working area.
        *   \param  height [out] A reference to the variable that will receive the height of the GUI working area.
        */
        void GetViewportSize(unsigned int &width, unsigned int &height) const;

        /// Retrieves the width of the viewport.
        unsigned int GetViewportWidth() const { return this->viewportWidth; }

        /// Retrieves the height of the viewport.
        unsigned int GetViewportHeight() const { return this->viewportHeight; }

    
        /**
        *   \brief                     Retrieves a pointer to an element based on it's ID.
        *   \param  id            [in] The ID of the element being retrieved.
        *   \param  idSizeInBytes [in] The size in bytes of the ID, not including the null terminator.
        *
        *   \remarks
        *       Use idSizeInBytes when \c id can not easily be null terminated, or if you happen to know the length before hand.
        */
        Element* GetElementByID(const char *id, ptrdiff_t idSizeInBytes = -1) const;

        /// Helper method for retrieving the root element.
        ///
        /// @return A pointer to the root element.
        Element* GetRootElement() { return this->GetElementByID("_Root"); }
        

        /// Steps the GUI by the given delta time, in seconds.
        ///
        /// @remarks
        ///     A step operation is simply a way of moving time forward in the GUI to allow time-based operations to work properly.
        void Step();
        void Step(double deltaTimeInSeconds);


        /// Repaints a section of the GUI.
        ///
        /// @param left   [in] The position of the left side of the rectangle to repaint.
        /// @param top    [in] The position of the top side of the rectangle to repaint.
        /// @param right  [in] The position of the right side of the rectangle to repaint.
        /// @param bottom [in] The position of the bottom side of the rectangle to repaint.
        ///
        /// @remarks
        ///     'left' must be less or equal to 'right' and 'top' must be less or equal to 'bottom'.
        void Paint(int left, int top, int right, int bottom);
        void Paint();


        /// Updates the layout of elements, posting all of the relevant events.
        void UpdateLayout();

        
        /// Fills the given FontInfo object based on the given element.
        ///
        /// @param element [in]  The element whose style will be used to retrieve th font information.
        /// @param fi      [out] A reference to the FontInfo object that will receive the data.
        void GetFontInfoFromElement(const Element &element, GT::FontInfo &fi) const;
        
        
        /// Acquires a font from the given font info.
        ///
        /// @param fi [in] The FontInfo object representing the font to acquire.
        ///
        /// @return A pointer to the font.
        ///
        /// @remarks
        ///     If a font has already been acquired from the same font info, a pointer to the already-acquired font
        ///     will be returned.
        GT::Font* AcquireFont(const GT::FontInfo &fi);
        
        /// Unacquires a font that was previously acquired with AcquireFont().
        ///
        /// @param font [in] A pointer to the font to unacquire.
        void UnacquireFont(const GT::Font* font);


        /// Acquires an image based on an absolute URL or ID.
        ///
        /// @param absURLOrID [in] The absolute URL or ID of the image to acquire.
        ///
        /// @return A pointer to the newly acquired image.
        ///
        /// @remarks
        ///     If <absURLOrID> is a path and the image has not yet been loaded, it will be loaded.
        ImageHandle AcquireImage(const char* absURLOrID);



        /// Marks the beginning of a bunch of event postings.
        ///
        /// @return An object to post events to.
        ///
        /// @remarks
        ///     The returned object must be passed to EndPostingEvents() when the batch of events have been posted.
        EventContext BeginPostingEvents();

        /// Marks the end of a bunch of event postings.
        ///
        /// @param eventContext [in] The return value from the corresponding BeginPostingEvents() call.
        void EndPostingEvents(EventContext eventContext);


        /// Applications should call this when the mouse has moved. This will allow GTGUI to send and handle mouse events.
        ///
        /// @param x [in] The x position of the mouse.
        /// @param y [in] The y position of the mouse.
        void OnMouseMove(EventContext eventContext, int x, int y);
        
        /// Called by applications when the mouse wheel is turned.
        ///
        /// @param delta [in] The mouse wheel delta (how the wheel was turned).
        /// @param x     [in] The x position of the mouse.
        /// @param y     [in] The y position of the mouse.
        void OnMouseWheel(EventContext eventContext, int delta, int x, int y);
        
        /// Called by applications when a mouse button is pressed.
        ///
        /// @param button [in] The index of the button that was pressed.
        void OnMouseButtonDown(EventContext eventContext, int button);
        
        /// Called by applications when a mouse button is released.
        ///
        /// @param button [in] The index of the button that was released.
        void OnMouseButtonUp(EventContext eventContext, int button);
        
        /// Called by applications when a mouse button is double-clicked.
        ///
        /// @param button [in] The index of the button that was double-clicked.
        void OnMouseButtonDoubleClick(EventContext eventContext, int button);
        
        /// Called by applications when a key is pressed initially pressed.
        ///
        /// @param key [in] The key that was pressed.
        void OnKeyPressed(EventContext eventContext, GT::Key key);
        
        /// Called by applications when a key is down, taking auto-repeat into account.
        ///
        /// @param key [in] The key that was pressed.
        void OnKeyDown(EventContext eventContext, GT::Key key);
        
        /// Called when a key is raised.
        ///
        /// @param key [in] The key that was raised.
        void OnKeyUp(EventContext eventContext, GT::Key key);




        /// Called by the layout manager when an element has changed size.
        void OnSize(Element* element);

        /// Called when an element has moved.
        void OnMove(Element* element);
        
        /// Called by GTGUI when an element is shown.
        void OnShow(Element* element);
        void OnShow(Element &element) { this->OnShow(&element); }
        
        /// Called by GTGUI when an element is hidden.
        void OnHide(Element* element);
        void OnHide(Element &element) { this->OnHide(&element); }



        /// Adds an event to the event queue.
        ///
        /// @param e [in] A reference to the event to add to the end of the event queue.
        void QueueEvent(Event &e);


        /// Gives the given element focus.
        void FocusElement(Element *element);

        /// Causes the element that currently has focus to lose it's focus.
        void BlurFocusedElement();

        /// Retrieves a pointer to the element that has the keyboard focus. Returns null if no element has the keyboard focus.
        Element* GetFocusedElement();
        
        /// Determines whether or not the element can receive focus from a mouse down event.
        ///
        /// @param element [in] A reference to the element to check.
        bool CanElementReceiveFocusFromMouse(const Element &element) const;
        bool CanElementReceiveFocusFromMouse(const Element* element) const { if (element != nullptr) return this->CanElementReceiveFocusFromMouse(*element); return false; }


        /// Brings the given element to the top.
        ///
        /// @param element [in] A reference to the element to bring to the top.
        ///
        /// @remarks
        ///     This will only work on absolute and relative positioned elements.
        void BringToTop(Element &element);

        /// Pushes the given element to the back.
        ///
        /// @param element [in] A reference to the element to push to the back.
        ///
        /// @remarks
        ///     This will only work on absolute and relative positioned elements.
        void PushToBack(Element &element);


        /**
        *   \brief  Positions the caret based on an input click position.
        *
        *   \remarks
        *       The click position should be in relative coordinates to the caret's current owner.
        */
        void PositionCaret(int clickPosX, int clickPosY, bool extendSelection = true, bool autoScroll = true);
        void PositionCaret(bool extendSelection, bool autoScroll = true);

        /// Scrolls the focused element to ensure the text cursor is visible.
        ///
        /// @remarks
        ///     This is how scrolling-while-typing works.
        void ScrollTextCursorIntoVisibility();

        /// Handles the text selection based on the current input and cursor position.
        void HandleTextSelection(bool extendSelection);


        /**
        *   \brief                              Retrieves the top-most element that is under the given point.
        *   \param  x                      [in] The x coordinate of the point.
        *   \param  y                      [in] The y coordinate of the point.
        *   \param  fromMouseInput         [in] Whether or not the function is being called as a result of mouse input.
        *   \param  ignoreDragAndDropProxy [in] Whether or not to ignore the elements that are being followed by the mouse.
        *   \return                             A pointer to the top-most element under the given point. Can return null if it is not under any element.
        *
        *   \remarks
        *       The <fromMouseInput> argument is required in order to honour the 'transparent-mouse-input' style attribute.
        */
        Element* GetElementUnderPoint(int x, int y, bool fromMouseInput = false, bool ignoreDragAndDropProxy = false);



        /// Retrieves a reference to the caret.
              Caret & GetCaret()       { return this->caret; }
        const Caret & GetCaret() const { return this->caret; }

        /**
        *   \brief  Handles every event currently in the event queue, and empties it.
        */
        //void HandleEvents();


        /// Retrieves a reference to the server's garbage collector.
        GarbageCollector & GetGarbageCollector() { return this->garbageCollector; }


        /// Invalidates the mouse. This will cause the next call to Step() to validate the mouse, posting the appropriate events.
        ///
        /// @remarks
        ///     Call this function if the mouse needs to be refreshed due to some event such as the removal of an element.
        void InvalidateMouse();


        /// Registers an image with an identifier.
        ///
        /// @param id     [in] The image identifier for use in styles.
        /// @param width  [in] The width of the image.
        /// @param height [in] The height of the image.
        /// @param bpp    [in] The bytes-per-pixel. 3 = RGB, 4 = RGBA.
        /// @param data   [in] The image data.
        ///
        /// @remarks
        ///     The image can be accessed via styles by using the given ID.
        ///     @par
        ///     If an image with the same ID already exists, it will be replaced. This will make loading themes a bit easier as it
        ///     will allow easier overriding.
        void RegisterImage(const char* id, unsigned int width, unsigned int height, unsigned int bpp, const void* data);

        /// Retrieves a registered image based on it's ID.
        ///
        /// @param id [in] The ID of the of image to retrieve, as previously specified by RegisterImage().
        ///
        /// @remarks
        ///     Images can be dynamically changed, so it is best not to store the returned pointers.
        ImageHandle GetRegisteredImage(const char* id);



        /// Posts an error.
        ///
        /// @param message [in] The error message.
        void PostError(const char* message);

        /// Posts a warning.
        ///
        /// @param message [in] The warning message.
        void PostWarning(const char* message);

        /// Posts a log message.
        ///
        /// @param message [in] The log message.
        void PostLogMessage(const char* message);


        /// Sets the error message level.
        ///
        /// @param newErrorLevel [in] The level of error message detail - higher means more detail.
        ///
        /// @remarks
        ///     Values can be:
        ///       0 - Minimal
        ///       1 - Medium (default)
        ///       2 - High
        void SetErrorMessageLevel(int newErrorLevel) { this->errorLevel = newErrorLevel; }
        
        /// Retrieves the error message level.
        int GetErrorMessageLevel() const { return this->errorLevel; }


        /// Determines whether or not the CTRL key is pressed.
        bool IsCTRLKeyDown() const { return this->isCTRLKeyDown; }

        /// Determines whether or not the Shift key is pressed.
        bool IsShiftKeyDown() const { return this->isShiftKeyDown; }



        /// Sets the drag-and-drop proxy element.
        ///
        /// @param newDragAndDropProxyElement [in] A reference to the element that is the new drag-and-drop proxy.
        ///
        /// @remarks
        ///     If another element is already acting as the proxy, it will be removed as if RemoveCurrentDragAndDropProxyElement() was called.
        void SetDragAndDropProxyElement(Element &newDragAndDropProxyElement);

        /// Retrieves a pointer to the drag-and-drop proxy element.
        Element* GetDragAndDropProxyElement();

        /// Removes the current drag-and-drop proxy element.
        ///
        /// @param postRemoveEvent [in] Whether or not the OnDragAndDropProxyRemoved() event should be called.
        void RemoveCurrentDragAndDropProxyElement(bool postRemoveEvent = true);

        /// Sets the offset of the drag-and-drop proxy element.
        ///
        /// @param offsetX [in] The x offset.
        /// @param offsetY [in] The y offset.
        void SetDragAndDropProxyElementOffset(int offsetX, int offsetY);



        /// Retrieves the current mouse position.
        void GetMousePosition(int &xOut, int &yOut) const;

        /// Sets the mouse position without posting a mouse-move event.
        ///
        /// @param x [in] The new x position of the mouse.
        /// @param y [in] The new y position of the mouse.
        void SetMousePosition(int x, int y);


        /// Marks the mouse as captured.
        void MarkMouseAsCaptured();

        /// Marks the mouse an released.
        void UnmarkMouseAsCaptured();

        /// Determines whether or not the mouse is marked as captured.
        bool IsMouseCaptured() const;


        /// Determines whether or not the left mouse button is down.
        bool IsLMBDown() const;

        /// Determines whether or not the right mouse button is down.
        bool IsRMBDown() const;

        /// Determines whether or not the middle mouse button is down.
        bool IsMMBDown() const;



    // The methods below are called in response to an event.
    public:

        /**
        *   \brief  Called when the z-index of the given element has changed.
        */
        void OnZIndexChanged(Element &element);

        /**
        *   \brief  Called when the width of the given element has changed.
        */
        void OnWidthChanged(Element &element);

        /**
        *   \brief  Called when the height of the given element has changed.
        */
        void OnHeightChanged(Element &element);

        /**
        *   \brief               Called when the padding of the given element has changed.
        *   \param  element [in] A pointer to the element whose padding has changed.
        *
        *   \remarks
        *       Padding includes borders.
        */
        void OnPaddingXChanged(Element &element);
        void OnPaddingYChanged(Element &element);

        /**
        *   \brief               Called when the margins of the given element has changed.
        *   \param  element [in] A pointer to the element whose margin has changed.
        */
        void OnMarginsXChanged(Element &element);
        void OnMarginsYChanged(Element &element);

        /**
        *   \brief               Called when the visibility of the given element has changed.
        *   \param  element [in] A reference to the element whose visibility has just changed
        */
        void OnVisibilityChanged(Element &element);

        /**
        *   \brief  Called when the child plane of the given element has changed.
        */
        void OnChildPlaneChanged(Element &element);

        /**
        *   \brief  Called when the horizontal align of the given element has changed.
        */
        void OnHorizontalAlignChanged(Element &element);

        /**
        *   \brief  Called when the vertical align of the given element has changed.
        */
        void OnVerticalAlignChanged(Element &element);

        /**
        *   \brief  Called when the 'positioning' style has changed. This does not refer to the X,Y positioning of the element.
        */
        void OnPositioningChanged(Element &element);

        /**
        *   \brief  Called when the 'left' style has changed.
        */
        void OnLeftChanged(Element &element);

        /**
        *   \brief  Called when the 'right' style has changed.
        */
        void OnRightChanged(Element &element);

        /**
        *   \brief  Called when the 'top' style has changed.
        */
        void OnTopChanged(Element &element);

        /**
        *   \brief  Called when the 'bottom' style has changed.
        */
        void OnBottomChanged(Element &element);

        /// Called when the 'inner-offset-x' style attribute has changed.
        void OnInnerOffsetXChanged(Element &element);

        /// Called when the 'inner-offset-y' style attribute has changed.
        void OnInnerOffsetYChanged(Element &element);

        
        /// Called when the text of an element changes.
        ///
        /// @remarks
        ///     This will not modify the position of the text cursor.
        void OnTextChanged(Element &element, bool dontPostEvent = false);


        /// Called when a child is attached to the given element.
        void OnChildAttached(Element &element);

        /// Called when a child is detached from the given element.
        void OnChildDetached(Element &element);


        /// Called when the flex-child-width style has changed.
        void OnFlexWidthChanged(Element &element);

        /// Called when the flex-child-height style has changed.
        void OnFlexHeightChanged(Element &element);



    // Methods below are event handlers.
    private:

        void HandleEvent_OnSize(Event &e);
        void HandleEvent_OnMove(Event &e);
        void HandleEvent_OnMouseMove(Event &e);
        void HandleEvent_OnMouseWheel(Event &e);
        void HandleEvent_OnMouseButtonDown(Event &e);
        void HandleEvent_OnMouseButtonUp(Event &e);
        void HandleEvent_OnMouseButtonDoubleClick(Event &e);
        void HandleEvent_OnLMBDown(Event &e);
        void HandleEvent_OnLMBUp(Event &e);
        void HandleEvent_OnKeyPressed(Event &e);
        void HandleEvent_OnKeyDown(Event &e);
        void HandleEvent_OnKeyUp(Event &e);


        /// Marks the beginning of event handling.
        ///
        /// @remarks
        ///     This should be paired with a called to EndEventHandling(). Every time an event is handled, it should be wrapped in a BeginEventHandling/EndEventHandling pair.
        ///     @par
        ///     The increments a counter, whereas EndEventHandling() decrements it. When the counter is at 0, it means events are not currently being handled.
        ///
        /// @see
        ///     GTGUI::Server::EndEventHandling(), GTGUI::Server::IsHandlingEvent()
        void BeginEventHandling();

        /// Marks the end of event handling.
        ///
        /// @remarks
        ///     This should be paired with a call to BeginEventHandling().
        ///
        /// @see
        ///     GTGUI::Server::BeginEventHandling(), GTGUI::Server::IsHandlingEvent()
        void EndEventHandling();

        /// Determines whether or not an event is being handled.
        bool IsHandlingEvent() const;


    private:

        /// Performs the rendering operations of the GUI.
        void Render();


        /**
        *   \brief  Loads any defaults - things like the _Root element.
        */
        bool LoadDefaults();

        /// Loads the standard library.
        bool LoadStandardLibrary();
    
        /**
        *   \brief                 Creates and adds a new element to the server.
        *   \param  elementID [in] The ID of the new element.
        *
        *   \remarks
        *       The ID must be a valid string. If an element with the same ID already exists, null is returned.
        *       \par
        *       The element is NOT added to the script in this function. It only adds the new C++ Element object to the internal
        *       container.
        */
        Element* CreateNewElement(const char* elementID);
        
        

        /// Performs the garbage collection.
        void CollectGarbage();


        /// Validates the mouse. This will be called by Step() AFTER events have been posted.
        ///
        /// This function will use this->mousePosX and this->mousePosY for the position of the mouse.
        void ValidateMouse();


        /// Recursively dds an element to end the internal z-index list.
        ///
        /// @param element [in] The element to add to the end of the z-index list.
        ///
        /// @remarks
        ///     This method will also append any appropraite children to the end of the z-index list. This needs to be done
        ///     to ensure decendants using the same z-index are always shown above.
        void AddToZIndexList(Element &element);

        /// Removes an element from the internal z-index list.
        void RemoveFromZIndexList(Element &element);

        /// Retrieves a pointer to the list of elements for the given z-index.
        GT::List<Element*>* GetElementListByZIndex(int zIndex);

        /// Retrieves a pointer to the list of elements that the given element is currently in.
        GT::List<Element*>* FindListContainingElement(Element &element);

        /// Helper function for finding the element under the given point. This will run through each element recursively.
        ///
        /// @remarks
        ///     This does not check relative or absolute elements, since that will be done during a different stage.
        void GetElementUnderPoint(int x, int y, bool fromMouseInput, Element &base, const GT::Rect<int> &parentRect, const GT::Rect<int> &parentScissorRect, Element * &result);


        /// Sets the cursor.
        ///
        /// @param newCursor [in] The new cursor to use.
        ///
        /// @remarks
        ///     This method will not do anything if the cursor is the same as the current one.
        void SetCursor(GT::Cursor newCursor);

    
        /// Releases the pushed element, if any.
        void ReleasePushedElement(bool blockEvent = false);

        /// Posts and clears element visibility events.
        void PostVisibilityEvents();

        /// Marks the given element as needing the OnShow event posted.
        void MarkElementAsNeedingOnShow(Element &element);

        /// Marks the given element as needing the OnHide event posted.
        void MarkElementAsNeedingOnHide(Element &element);


        /// Posts the OnShowTooltip events.
        ///
        /// @param deltaTimeInSeconds [in] The time since the last step.
        ///
        /// @remarks
        ///     Tooltip events are timer based, so we need to pass the delta time.
        void PostShowTooltipEvents(double deltaTimeInSeconds);


    private:
        
        /// The operation mode. Defaults to delayed/double-buffered. This is changed with SetOperationMode().
        int operationMode;
    
        /// A pointer to the event handler. This will never be null because it will be initialised to the default event handler.
        ServerEventHandler* eventHandler;
    
        /// The script server.
        ScriptServer scripting;
        
        /// The style server.
        StyleServer styling;
        
        
        /// The markup loader for creating elements from markup scripts.
        MarkupLoader markupLoader;


        /// A pointer to the image manager. This can be null, in which case images and fonts/text will not be supported.
        ImageManager* m_imageManager;

        /// The glyph map manager for use by the font server.
        FontGlyphMapManager glyphMapManager;



        /// A pointer to the renderer. This can be null, so we use a pointer instead of a reference. If this is null, rendering commands
        /// will simply not be posted.
        Renderer* m_renderer;
        
        
        /// A binary search tree containing all of the loaded elements. This is index by the element's ID.
        ElementTree elements;

        /// The list of absolute positioned elements. These are also in the main 'elements' list.
        GT::List<Element*> absoluteElements;


        /// We need to keep track of all the elements that use the z-index property. We use a map whose key is
        /// the z-index, and value is a pointer to a list of all the elements that are using that z-index. The
        /// only elements that will use the z-index are relative and absolute elements.
        GT::Map<int, GT::List<Element*>*> elementsUsingZIndex;


        /// The layout manager.
        LayoutManager layoutManager;


        /// The timer that will keep track of the time between each step. This is required for the parameterless version of Step(), which
        /// will use the time since the last update for it's delta.
        GT::Timer stepTimer;

        /// The event queue. Access to this queue is thread-safe. Use PostEvent() to add an event to the queue. This will be emptied at
        /// the beginning of each call to Step().
        EventQueue eventQueue;

        /// The lock we'll use to keep access to 'eventQueue' thread-safe.
        GT::Mutex eventLock;


        unsigned int viewportWidth;     //< The width of the working area.
        unsigned int viewportHeight;    //< The height of the working area.

        /// The font server.
        GT::FontServer fontServer;
        
        /// The font cache.
        FontCache fontCache;


        /// The list of elements that have the mouse over them.
        GT::List<Element*> hoveredElements;


        /// Structure containing proeprties for tooltips.
        struct ElementTooltipProperties
        {
            ElementTooltipProperties()
                : hoverTime(0.0f), hasShowEventBeenPosted(false)
            {
            }

            /// The amount of time the mouse has hovered over the element.
            float hoverTime;

            /// Whether or not the OnShowTooltip event has been posted. This is used in determining whether or not a corresponding
            /// OnHideTooltip event should be posted.
            bool hasShowEventBeenPosted;
        };

        /// A map of hover times for elements that are currently hovered.
        GT::Map<Element*, ElementTooltipProperties> elementTooltipProperties;

        /// The tooltip delay.
        float tooltipDelay;


        /// The element that is being pushed. Only ever one of these at a time.
        Element* pushedElement;
        int pushedElementMousePosX;         // Relative to <pushedElement>
        int pushedElementMousePosY;         // Relative to <pushedElement>

        /// The element that currently has the keyboard focus (and owns the caret).
        Element* focusedElement;
        
        /// Whether or not we are dragging the pushed element.
        bool draggingPushedElement;
        int draggedElementClickPosX;        // Relative to the left of <pushedElement>
        int draggedElementClickPosY;        // Relative to the top of <pushedElement>

        /// The element that is currently being resized with the mouse.
        bool resizingPushedElement;
        bool overLeftSizingGripper;
        bool overRightSizingGripper;
        bool overTopSizingGripper;
        bool overBottomSizingGripper;
        bool resizingUsingLeftGripper;
        bool resizingUsingRightGripper;
        bool resizingUsingTopGripper;
        bool resizingUsingBottomGripper;
        int  gripperClickPosX;
        int  gripperClickPosY;



        /// The garbage collector.
        GarbageCollector garbageCollector;

        /// The caret for this server. There is only ever one of these, and it will be shared across elements.
        mutable Caret caret;

        /// Keeps track of whether or not the server is in the middle of stepping. We use this in determining whether or not elements should be deleted immediately
        /// or via the garbage collector. If the server is in the middle of stepping we don't want to deallocate immediately because the element may be used later
        /// on in the step.
        bool isStepping;

        /// Keeps track of a counter which determines whether or not events are being handled. When the counter is > 0, it means events are being handled. This is
        /// incremented by BeginEventHandling() and decremented by EndEventHandling(). Defaults to 0.
        unsigned int eventHandlingCounter;

        /// Keeps track of whether or not the mouse is valid.
        bool isMouseValid;

        /// Keeps track of whether or not the mouse is marked as captured. When the mouse is captured, mouse events still happen, but only on a global level. The only way
        /// to intercept mouse events while it is captured is by handling the Watch* events.
        bool isMouseCaptured;

        /// Keeps track of whether or not the left mouse button is down.
        bool isLeftMouseButtonDown;

        /// Keeps track of whether or not the right mouse button is down.
        bool isRightMouseButtonDown;

        /// Keeps track of whether or not the middle mouse button is down.
        bool isMiddleMouseButtonDown;

        /// Keeps track of whether or not the mouse has moved. We use this in determining whether or not to post mouse-move events
        /// to elements when the mouse is validated.
        bool mouseMoved;

        /// The position of the mouse from the last mouse move.
        int mousePosX;
        int mousePosY;

        /// The position of the mouse on the last click.
        int lastMouseClickPosX;
        int lastMouseClickPosY;

        /// The current cursor.
        GT::Cursor currentCursor;


        /// The list of images currently loaded by the server. These are keyed by the file name.
        GT::Dictionary<ImageHandle> loadedImages;

        /// The map of registered images. The key is the ID of the image that is used by styles.
        GT::Dictionary<ImageHandle> registeredImages;


        /// An integer representing the detail of the error messages.
        int errorLevel;


        /// Keeps track of whether or not a CTRL key is down. We need to do this so that we don't post key pressed to text boxes while CTRL is held down. If we
        /// don't do this, the text box will, for example, print 'S' if the user was to to CTRL+S to save something.
        bool isCTRLKeyDown;

        /// Keeps track of whether or not a Shift key is down. We need to do this so that we can handle text box selections correctly.
        bool isShiftKeyDown;

        /// Keeps track of whether or not the OnTear() event has been called on the currently pushed element.
        bool onTearEventPosted;

        /// Keeps track of whether or not the mouse is selecting text.
        bool isMouseSelectingText;


        /// A pointer to the element acting as the drag-and-drop proxy.
        Element* dragAndDropProxyElement;

        /// The offset to apply to the drag-and-drop proxy element when positioning it against the mouse.
        GT::Point<int> dragAndDropProxyElementOffset;


        /// The list of elements that need to have OnShow events posted.
        GT::SortedVector<Element*> elementsNeedingOnShow;

        /// The list of elements that need to have OnHide events posted.
        GT::SortedVector<Element*> elementsNeedingOnHide;
        
        
        /// A counter for creating unique ID's for anonymous elements.
        int autoElementCounter;

    
    private:    // No copying.
        Server(const Server &);
        Server & operator=(const Server &);
        

    // The script and style servers are closely related, so we will make them friendly.
    friend class ScriptServer;
    };
}

#endif
