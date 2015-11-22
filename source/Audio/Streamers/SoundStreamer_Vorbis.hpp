// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_Engine_SoundStreamer_Vorbis_hpp_
#define __GT_Engine_SoundStreamer_Vorbis_hpp_

#include <GTEngine/Audio/SoundStreamer.hpp>
#include <GTLib/IO.hpp>

#define STB_VORBIS_HEADER_ONLY
#include <GTEngine/external/stb_vorbis.c>

namespace GT
{
    namespace Engine
    {
        /// Sound streamer for Ogg Vorbis files.
        class SoundStreamer_Vorbis : public GTEngine::SoundStreamer
        {
        public:

            /// Constructor.
            SoundStreamer_Vorbis(const void* fileData, size_t fileDataSizeInBytes);

            /// Destructor.
            ~SoundStreamer_Vorbis();


            /// SoundStreamer::Initialize()
            bool Initialize();


            /// SoundStreamer::ReadChunk().
            const void* ReadNextChunk(size_t &dataSizeOut);

            /// SoundStreamer::Seek().
            ///
            /// @remarks
            ///     This currently doesn't work for Vorbis files except for when time is 0.0, in which case it'll be rewound to the start.
            void Seek(double time);

            /// SoundStreamer::GetNumChannels()
            uint16_t GetNumChannels() const;

            /// SoundStreamer::GetBitsPerSample()
            uint16_t GetBitsPerSample() const;

            /// SoundStreamer::GetSampleRate()
            uint32_t GetSampleRate() const;

            /// SoundStreamer::GetFormat()
            GTEngine::AudioDataFormat GetFormat() const;


        private:

            /// A pointer to the file data.
            const void* m_fileDataPtr;

            /// The size of the file data in bytes.
            size_t m_fileDataSizeInBytes;


            /// A pointer to the stb_vorbis for doing the actual reading of the Vorbis file.
            stb_vorbis* m_vorbis;

            /// The structure containing the Vorbis data so we don't have to keep calling stb_vorbis_get_info().
            stb_vorbis_info m_vorbisInfo;


            /// A pointer to the buffer that contains the sample data of the next data chunk. This is allocated and freed once.
            float* m_nextChunkData;



        private:    // No copying.
            SoundStreamer_Vorbis(const SoundStreamer_Vorbis &);
            SoundStreamer_Vorbis & operator=(const SoundStreamer_Vorbis &);
        };
    }
}

#endif
