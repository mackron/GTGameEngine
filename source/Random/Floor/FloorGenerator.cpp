
#include <GTEngine/Random/Floor/FloorGenerator.hpp>

namespace GTEngine
{
    FloorGenerator::FloorGenerator()
        : random()
    {
    }

    FloorGenerator::~FloorGenerator()
    {
    }

    void FloorGenerator::Seed(int seed)
    {
        this->random.Seed(static_cast<int32_t>(seed));
    }

    FloorDefinition* FloorGenerator::GenerateFloorDefinition(const FloorProperties &properties)
    {
        // Every tiles is located in the quadtree. We need to find the bounds of this quadtree based on the input properties. We
        // need to allow for the side rooms and we also need to ensure the bounds are a power of 2.
        unsigned int totalWidth  = GTCore::NextPowerOf2(properties.width  + (properties.maxRoomWidth  << 1));
        unsigned int totalHeight = GTCore::NextPowerOf2(properties.height + (properties.maxRoomHeight << 1));
        unsigned int halfWidth   = totalWidth  >> 1;
        unsigned int halfHeight  = totalHeight >> 1;

        int quadtreeLeft   = -static_cast<int>(halfWidth);
        int quadtreeTop    = -static_cast<int>(halfHeight);
        int quadtreeRight  =  static_cast<int>(halfWidth);
        int quadtreeBottom =  static_cast<int>(halfHeight);
        auto definition = new FloorDefinition(quadtreeLeft, quadtreeTop, quadtreeRight, quadtreeBottom);

        // Here we need to set the bounds of the level.
        int left   = -static_cast<int>(properties.width  >> 1);
        int top    = -static_cast<int>(properties.height >> 1);
        int right  =  left + properties.width;
        int bottom =  top  + properties.height;
        int outerLeft   = left   - properties.maxRoomWidth;
        int outerTop    = top    - properties.maxRoomHeight;
        int outerRight  = right  + properties.maxRoomWidth;
        int outerBottom = bottom + properties.maxRoomHeight;
        definition->SetBounds(left, top, right, bottom, outerLeft, outerTop, outerRight, outerBottom);


        //
        // Step 1: Here we generate the floor tiles. These all start out as corridor floor tiles.
        //
        this->GenerateFloorTiles(properties, *definition);

        //
        // Step 2: The side rooms.
        //
        this->GenerateSideRooms(properties, *definition);
        
        



        return definition;
    }

    void FloorGenerator::DeleteFloorDefinition(FloorDefinition* definition)
    {
        delete definition;
    }


    //////////////////////////////////////////////////////////
    // Generation Methods.

    void FloorGenerator::GenerateSideRooms(const FloorProperties &properties, FloorDefinition &definition)
    {
        for (auto i = 0U; i < properties.sideRoomCount; ++i)
        {
            int left;
            int top;
            int right;
            int bottom;
            if (this->FindSideRoomLocation(left, top, right, bottom, properties, definition))
            {
                definition.AddSideRoom(left, top, right, bottom);

                // We need to create the tiles for the side room.
                for (int x = left; x < right; ++x)
                {
                    for (int y = top; y < bottom; ++y)
                    {
                        auto tile = this->CreateTile(x, y, definition);
                        assert(tile != nullptr);

                        tile->SetFlags(FloorTileFlag_SideRoom);

                        printf("Testing: %d %d\n", x, y);
                    }
                }
            }
            else
            {
                // If we get here, it means there is no room for a side room.
                return;
            }
        }
    }

    void FloorGenerator::GenerateFloorTiles(const FloorProperties &properties, FloorDefinition &definition)
    {
        (void)properties;

        auto &quadtree = definition.GetQuadtree();

        // We're going to brute force and iterate over each tile.
        for (int x = definition.GetLeft(); x < definition.GetRight(); ++x)
        {
            for (auto y = definition.GetTop(); y < definition.GetBottom(); ++y)
            {
                auto tile = quadtree.QuerySingle(x, y);
                if (tile == nullptr)
                {
                    tile = this->CreateTile(x, y, definition);
                    tile->SetFlags(FloorTileFlag_Corridor);
                }
            }
        }
    }

    FloorTile* FloorGenerator::CreateTile(int x, int y, FloorDefinition &definition)
    {
        auto &quadtree = definition.GetQuadtree();

        auto tile = new FloorTile(x, y);
        quadtree.Insert(tile, x, y);

        return tile;
    }


