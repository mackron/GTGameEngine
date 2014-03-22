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
    
    bool GameEventFilter::OnMouseButtonDown(GTLib::MouseButton &, int &, int &)
    {
        return true;
    }
    
    bool GameEventFilter::OnMouseButtonUp(GTLib::MouseButton &, int &, int &)
    {
        return true;
    }
    
    bool GameEventFilter::OnMouseButtonDoubleClick(GTLib::MouseButton &, int &, int &)
    {
        return true;
    }
    
    
    bool GameEventFilter::OnKeyPressed(GTLib::Key &)
    {
        return true;
    }
    
    bool GameEventFilter::OnKeyReleased(GTLib::Key &)
    {
        return true;
    }
    
    bool GameEventFilter::OnKeyDown(GTLib::Key &)
    {
        return true;
    }
    
    bool GameEventFilter::OnKeyUp(GTLib::Key &)
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
