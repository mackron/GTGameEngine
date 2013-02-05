// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/GameUpdateJob.hpp>
#include <GTEngine/Game.hpp>

namespace GTEngine
{
    GameUpdateJob::GameUpdateJob(Game &game)
        : game(game)
    {
    }

    GameUpdateJob::~GameUpdateJob()
    {
    }

    void GameUpdateJob::Run()
    {
        this->game.Update();
    }
}