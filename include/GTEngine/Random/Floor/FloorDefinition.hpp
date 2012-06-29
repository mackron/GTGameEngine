
#ifndef __GTEngine_Random_FloorDefinition_hpp_
#define __GTEngine_Random_FloorDefinition_hpp_

#include "FloorTile.hpp"
#include "FloorRoom.hpp"
#include "FloorLobby.hpp"
#include "../../SimpleQuadtree.hpp"

namespace GTEngine
{
    /// Class containing the data of a floor.
    ///
    /// The floors are defined as a series of tiles arranged in a quadtree. The size of each tile is not used in the generation
    /// of floors. Instead, the size of objects such as rooms are defined in tiles.
    class FloorDefinition
    {
    public:

        /// Constructor.
        FloorDefinition(int quadtreeLeft, int quadtreeTop, int quadtreeRight, int quadtreeBottom);

        /// Destructor.
        ~FloorDefinition();


        /// Sets the actual bounds of the level.
        ///
        /// @remarks
        ///     There are two boundaries - the inner and the outer. The innner is for the main area, whereas the outer encompasses the side rooms.
        void SetBounds(int left, int top, int right, int bottm, int outerLeft, int outerTop, int outerRight, int outerBottom);

        int GetLeft()   const { return this->left;   }
        int GetTop()    const { return this->top;    }
        int GetRight()  const { return this->right;  }
        int GetBottom() const { return this->bottom; }

        int GetOuterLeft()   const { return this->outerLeft;   }
        int GetOuterTop()    const { return this->outerTop;    }
        int GetOuterRight()  const { return this->outerRight;  }
        int GetOuterBottom() const { return this->outerBottom; }



        /// Retrieves a reference to the quadtree of this definition.
              SimpleQuadtree<FloorTile> & GetQuadtree()       { return this->quadtree; }
        const SimpleQuadtree<FloorTile> & GetQuadtree() const { return this->quadtree; }


        /// Retrieves the number of rooms, not including side rooms.
        ///
        /// @return The number of rooms.
        size_t GetRoomCount() const { return rooms.count; }

        /// Retrieves a pointer to the room at the given index.
        ///
        /// @param index [in] The index of the room to retrieve.
        ///
        /// @return A pointer to the room at the given index.
        ///
        /// @remarks
        ///     This method will assert the index is valid.
              FloorRoom* GetRoom(size_t index)       { return this->rooms[index]; }
        const FloorRoom* GetRoom(size_t index) const { return this->rooms[index]; }


        /// Retrieves the number of side rooms, not including side rooms.
        ///
        /// @return The number of rooms.
        size_t GetSideRoomCount() const { return rooms.count; }

        /// Retrieves a pointer to the side room at the given index.
        ///
        /// @param index [in] The index of the room to retrieve.
        ///
        /// @return A pointer to the room at the given index.
        ///
        /// @remarks
        ///     This method will assert the index is valid.
              FloorRoom* GetSideRoom(size_t index)       { return this->rooms[index]; }
        const FloorRoom* GetSideRoom(size_t index) const { return this->rooms[index]; }


        /// Builds a list of tiles that make up the floor.
        void GetAllTiles(GTCore::Vector<FloorTile*> &output) const;



        /// Adds a standard room to the definition.
        ///
        /// @param left   [in] The left boundary of the room.
        /// @param top    [in] The top boundary of the room.
        /// @param right  [in] The right boundary of the room.
        /// @param bottom [in] The bottom boundary of the room.
        FloorRoom* AddRoom(int left, int top, int right, int bottom);

        /// Adds a side room to the definition.
        ///
        /// @param left   [in] The left boundary of the room.
        /// @param top    [in] The top boundary of the room.
        /// @param right  [in] The right boundary of the room.
        /// @param bottom [in] The bottom boundary of the room.
        FloorRoom* AddSideRoom(int left, int top, int right, int bottom);


        /// Retrieves a pointer to the tile at the given position.
        ///
        /// @param x [in] The x position of the tile to retrieve.
        /// @param y [in] The y position of the tile to retrieve.
        ///
        /// @return A pointer to the tile at the given position, or null if there is no tile at that position.
              FloorTile* GetTileAt(int x, int y)       { return this->quadtree.QuerySingle(x, y); }
        const FloorTile* GetTileAt(int x, int y) const { return this->quadtree.QuerySingle(x, y); }


    private:

        // There are two rectangles defining the size of the floor. The first is the main rectangle where the main content is located, not including the side rooms.
        // The second rectangle defines the outer rectangle that also encompasses the side rooms.
        int left;
        int top;
        int right;
        int bottom;

        int outerLeft;
        int outerTop;
        int outerRight;
        int outerBottom;

        /// The root quadtree node containing information about each tile. This does not include side rooms, which are attached to the outside edges of the quadtree.
        SimpleQuadtree<FloorTile> quadtree;

        /// The list of rooms.
        GTCore::Vector<FloorRoom*> rooms;

        /// The list of side rooms.
        GTCore::Vector<FloorRoom*> sideRooms;

        /// The lobbies. These are located just outside the side rooms.
        GTCore::Vector<FloorLobby*> lobbies;
    };
}

#endif