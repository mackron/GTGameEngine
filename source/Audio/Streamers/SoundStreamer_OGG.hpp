// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GT_Engine_SoundStreamer_OGG_hpp_
#define __GT_Engine_SoundStreamer_OGG_hpp_

#include <GTEngine/Audio/SoundStreamer.hpp>
#include <GTLib/IO.hpp>

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

namespace GT
{
    namespace Engine
    {
        /// Sound streamer for Ogg Vorbis files.
        class SoundStreamer_Vorbis : public GTEngine::SoundStreamer
        {
        public:

            /// Constructor.
            SoundStreamer_Vorbis(const char* filePath);

            /// Destructor.
            ~SoundStreamer_Vorbis();


            /// SoundStreamer::GetChunkSize().
            //size_t GetChunkSize() const;


            /// SoundStreamer::Open().
            bool Open();

            /// SoundStreamer::Close().
            void Close();


            /// SoundStreamer::ReadChunk().
            const void* ReadNextChunk(size_t &dataSizeOut);

            /// SoundStreamer::Seek().
            ///
            /// @remarks
            ///     This currently doesn't work for Vorbis files except for when time is 0.0, in which case it'll be rewound to the start.
            void Seek(double time);

            /// SoundStreamer::GetTotalPCMDataSize().
            //uint64_t GetTotalPCMDataSize();


        private:

            /// The handle to the file we'll be reading the audio data from. This file needs to support mapping because we're just going
            /// to pass a pointer to the mapped data to the Vorbis reader.
            GTLib::FileHandle m_file;

            /// The mapped file pointer.
            void* m_fileDataPtr;


            /// A pointer to the stb_vorbis for doing the actual reading of the Vorbis file.
            stb_vorbis* m_vorbis;


        private:    // No copying.
            SoundStreamer_Vorbis(const SoundStreamer_Vorbis &);
            SoundStreamer_Vorbis & operator=(const SoundStreamer_Vorbis &);
        };
    }
}

#endif
