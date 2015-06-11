// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GameContextEditorEventHandler.hpp>

#if defined(GT_BUILD_EDITOR)
#include <GTGameEngine/GameContext.hpp>

namespace GT
{
    GameContextEditorEventHandler::GameContextEditorEventHandler(GameContext &gameContext)
        : m_gameContext(gameContext)
    {
    }

    GameContextEditorEventHandler::~GameContextEditorEventHandler()
    {
    }


    void GameContextEditorEventHandler::OnEditorOpened()
    {
        m_gameContext.OnEditorOpened();
    }

    void GameContextEditorEventHandler::OnEditorClosed()
    {
        m_gameContext.OnEditorClosed();
    }
}

#endif
