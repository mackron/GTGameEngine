// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_GameUpdateJob_hpp_
#define __GTEngine_GameUpdateJob_hpp_

#include <GTEngine/Core/Threading/Job.hpp>

namespace GTEngine
{
    class Game;

    class GameUpdateJob : public GTLib::Threading::Job
    {
    public:

        /**
        *   \brief  Constructor.
        */
        GameUpdateJob(Game &game);

        /**
        *   \brief  Destructor.
        */
        ~GameUpdateJob();

        /// GTLib::Threading::Job::Run().
        void Run();


    private:

        /// The game this update job is for.
        Game &game;
    };
}

#endif