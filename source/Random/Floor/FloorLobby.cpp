// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Random/Floor/FloorLobby.hpp>

namespace GTEngine
{
    FloorLobby::FloorLobby(int leftIn, int topIn, int rightIn, int bottomIn)
        : left(leftIn), top(topIn), right(rightIn), bottom(bottomIn)
    {
    }

    FloorLobby::~FloorLobby()
    {
    }

    void FloorLobby::GetRect(int &leftOut, int &topOut, int &rightOut, int &bottomOut) const
    {
        leftOut   = this->left;
        topOut    = this->top;
        rightOut  = this->right;
        bottomOut = this->bottom;
    }
}