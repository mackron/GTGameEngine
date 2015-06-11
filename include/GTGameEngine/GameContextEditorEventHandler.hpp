// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GameContextEditorEventHandler_hpp_
#define __GT_GameContextEditorEventHandler_hpp_

#include "Config.hpp"

#if defined(GT_BUILD_EDITOR)
#include "Editor/EditorEventHandler.hpp"

namespace GT
{
    class GameContext;

    class GameContextEditorEventHandler : public EditorEventHandler
    {
    public:

        /// Constructor.
        GameContextEditorEventHandler(GameContext &gameContext);

        /// Destructor.
        ~GameContextEditorEventHandler();


        /// @copydoc EditorEventHandler::OnEditorOpened()
        void OnEditorOpened();

        /// @copydoc EditorEventHandler::OnEditorClosed()
        void OnEditorClosed();



    private:

        /// A reference to the game context.
        GameContext &m_gameContext;



    private:    // No copying.
        GameContextEditorEventHandler(const GameContextEditorEventHandler &);
        GameContextEditorEventHandler & operator=(const GameContextEditorEventHandler &);
    };
}

#endif

#endif
