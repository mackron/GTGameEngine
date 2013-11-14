// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditorGameEventFilter.hpp>

namespace GTEngine
{
    SceneEditorGameEventFilter::SceneEditorGameEventFilter()
        : m_viewport(nullptr)
    {
    }
    
    SceneEditorGameEventFilter::~SceneEditorGameEventFilter()
    {
    }
    
    
    void SceneEditorGameEventFilter::SetViewportElement(GTGUI::Element* viewport)
    {
        m_viewport = viewport;
    }
    
    
    /////////////////////////////////////////
    // Virtual implementations.
    
    bool SceneEditorGameEventFilter::OnMouseMove(int &mousePosX, int &mousePosY)
    {
        this->NormalizeMouseCoordinates(mousePosX, mousePosY);
        return true;
    }
    
    bool SceneEditorGameEventFilter::OnMouseWheel(int &, int &mousePosX, int &mousePosY)
    {
        this->NormalizeMouseCoordinates(mousePosX, mousePosY);
        return true;
    }
    
    bool SceneEditorGameEventFilter::OnMouseButtonDown(GTCore::MouseButton &, int &mousePosX, int &mousePosY)
    {
        this->NormalizeMouseCoordinates(mousePosX, mousePosY);
        return true;
    }
    
    bool SceneEditorGameEventFilter::OnMouseButtonUp(GTCore::MouseButton &, int &mousePosX, int &mousePosY)
    {
        this->NormalizeMouseCoordinates(mousePosX, mousePosY);
        return true;
    }
    
    bool SceneEditorGameEventFilter::OnMouseButtonDoubleClick(GTCore::MouseButton &, int &mousePosX, int &mousePosY)
    {
        this->NormalizeMouseCoordinates(mousePosX, mousePosY);
        return true;
    }
    
    
    
    /////////////////////////////////////////
    // Private
    
    void SceneEditorGameEventFilter::NormalizeMouseCoordinates(int &mousePosX, int &mousePosY) const
    {
        if (m_viewport != nullptr)
        {
            int viewportPosX;
            int viewportPosY;
            m_viewport->GetAbsolutePosition(viewportPosX, viewportPosY);
            
            mousePosX -= viewportPosX;
            mousePosY -= viewportPosY;
        }
    }
}
