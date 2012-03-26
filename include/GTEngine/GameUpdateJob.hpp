
#ifndef __GTEngine_GameUpdateJob_hpp_
#define __GTEngine_GameUpdateJob_hpp_

#include <GTCore/Threading/Job.hpp>

namespace GTEngine
{
    class Game;

    class GameUpdateJob : public GTCore::Threading::Job
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

        /// GTCore::Threading::Job::Run().
        void Run();


    private:

        /// The game this update job is for.
        Game &game;
    };
}

#endif