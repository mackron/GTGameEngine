// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Random_FloorRoom_hpp_
#define __GTEngine_Random_FloorRoom_hpp_

namespace GTEngine
{
    /// Class representing a floor room.
    class FloorRoom
    {
    public:

        /// Constructor.
        FloorRoom(int left, int top, int right, int bottom);

        /// Destructor.
        ~FloorRoom();


        /// Retrieves the bounds of the room.
        ///
        /// @param left   [out] A reference to the variable that will receive the left coordinate.
        /// @param top    [out] A reference to the variable that will receive the top coordinate.
        /// @param right  [out] A reference to the variable that will receive the right coordinate.
        /// @param bottom [out] A reference to the variable that will receive the bottom coordinate.
        void GetRect(int &left, int &top, int &right, int &bottom) const;


    private:

        int left;
        int top;
        int right;
        int bottom;
    };
}

#endif
