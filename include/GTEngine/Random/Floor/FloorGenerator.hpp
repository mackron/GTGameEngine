
#ifndef __GTEngine_FloorGenerator_hpp_
#define __GTEngine_FloorGenerator_hpp_

#include "FloorDefinition.hpp"
#include "FloorProperties.hpp"
#include <GTCore/Random.hpp>

namespace GTEngine
{
    /// Class used to generate floor definitions.
    class FloorGenerator
    {
    public:

        /// Constructor.
        ///
        /// This seeds the random generator with a time-based seed.
        FloorGenerator();

        /// Destructor.
        ~FloorGenerator();


        /// Seeds the random number generator.
        ///
        /// @param seed [in] The new seed.
        void Seed(int seed);


        /// Generates a floor definition.
        ///
        /// @param properties [in] A reference to the structure containing the properties of the floor.
        ///
        /// @return A pointer to the new floor definition. Delete this with 'delete' or DeleteFloorDefinition().
        FloorDefinition* GenerateFloorDefinition(const FloorProperties &properties);

        /// Deletes a floor definition that was created with GenerateFloorDefinition().
        ///
        /// @param defintion [in] A pointer to the definition to delete.
        void DeleteFloorDefinition(FloorDefinition* definition);



    private:

        /// Generates the side rooms.
        ///
        /// @remarks
        ///     It is asserted that this is the first generation method to be used.
        void GenerateSideRooms(const FloorProperties &properties, FloorDefinition &definition);


        /// Generates the floor tiles.
        void GenerateFloorTiles(const FloorProperties &properties, FloorDefinition &definition);



        /// Creates a tile at the given position.
        FloorTile* CreateTile(int x, int y, FloorDefinition &definition);


        /// Finds a possible location to place a side room.
        ///
        /// @param left   [out] A reference to the variable that will receive the left boundary of the side room.
        /// @param top    [out] A reference to the variable that will receive the top boundary of the side room.
        /// @param right  [out] A reference to the variable that will receive the right boundary of the side room.
        /// @param bottom [out] A reference to the variable that will receive the bottom boundary of the side room.
        ///
        /// @return True if a side room location was found; false otherwise.
        bool FindSideRoomLocation(int &left, int &top, int &right, int &bottom, const FloorProperties &properties, const FloorDefinition &definition) const;


    private:

        /// The random number generator being used to generate the floors.
        mutable GTCore::Random random;
    };
}

#endif