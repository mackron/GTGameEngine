// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/GameUpdateJob.hpp>
#include <GTEngine/Game.hpp>

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