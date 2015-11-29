// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SceneEditorGameEventFilter
#define GT_SceneEditorGameEventFilter

#include "../../GameEventFilter.hpp"
#include <GTEngine/GUI/Element.hpp>

namespace GT
{
    class SceneEditorGameEventFilter : public GameEventFilter
    {
    public:
    
        /// Constructor.
        SceneEditorGameEventFilter();
        
        /// Destructor.
        ~SceneEditorGameEventFilter();
        
        
        /// Sets the viewport element that mouse coordinates should be relative to.
        ///
        /// @param viewport [in] A pointer to the viewport element.
        void SetViewportElement(GTGUI::Element* viewport);
        
        
        
        /////////////////////////////////////////
        // Virtual implementations.
        
        /// GameEventFilter::OnMouseMove().
        bool OnMouseMove(int &mousePosX, int &mousePosY);
        
        /// GameEventFilter::OnMouseWheel().
        bool OnMouseWheel(int &delta, int &mousePosX, int &mousePosY);

        /// GameEventFilter::OnMouseButtonDown().
        bool OnMouseButtonDown(MouseButton &button, int &mousePosX, int &mousePosY);

        /// GameEventFilter::OnMouseButtonUp().
        bool OnMouseButtonUp(MouseButton &button, int &mousePosX, int &mousePosY);

        /// GameEventFilter::OnMouseButtonDoubleClick().
        bool OnMouseButtonDoubleClick(MouseButton &button, int &mousePosX, int &mousePosY);
        
        
    private:
        
        /// Normalizes the given mouse coordinates.
        ///
        /// @param mousePosX [in, out] The X coordinate to normalize.
        /// @param mousePosY [in, out] The Y coordiante to normalize.
        ///
        /// @remarks
        ///     This is normalized based on the set viewport. If the viewport is not set (null), then the coordinates will be left unmodified.
        void NormalizeMouseCoordinates(int &mousePosX, int &mousePosY) const;
        
    
    private:
        
        /// A pointer to the viewport element mouse coordintes should be relative to. If this is null, anything depending on the viewport
        /// will be unfiltered. Defaults to null.
        GTGUI::Element* m_viewport;
    
    
    private:    // No copying.
        SceneEditorGameEventFilter(const SceneEditorGameEventFilter &);
        SceneEditorGameEventFilter & operator=(const SceneEditorGameEventFilter &);
    };
}

#endif
