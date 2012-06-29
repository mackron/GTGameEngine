
#ifndef __GTEngine_Random_FloorLobby_hpp_
#define __GTEngine_Random_FloorLobby_hpp_

namespace GTEngine
{
    class FloorLobby
    {
    public:

        /// Constructor.
        FloorLobby(int left, int top, int right, int bottom);

        /// Destructor.
        ~FloorLobby();


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