    bool FloorGenerator::FindSideRoomLocation(int &left, int &top, int &right, int &bottom, const FloorProperties &properties, const FloorDefinition &definition) const
    {
        auto &quadtree = definition.GetQuadtree();

        GTCore::Vector<SimpleQuadtree<FloorTile>::Rect> emptyLeftSpaces;
        GTCore::Vector<SimpleQuadtree<FloorTile>::Rect> emptyTopSpaces;
        GTCore::Vector<SimpleQuadtree<FloorTile>::Rect> emptyRightSpaces;
        GTCore::Vector<SimpleQuadtree<FloorTile>::Rect> emptyBottomSpaces;
        quadtree.ScanEmptySpacesY(definition.GetLeft() - 1, definition.GetTop(),     definition.GetLeft(),      definition.GetBottom(),     properties.minSideRoomHeight, emptyLeftSpaces);
        quadtree.ScanEmptySpacesX(definition.GetLeft(),     definition.GetTop() - 1, definition.GetRight(),     definition.GetTop(),        properties.minSideRoomWidth,  emptyTopSpaces);
        quadtree.ScanEmptySpacesY(definition.GetRight(),    definition.GetTop(),     definition.GetRight() + 1, definition.GetBottom(),     properties.minSideRoomHeight, emptyRightSpaces);
        quadtree.ScanEmptySpacesX(definition.GetLeft(),     definition.GetBottom(),  definition.GetRight(),     definition.GetBottom() + 1, properties.minSideRoomWidth,  emptyBottomSpaces);



        // At this point we will have our lists of empty spaces. What we need to do is build another vector containing pointers to the lists with
        // content. We then randomly index into that list to find the edge to place the side room on.
        GTCore::Vector<GTCore::Vector<SimpleQuadtree<FloorTile>::Rect>*> availableSides;
        if (emptyLeftSpaces.count > 0)
        {
            availableSides.PushBack(&emptyLeftSpaces);
        }
        if (emptyTopSpaces.count > 0)
        {
            availableSides.PushBack(&emptyTopSpaces);
        }
        if (emptyRightSpaces.count > 0)
        {
            availableSides.PushBack(&emptyRightSpaces);
        }
        if (emptyBottomSpaces.count > 0)
        {
            availableSides.PushBack(&emptyBottomSpaces);
        }


        if (availableSides.count > 0)
        {
            auto sideList = availableSides[this->random.Next(0, static_cast<int>(availableSides.count - 1))];
            assert(sideList != nullptr);

            // Now we pick a random space in this list as the result.
            auto &spaceRect = sideList->buffer[this->random.Next(0, static_cast<int>(sideList->count - 1))];
            left   = spaceRect.left;
            top    = spaceRect.top;
            right  = spaceRect.right;
            bottom = spaceRect.bottom;

            // We need to randomise the location inside the chosen space. We do this by applying an offset to the left or top of the space, depending
            // on the axis (horizontal or vertical).
            if (sideList == &emptyLeftSpaces || sideList == &emptyRightSpaces)
            {
                int minOffset = 0;
                int maxOffset = bottom - top - properties.minSideRoomHeight;

                top    += this->random.Next(minOffset, maxOffset);
                bottom  = top + this->random.Next(static_cast<int>(properties.minSideRoomHeight), GTCore::Min(static_cast<int>(properties.maxSideRoomHeight), (bottom - top)));

                // Now we need to randomize the width.
                int width = this->random.Next(static_cast<int>(properties.minSideRoomWidth), static_cast<int>(properties.maxSideRoomWidth));
                if (sideList == &emptyLeftSpaces)
                {
                    left = right - width;
                }
                else
                {
                    right = left + width;
                }
            }
            else if (sideList == &emptyTopSpaces || sideList == &emptyBottomSpaces)
            {
                int minOffset = 0;
                int maxOffset = right - left - properties.minSideRoomWidth;

                left  += this->random.Next(minOffset, maxOffset);
                right  = left + this->random.Next(static_cast<int>(properties.minSideRoomWidth), GTCore::Min(static_cast<int>(properties.maxSideRoomWidth), (right - left)));

                // Now we need to randomize the height.
                int height = this->random.Next(static_cast<int>(properties.minSideRoomHeight), static_cast<int>(properties.maxSideRoomHeight));
                if (sideList == &emptyTopSpaces)
                {
                    top = bottom - height;
                }
                else
                {
                    bottom = top + height;
                }
            }

            return true;
        }
        else
        {
            return false;
        }


        /*
        // What we do here is find empty spaces along each side room edge and check if a minimum-sized side room can fit in any of those spaces.
        GTCore::Vector<FloorTile*> leftTiles;
        GTCore::Vector<FloorTile*> topTiles;
        GTCore::Vector<FloorTile*> rightTiles;
        GTCore::Vector<FloorTile*> bottomTiles;

        quadtree.QueryRange(definition.GetLeft() - 1, definition.GetTop(),     definition.GetLeft(),      definition.GetBottom(),     leftTiles);
        quadtree.QueryRange(definition.GetLeft(),     definition.GetTop() - 1, definition.GetRight(),     definition.GetTop(),        topTiles);
        quadtree.QueryRange(definition.GetRight(),    definition.GetTop(),     definition.GetRight() + 1, definition.GetBottom(),     rightTiles);
        quadtree.QueryRange(definition.GetLeft(),     definition.GetBottom(),  definition.GetRight(),     definition.GetBottom() + 1, bottomTiles);

        // At this point we have all of the occupied tiles along each outside edge. We can now grab empty sections that are at least as big as the
        // minimum size of a side room.
        

        GTCore::Vector<Section> emptyLeftSections;
        */

        //return false;
    }
}
