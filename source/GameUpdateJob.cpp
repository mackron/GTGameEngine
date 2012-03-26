
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