// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GameEventFilter.hpp>

namespace GT
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
    
    bool GameEventFilter::OnMouseButtonDown(MouseButton &, int &, int &)
    {
        return true;
    }
    
    bool GameEventFilter::OnMouseButtonUp(MouseButton &, int &, int &)
    {
        return true;
    }
    
    bool GameEventFilter::OnMouseButtonDoubleClick(MouseButton &, int &, int &)
    {
        return true;
    }
    
    
    bool GameEventFilter::OnKeyPressed(Key &)
    {
        return true;
    }
    
    bool GameEventFilter::OnKeyReleased(Key &)
    {
        return true;
    }
    
    bool GameEventFilter::OnKeyDown(Key &)
    {
        return true;
    }
    
    bool GameEventFilter::OnKeyUp(Key &)
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
