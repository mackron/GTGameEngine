
#include <GTEngine/Random/Floor/FloorTile.hpp>

namespace GTEngine
{
    FloorTile::FloorTile(int x, int y)
        : flags(FloorTileFlag_None), xPos(x), yPos(y)
    {
    }

    FloorTile::~FloorTile()
    {
    }
}