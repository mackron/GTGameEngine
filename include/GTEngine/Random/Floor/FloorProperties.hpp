// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Random_FloorProperties_hpp_
#define __GTEngine_Random_FloorProperties_hpp_

namespace GTEngine
{
    /// Enum for the layout of the floors.
    enum FloorLayout
    {
        FloorLayout_Random = 0,
    };


    /// Structure containing properties of a level for generation.
    struct FloorProperties
    {
        /// Constructor.
        FloorProperties()
            : width(0), height(0),
              layout(FloorLayout_Random),
              minRoomWidth(1), maxRoomWidth(4), minRoomHeight(1), maxRoomHeight(4),
              sideRoomCount(2), minSideRoomWidth(1), maxSideRoomWidth(1), minSideRoomHeight(1), maxSideRoomHeight(1),
              minLobbyWidth(3), maxLobbyWidth(5), minLobbyHeight(3), maxLobbyHeight(5)
        {
        }
        

        /// The width of the floor.
        unsigned int width;

        /// The height/depth of the floor.
        unsigned int height;

        /// The layout type of the floor.
        FloorLayout layout;


        // The min/max size of rooms.
        unsigned int minRoomWidth;      ///< The minimum room width.
        unsigned int maxRoomWidth;      ///< The maximum room width.
        unsigned int minRoomHeight;     ///< The minimum room height.
        unsigned int maxRoomHeight;     ///< The maximum room height.


        /// The number of tiles to place on the sides of walls. Good for things like entry/exit points of a level. The generator will
        /// ensure that these points are not covered with rooms.
        unsigned int sideRoomCount;

        // The min/max size of the side areas.
        unsigned int minSideRoomWidth;
        unsigned int maxSideRoomWidth;
        unsigned int minSideRoomHeight;
        unsigned int maxSideRoomHeight;


        // The min/max size of lobbies.
        unsigned int minLobbyWidth;
        unsigned int maxLobbyWidth;
        unsigned int minLobbyHeight;
        unsigned int maxLobbyHeight;
    };
}

#endif
