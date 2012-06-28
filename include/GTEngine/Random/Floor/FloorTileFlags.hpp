
#ifndef __GTEngine_Random_FloorTileFlags_hpp_
#define __GTEngine_Random_FloorTileFlags_hpp_

namespace GTEngine
{
    enum FloorTileFlag
    {
        FloorTileFlag_None = 0x0,

        FloorTileFlag_Room     = (1 << 0),     ///< A tile that's in a room.
        FloorTileFlag_Corridor = (1 << 1),     ///< A tile that's not in a room.
        FloorTileFlag_SideRoom = (1 << 2),     ///< A tile that's in a side room.
    };
}

#endif