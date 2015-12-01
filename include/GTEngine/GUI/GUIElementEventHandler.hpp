// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIElementEventHandler
#define GT_GUIElementEventHandler

#include <GTEngine/Core/Windowing/Keys.hpp>

namespace GT
{
    class GUIElement;

    class GUIElementEventHandler
    {
    public:

        /// Warning silencer...
        virtual ~GUIElementEventHandler();


    public:

        /// Called after the element has been resized.
        virtual void OnSize(GUIElement &element);

        /// Called after the element has moved.
        virtual void OnMove(GUIElement &element);


        /// Called just before the element's rendering commands are executed.
        virtual void OnDraw(GUIElement &element);

        /// A special virtual method that controls whether or not OnDraw should be called.
        ///
        /// @remarks
        ///     If OnDraw should be called, have this method return true. Otherwise, return false. The default implementation returns false. When this
        ///     method returns false, OnDraw will not be called. This is due to performance because most elements won't actually want to call this.
        virtual bool ImplementsOnDraw() const;


        /// Called when a mouse button is pressed on the element.
        virtual void OnMouseButtonDown(GUIElement &element, int button, int x, int y);
        
        /// Called when a mouse button is raised on the element.
        virtual void OnMouseButtonUp(GUIElement &element, int button, int x, int y);
        
        /// Called when the left mouse button is double clicked on the element.
        virtual void OnMouseButtonDoubleClick(GUIElement &element, int button, int x, int y);


        /// Called when the mouse enters the element.
        virtual void OnMouseEnter(GUIElement &element);

        /// Called when the mouse leaves the element.
        virtual void OnMouseLeave(GUIElement &element);

        /// Called when the element is pushed down. Use OnPressed() for handling cases where the element has been pushed, and then released.
        virtual void OnPush(GUIElement &element);

        /// Called when the element is released from a previously pushed state.
        virtual void OnRelease(GUIElement &element);

        /// Called when the element has been pushed, and then released. Use this event for button presses.
        virtual void OnPressed(GUIElement &element);


        /// Called when the mouse is moved while over the given element.
        virtual void OnMouseMove(GUIElement &element, int x, int y);

        /// Called when the mouse wheel is moved while over the given element.
        virtual void OnMouseWheel(GUIElement &element, int delta, int x, int y);


        /// Called when a key is pressed. This is different to OnKeyDown in that it will not be called for repeating keys.
        virtual void OnKeyPressed(GUIElement &element, GT::Key key);

        /// Called when a key is pushed down.
        ///
        /// @remarks
        ///     To cancel the key input, return false. Otherwise, return true. This is a good way to selectively ignore keys.
        virtual bool OnKeyDown(GUIElement &element, GT::Key key);

        /// Called when a key is released.
        virtual void OnKeyUp(GUIElement &element, GT::Key key);


        /// Called when the element's text has changed.
        virtual void OnTextChanged(GUIElement &element);


        /// Called when the element receives keyboard focus.
        virtual void OnFocus(GUIElement &element);

        /// Called when the element loses keyboard focus.
        virtual void OnBlur(GUIElement &element);


        /// Called when the element has been torn for a drag-and-drop operation.
        virtual void OnTear(GUIElement &element);

        /// Called when an element has been dropped onto the element.
        ///
        /// @param droppedElement [in] The element that was dropped onto the element.
        virtual void OnDrop(GUIElement &element, GUIElement &droppedElement);

        /// Called when an element currently being drag-and-dropped enters the element.
        ///
        /// @param dragAndDropElement [in] The element being dragged.
        virtual void OnDragAndDropEnter(GUIElement &element, GUIElement &dragAndDropElement);

        /// Called when an element currently being drag-and-dropped leaves the element.
        ///
        /// @param dragAndDropElement [in] The element being dragged.
        virtual void OnDragAndDropLeave(GUIElement &element, GUIElement &dragAndDropElement);

        /// Called only on a proxy element for when it is removed as the current proxy.
        ///
        /// @param element [in] The proxy element that has just been removed as the current drag-and-drop proxy.
        virtual void OnDragAndDropProxyRemoved(GUIElement &element);


        /// Called when an element goes from an invisible to visible state.
        ///
        /// @param element [in] A reference to the element that was just made visible.
        virtual void OnShow(GUIElement &element);

        /// Called when an element goes from an visible to invisible state.
        ///
        /// @param element [in] A reference to the element that was just made invisible.
        virtual void OnHide(GUIElement &element);


        /// Called when an element should have it's tooltip displayed.
        ///
        /// @param element [in] A reference to the element that should have it's tooltip displayed.
        virtual void OnShowTooltip(GUIElement &element);

        /// Called when an element should have it's tooltip hidden.
        ///
        /// @param element [in] A reference to the element that should have it's tooltip hidden.
        virtual void OnHideTooltip(GUIElement &element);
    };
}

#endif
