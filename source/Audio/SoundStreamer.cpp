
#include <GTEngine/Audio/SoundStreamer.hpp>
#include <GTCore/Path.hpp>

namespace GTEngine
{
    SoundStreamer::SoundStreamer(const char* fileName)
        : absolutePath(),
          numChannels(0), bitsPerSample(0), sampleRate(0), format(SoundFormat_Mono8)
    {
        GTCore::Path path(fileName);
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
