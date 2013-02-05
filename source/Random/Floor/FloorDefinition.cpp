// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Random/Floor/FloorDefinition.hpp>

namespace GTEngine
{
    FloorDefinition::FloorDefinition(int quadtreeLeft, int quadtreeTop, int quadtreeRight, int quadtreeBottom)
        : left(0), top(0), right(0), bottom(0),
          outerLeft(0), outerTop(0), outerRight(0), outerBottom(0),
          quadtree(quadtreeLeft, quadtreeTop, quadtreeRight, quadtreeBottom),
          rooms(), sideRooms(), lobbies()
    {
    }

    FloorDefinition::~FloorDefinition()
    {
        // We will delete the rooms before the tiles. This is not strictly required, but it makes more sense.
        for (size_t i = 0; i < this->rooms.count; ++i)
        {
            delete this->rooms[i];
        }

        for (size_t i = 0; i < this->sideRooms.count; ++i)
        {
            delete this->sideRooms[i];
        }


        // Now we delete the tiles. To do this, we query the tiles from the quadtree.
        GTCore::Vector<FloorTile*> tiles;
        this->quadtree.QueryAll(tiles);

        for (size_t i = 0; i < tiles.count; ++i)
        {
            delete tiles[i];
        }
    }

    void FloorDefinition::SetBounds(int left, int top, int right, int bottom, int outerLeft, int outerTop, int outerRight, int outerBottom)
    {
        this->left   = left;
        this->top    = top;
        this->right  = right;
        this->bottom = bottom;

        this->outerLeft   = outerLeft;
        this->outerTop    = outerTop;
        this->outerRight  = outerRight;
        this->outerBottom = outerBottom;
    }

    void FloorDefinition::GetAllTiles(GTCore::Vector<FloorTile*> &output) const
    {
        this->quadtree.QueryAll(output);
    }


    FloorRoom* FloorDefinition::AddRoom(int left, int top, int right, int bottom)
    {
        auto room = new FloorRoom(left, top, right, bottom);
        this->rooms.PushBack(room);

        return room;
    }

    FloorRoom* FloorDefinition::AddSideRoom(int left, int top, int right, int bottom)
    {
        auto room = new FloorRoom(left, top, right, bottom);
        this->sideRooms.PushBack(room);

        return room;
    }
}
