
#ifndef __GTEngine_Random_FloorTile_hpp_
#define __GTEngine_Random_FloorTile_hpp_

#include "FloorTileFlags.hpp"

namespace GTEngine
{
    /// Class representing a tile in a floor.
    class FloorTile
    {
    public:

        /// Constructor.
        FloorTile(int x, int y);

        /// Destructor.
        ~FloorTile();


        /// Sets the flags of the tile.
        ///
        /// @param newFlags [in] The new flags of the tile.
        void SetFlags(FloorTileFlag newFlags) { this->flags = newFlags; }

        /// Retrieves the tile flags.
        FloorTileFlag GetFlags() const { return this->flags; }


        /// Retrieves the x position of the tile.
        int GetXPosition() const { return this->xPos; }

        /// Retrieves the y position of the tile.
        int GetYPosition() const { return this->yPos; }



    private:

        /// The flags of this tile.
        FloorTileFlag flags;

        int xPos;       ///< The x position of the tile.
        int yPos;       ///< The y position of the tile.
    };
}

#endif