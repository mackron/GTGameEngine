// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Audio_SoundFormats_hpp_
#define __GTEngine_Audio_SoundFormats_hpp_

namespace GTEngine
{
    /**
    *   \brief  Enumerator for the different sound formats.
    */
    enum SoundFormat
    {
        SoundFormat_Mono8,          ///< 8-bit, single channel.
        SoundFormat_Stereo8,        ///< 8-bit, stereo channel.

        SoundFormat_Mono16,         ///< 16-bit, single channel.
        SoundFormat_Stereo16,       ///< 16-bit, stereo channel.

        SoundFormat_Mono24,         ///< 24-bit, single channel.
        SoundFormat_Stereo24,       ///< 24-bit, stereo channel.

        SoundFormat_Mono32F,        ///< 32-bit float, single channel.
        SoundFormat_Stereo32F,      ///< 32-bit float, stereo channel.
    };
}

#endif
