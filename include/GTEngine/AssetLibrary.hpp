// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_Engine_AssetLibrary_hpp_
#define __GT_Engine_AssetLibrary_hpp_

#include "Assets/Asset.hpp"
#include "Audio/SoundStreamer.hpp"
#include <GTLib/Dictionary.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/IO.hpp>

namespace GT
{
    namespace Engine
    {
        /// Class for loading and managing assets.
        class AssetLibrary
        {
        public:

            /// Constructor.
            AssetLibrary();

            /// Destructor.
            ~AssetLibrary();



            ///////////////////////////////////////////////////
            // Images / Textures





            ///////////////////////////////////////////////////
            // Sounds

            /// Creates a sound streamer using the given relative file path.
            ///
            /// @param relativeFilePath [in] The relative file path of the sound file to load.
            ///
            /// @return A pointer to the sound streamer for the given file; or null if the file could not be opened.
            ///
            /// @remarks
            ///     The returned streamer should be deleted with CloseSoundStreamer().
            ///     @par
            ///     The internal file pointer will remain open until the streamer is closed with CloseSoundStreamer().
            GTEngine::SoundStreamer* OpenSoundStreamer(const char* relativeFilePath);

            /// Closes the given sound streamer that was opened with OpenSoundStreamer().
            ///
            /// @param streamer [in] A reference to the streamer to close.
            ///
            /// @remarks
            ///     The streamer will become invalid after calling this method.
            ///     @par
            ///     This should only be called on streamers that were opened with OpenSoundStreamer().
            void CloseSoundStreamer(GTEngine::SoundStreamer* streamer);
            void CloseSoundStreamer(GTEngine::SoundStreamer &streamer) { this->CloseSoundStreamer(&streamer); }





        private:

            /// The list of opened assets. This is keyed by the absolute path of the asset.
            GTLib::Dictionary<Asset*> m_openedAssets;





            struct FileCounter
            {
                FileCounter(GTLib::FileHandle file, unsigned int count)
                    : file(file), count(count)
                {
                }

                /// The file handle.
                GTLib::FileHandle file;

                /// The current open counter. When a file is opened, this is incremented. When it is closed, this is decremented. When it hits 0,
                /// the file will be closed for real.
                unsigned int count;
            };


            struct AssetFile
            {
                /// The absolute path of the file.
                GTLib::String absolutePath;

                /// The current open counter. When an asset is opened, this is incremented. When it is closed, it is decremented. When it hits 0,
                /// the asset will be deleted for real.
                unsigned int counter;

                /// The file handle for the asset. If the asset requires an open file handle (such as a sound) and this is 0, the handle will need
                /// to be reopened, which can be done by using absolutePath.
                GTLib::FileHandle hFile;
            };



            /// The list of currently opened sound files. When a sound streamer is opened, it reuses an already-opened file.
            GTLib::Dictionary<FileCounter> m_openedSoundFiles;

            /// The list of currently opened sound streamers, with the absolute file path as the value.
            GTLib::Map<GTEngine::SoundStreamer*, GTLib::String> m_openedSoundStreamers;
        };
    }
}

#endif // !__GT_Engine_AssetLibrary_hpp_
