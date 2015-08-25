// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/FontEventHandler.hpp>
#include <GTLib/FontServer.hpp>

namespace GTLib
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
