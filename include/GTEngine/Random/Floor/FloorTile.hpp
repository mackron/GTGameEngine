
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


        /// Sets the flags for the left wall.
        ///
        /// @param newFlags [in] The new flags for the wall.
        void SetLeftWallFlags(FloorTileWallFlag newFlags) { this->leftWallFlags = newFlags; }

        /// Sets the flags for the top wall.
        ///
        /// @param newFlags [in] The new flags for the wall.
        void SetTopWallFlags(FloorTileWallFlag newFlags) { this->topWallFlags = newFlags; }

        /// Sets the flags for the right wall.
        ///
        /// @param newFlags [in] The new flags for the wall.
        void SetRightWallFlags(FloorTileWallFlag newFlags) { this->rightWallFlags = newFlags; }

        /// Sets the flags for the bottom wall.
        ///
        /// @param newFlags [in] The new flags for the wall.
        void SetBottomWallFlags(FloorTileWallFlag newFlags) { this->bottomWallFlags = newFlags; }


        /// Retrieves the flags for the left wall.
        FloorTileWallFlag GetLeftWallFlags() const { return this->leftWallFlags; }

        /// Retrieves the flags for the left wall.
        FloorTileWallFlag GetTopWallFlags() const { return this->topWallFlags; }

        /// Retrieves the flags for the left wall.
        FloorTileWallFlag GetRightWallFlags() const { return this->rightWallFlags; }

        /// Retrieves the flags for the left wall.
        FloorTileWallFlag GetBottomWallFlags() const { return this->bottomWallFlags; }



    private:

        /// The flags of this tile.
        FloorTileFlag flags;

        int xPos;       ///< The x position of the tile.
        int yPos;       ///< The y position of the tile.

        FloorTileWallFlag leftWallFlags;
        FloorTileWallFlag topWallFlags;
        FloorTileWallFlag rightWallFlags;
        FloorTileWallFlag bottomWallFlags;
    };
}

#endif