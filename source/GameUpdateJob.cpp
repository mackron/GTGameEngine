// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GameUpdateJob.hpp>
#include <GTGE/Game.hpp>

namespace GT
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