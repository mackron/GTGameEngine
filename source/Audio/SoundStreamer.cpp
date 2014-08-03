// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Audio/SoundStreamer.hpp>
#include <GTLib/Path.hpp>

namespace GTEngine
{
    SoundStreamer::SoundStreamer(const char* fileName)
        : absolutePath(),
          numChannels(0), bitsPerSample(0), sampleRate(0), format(AudioDataFormat_Mono8)
    {
        GTLib::Path path(fileName);
        path.MakeAbsolute();

        this->absolutePath = path.c_str();
    }

    SoundStreamer::~SoundStreamer()
    {
    }

    uint32_t SoundStreamer::GetChunkSize()
    {
        // TODO: Handle this correctly.
        return 16384;
    }
}
