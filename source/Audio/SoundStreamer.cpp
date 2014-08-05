// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Audio/SoundStreamer.hpp>
#include <GTLib/Path.hpp>

namespace GTEngine
{
    SoundStreamer::SoundStreamer(const char* fileName)
        : absolutePath()
    {
        GTLib::Path path(fileName);
        path.MakeAbsolute();

        this->absolutePath = path.c_str();
    }

    SoundStreamer::~SoundStreamer()
    {
    }

#if 0
    size_t SoundStreamer::GetChunkSize() const
    {
        // TODO: Handle this correctly.
        return 16384;
    }
#endif
}
