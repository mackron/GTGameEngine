
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
        // Step 2: The side rooms. This also creates the lobbies.
        //
        this->GenerateSideRooms(properties, *definition);
        
        //
        // Step 3: Generate the outside walls.
        //
        this->GenerateOutsideWalls(properties, *definition);
        
        //
        // Step 4: Generate rooms.
        //
        this->GenerateRooms(properties, *definition);


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
                    }
                }


                // We need to set the wall properties of the side room. We place walls on all sides except the side facing the
                // main area.
                if (right == definition.GetLeft() || left == definition.GetRight())
                {
                    // Top/Bottom
                    for (int x = left; x < right; ++x)
                    {
                        auto tile = definition.GetTileAt(x, top);           // assert(tile != nullptr)
                        tile->SetTopWallFlags(FloorTileWallFlag_Default);

                        tile = definition.GetTileAt(x, bottom - 1);         // assert(tile != nullptr)
                        tile->SetBottomWallFlags(FloorTileWallFlag_Default);
                    }
                    
                    if (right == definition.GetLeft())
                    {
                        // Walled on left, open on right.
                        for (int y = top; y < bottom; ++y)
                        {
                            auto tile = definition.GetTileAt(left, y);
                            tile->SetLeftWallFlags(FloorTileWallFlag_Default);

                            tile = definition.GetTileAt(right - 1, y);
                            tile->SetRightWallFlags(FloorTileWallFlag_None);
                        }
                    }
                    else
                    {
                        // Walls on right, open on left.
                        for (int y = top; y < bottom; ++y)
                        {
                            auto tile = definition.GetTileAt(left, y);
                            tile->SetLeftWallFlags(FloorTileWallFlag_None);

                            tile = definition.GetTileAt(right - 1, y);
                            tile->SetRightWallFlags(FloorTileWallFlag_Default);
                        }
                    }
                }
                else if (bottom == definition.GetTop() || top == definition.GetBottom())
                {
                    // Walls top/bottom
                    for (int y = top; y < bottom; ++y)
                    {
                        auto tile = definition.GetTileAt(left, y);           // assert(tile != nullptr)
                        tile->SetLeftWallFlags(FloorTileWallFlag_Default);

                        tile = definition.GetTileAt(right - 1, y);           // assert(tile != nullptr)
                        tile->SetRightWallFlags(FloorTileWallFlag_Default);
                    }
                    
                    if (bottom == definition.GetTop())
                    {
                        // Walls on top, open on bottom.
                        for (int x = left; x < right; ++x)
                        {
                            auto tile = definition.GetTileAt(x, top);
                            tile->SetTopWallFlags(FloorTileWallFlag_Default);

                            tile = definition.GetTileAt(x, bottom - 1);
                            tile->SetBottomWallFlags(FloorTileWallFlag_None);
                        }
                    }
                    else
                    {
                        // Walls on bottom, open on top.
                        for (int x = left; x < right; ++x)
                        {
                            auto tile = definition.GetTileAt(x, top);
                            tile->SetTopWallFlags(FloorTileWallFlag_None);

                            tile = definition.GetTileAt(x, bottom - 1);
                            tile->SetBottomWallFlags(FloorTileWallFlag_Default);
                        }
                    }
                }


                // Here is where we create the lobby for the side room. We first calculate a desired size, and then clamp where applicable. We will center
                // the lobbies, but make sure they're not overflowing outside the main area.
                int lobbyWidth  = this->random.Next(static_cast<int>(properties.minLobbyWidth),  static_cast<int>(properties.maxLobbyWidth));
                int lobbyHeight = this->random.Next(static_cast<int>(properties.minLobbyHeight), static_cast<int>(properties.maxLobbyHeight));

                int lobbyLeft;
                int lobbyTop;
                int lobbyRight; (void)lobbyRight;
                int lobbyBottom;

                if (right == definition.GetLeft() || left == definition.GetRight())
                {
                    if (right == definition.GetLeft())
                    {
                        // The side room is on the left, so we create the lobby on the left side.
                        lobbyLeft  = definition.GetLeft();
                        lobbyRight = lobbyLeft + lobbyWidth;

                        lobbyTop    = top + ((bottom - top) / 2) - (lobbyHeight / 2);
                        lobbyBottom = top + lobbyHeight;

                        if (lobbyBottom > definition.GetBottom())
                        {
                            lobbyBottom = definition.GetBottom();
                            lobbyTop    = lobbyBottom - lobbyHeight;
                        }
                        
                        if (lobbyTop < definition.GetTop())
                        {
                            lobbyTop    = definition.GetTop();
                            lobbyBottom = lobbyTop + lobbyHeight;
                        }
                    }
                    else
                    {
                        // It's on the right side.
                    }
                }
                else
                {

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

    void FloorGenerator::GenerateOutsideWalls(const FloorProperties &properties, FloorDefinition &definition)
    {
        (void)properties;

        auto &quadtree = definition.GetQuadtree();

        // We do this edge by edge. Start with left and right.
        for (int y = definition.GetTop(); y < definition.GetBottom(); ++y)
        {
            // Left.
            auto tile = definition.GetTileAt(definition.GetLeft(), y);
            if (quadtree.IsEmptySpace(definition.GetLeft() - 1, y))
            {
                tile->SetLeftWallFlags(FloorTileWallFlag_Default);
            }

            // Right.
            tile = definition.GetTileAt(definition.GetRight() - 1, y);
            if (quadtree.IsEmptySpace(definition.GetRight(), y))
            {
                tile->SetRightWallFlags(FloorTileWallFlag_Default);
            }
        }

        // Top/Bottom.
        for (int x = definition.GetLeft(); x < definition.GetRight(); ++x)
        {
            // Top.
            auto tile = definition.GetTileAt(x, definition.GetTop());
            if (quadtree.IsEmptySpace(x, definition.GetTop() - 1))
            {
                tile->SetTopWallFlags(FloorTileWallFlag_Default);
            }

            // Bottom.
            tile = definition.GetTileAt(x, definition.GetBottom() - 1);
            if (quadtree.IsEmptySpace(x, definition.GetBottom()))
            {
                tile->SetBottomWallFlags(FloorTileWallFlag_Default);
            }
        }
    }

    void FloorGenerator::GenerateRooms(const FloorProperties &properties, FloorDefinition &definition)
    {
        (void)properties;
        (void)definition;
        
        // The way we generate rooms is by creating rows and colums of rooms inside the main area.
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
    }
}
