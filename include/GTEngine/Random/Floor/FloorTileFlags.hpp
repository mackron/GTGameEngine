// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Random_FloorTileFlags_hpp_
#define __GTEngine_Random_FloorTileFlags_hpp_

namespace GTEngine
{
    enum FloorTileFlag
    {
        FloorTileFlag_None = 0,

        FloorTileFlag_Room     = (1 << 0),      ///< A tile that's in a room.
        FloorTileFlag_Corridor = (1 << 1),      ///< A tile that's not in a room.
        FloorTileFlag_SideRoom = (1 << 2),      ///< A tile that's in a side room.
        FloorTileFlag_Lobby    = (1 << 3),      ///< A tile that's in a lobby area.
    };

    enum FloorTileWallFlag
    {
        FloorTileWallFlag_None = 0,

        FloorTileWallFlag_Default = (1 << 0),
        FloorTileWallFlag_Door    = (1 << 1),
    };
}

#endif