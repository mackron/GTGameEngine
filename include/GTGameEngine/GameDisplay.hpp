// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GameDisplay_hpp_
#define __GT_GE_GameDisplay_hpp_

namespace GT
{
    namespace GE
    {
        /// Class representing a game display.
        class GameDisplay
        {
        public:

            /// Constructor.
            GameDisplay();

            /// Destructor.
            virtual ~GameDisplay();



        private:    // No copying.
            GameDisplay(const GameDisplay &);
            GameDisplay & operator=(const GameDisplay &);
        };
    }
}

#endif // !__GT_GE_GameDisplay_hpp_
