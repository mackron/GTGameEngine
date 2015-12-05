// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGE/Graphics/GraphicsAPI_Null.hpp>

namespace GT
{
    GraphicsAPI_Null::GraphicsAPI_Null()
        : GraphicsAPI()
    {
    }

    GraphicsAPI_Null::~GraphicsAPI_Null()
    {
    }


    bool GraphicsAPI_Null::Startup()
    {
        return true;
    }

    void GraphicsAPI_Null::Shutdown()
    {
    }


    GraphicsAPIType GraphicsAPI_Null::GetType() const
    {
        return GraphicsAPIType_Null;
    }
}
