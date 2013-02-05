// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Random/Floor/FloorTile.hpp>

namespace GTEngine
{
    FloorTile::FloorTile(int x, int y)
        : flags(FloorTileFlag_None), xPos(x), yPos(y),
          leftWallFlags(FloorTileWallFlag_None), topWallFlags(FloorTileWallFlag_None), rightWallFlags(FloorTileWallFlag_None), bottomWallFlags(FloorTileWallFlag_None)
    {
    }

    FloorTile::~FloorTile()
    {
    }
}