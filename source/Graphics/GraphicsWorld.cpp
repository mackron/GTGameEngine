// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGE/Graphics/GraphicsWorld.hpp>

//#include <cstdio>

namespace GT
{
    GraphicsWorld::GraphicsWorld(drgui_context* pGUI)
        : m_pGUI(pGUI)
    {
    }

    GraphicsWorld::~GraphicsWorld()
    {
    }


    drgui_context* GraphicsWorld::GetGUI() const
    {
        return m_pGUI;
    }
}
