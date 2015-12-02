// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/FontEventHandler.hpp>
#include <GTGE/Core/FontServer.hpp>

namespace GT
{
    FontEventHandler::~FontEventHandler()
    {
    }

    void FontEventHandler::OnCreateFont(FontServer &, Font &)
    {
    }

    void FontEventHandler::OnDeleteFont(FontServer &, Font &)
    {
    }
}