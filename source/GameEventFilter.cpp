// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/GameEventFilter.hpp>

namespace GTEngine
{
    GameEventFilter::GameEventFilter()
    {
    }
    
    GameEventFilter::~GameEventFilter()
    {
    }
    
    
    bool GameEventFilter::OnSize(unsigned int &, unsigned int &)
    {
        return true;
    }
    
    bool GameEventFilter::OnMouseMove(int &, int &)
    {
        return true;
    }
    
    bool GameEventFilter::OnMouseWheel(int &, int &, int &)
    {
        return true;
    }
    
    bool GameEventFilter::OnMouseButtonDown(GTCore::MouseButton &, int &, int &)
    {
        return true;
    }
    
    bool GameEventFilter::OnMouseButtonUp(GTCore::MouseButton &, int &, int &)
    {
        return true;
    }
    
    bool GameEventFilter::OnMouseButtonDoubleClick(GTCore::MouseButton &, int &, int &)
    {
        return true;
    }
    
    
    bool GameEventFilter::OnKeyPressed(GTCore::Key &)
    {
        return true;
    }
    
    bool GameEventFilter::OnKeyReleased(GTCore::Key &)
    {
        return true;
    }
    
    bool GameEventFilter::OnKeyDown(GTCore::Key &)
    {
        return true;
    }
    
    bool GameEventFilter::OnKeyUp(GTCore::Key &)
    {
        return true;
    }
    
    
    bool GameEventFilter::OnReceiveFocus()
    {
        return true;
    }
    
    bool GameEventFilter::OnLoseFocus()
    {
        return true;
    }
}
