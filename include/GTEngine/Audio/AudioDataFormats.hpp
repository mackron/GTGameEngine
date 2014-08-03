// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#ifndef __GT_Engine_AudioDataFormats_hpp_
#define __GT_Engine_AudioDataFormats_hpp_

namespace GTEngine
{
    /// Enumerator for the various supporting audio data formats.
    enum AudioDataFormat
    {
        AudioDataFormat_Mono8,          ///< 8-bit, single channel.
        AudioDataFormat_Stereo8,        ///< 8-bit, stereo channel.

        AudioDataFormat_Mono16,         ///< 16-bit, single channel.
        AudioDataFormat_Stereo16,       ///< 16-bit, stereo channel.

        AudioDataFormat_Mono24,         ///< 24-bit, single channel.
        AudioDataFormat_Stereo24,       ///< 24-bit, stereo channel.

        //AudioDataFormat_Mono32F,        ///< 32-bit float, single channel.
        //AudioDataFormat_Stereo32F,      ///< 32-bit float, stereo channel.
    };
}

#endif
