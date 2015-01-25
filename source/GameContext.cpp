// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GameContext.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    namespace GE
    {
        GameContext::GameContext(EngineContext &engineContext)
            : m_engineContext(engineContext)
        {
        }

        GameContext::~GameContext()
        {
        }


        ResultCode GameContext::Startup()
        {
            return 0;   // No error.
        }

        void GameContext::Shutdown()
        {
        }
    }
}