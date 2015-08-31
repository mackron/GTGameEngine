// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Engine_AudioDataFormats
#define GT_Engine_AudioDataFormats

namespace GT
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

        AudioDataFormat_Mono32F,        ///< 32-bit float, single channel.
        AudioDataFormat_Stereo32F,      ///< 32-bit float, stereo channel.

        AudioDataFormat_Mono64F,        ///< 64-bit float, single channel.
        AudioDataFormat_Stereo64F,      ///< 64-bit float, stereo channel.

        AudioDataFormat_Mono_ALaw,      ///< 8-bit, single channel.
        AudioDataFormat_Stereo_ALaw,    ///< 8-bit, single channel.

        AudioDataFormat_Mono_ULaw,      ///< 8-bit, single channel.
        AudioDataFormat_Stereo_ULaw,    ///< 8-bit, single channel.
    };
}

#endif
