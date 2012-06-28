
#include <GTEngine/Random/Floor/FloorRoom.hpp>

namespace GTEngine
{
    FloorRoom::FloorRoom(int leftIn, int topIn, int rightIn, int bottomIn)
        : left(leftIn), top(topIn), right(rightIn), bottom(bottomIn)
    {
    }

    FloorRoom::~FloorRoom()
    {
    }

    void FloorRoom::GetRect(int &leftOut, int &topOut, int &rightOut, int &bottomOut) const
    {
        leftOut   = this->left;
        topOut    = this->top;
        rightOut  = this->right;
        bottomOut = this->bottom;
    }
}