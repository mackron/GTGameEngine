// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/AssetLibrary.hpp>
#include <GTLib/IO.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/String.hpp>

#include "Audio/Streamers/SoundStreamer_WAV.hpp"
#include "Audio/Streamers/SoundStreamer_Vorbis.hpp"

namespace GT
{
    namespace Engine
    {
        AssetLibrary::AssetLibrary()
            : m_openedSoundFiles(), m_openedSoundStreamers()
        {
        }

        AssetLibrary::~AssetLibrary()
        {
        }


        GTEngine::SoundStreamer* AssetLibrary::OpenSoundStreamer(const char* relativeFilePath)
        {
            GTLib::String absoluteFilePath;
            if (GTLib::IO::FindAbsolutePath(relativeFilePath, absoluteFilePath))
            {
                GTEngine::SoundStreamer* streamer     = nullptr;
                GTLib::FileHandle        streamerFile = 0;


                auto iFile = m_openedSoundFiles.Find(absoluteFilePath.c_str());
                if (iFile != nullptr)
                {
                    streamerFile = iFile->value.file;
                    iFile->value.count += 1;
                }
                else
                {
                    streamerFile = GTLib::OpenFile(absoluteFilePath.c_str(), GTLib::IO::OpenMode::Read);
                    if (streamerFile != 0)
                    {
                        m_openedSoundFiles.Add(absoluteFilePath.c_str(), FileCounter(streamerFile, 1));
                    }
                }


                if (streamerFile != 0)
                {
                    auto fileDataSizeInBytes = static_cast<size_t>(GTLib::GetFileSize(streamerFile));
                    auto fileData            = GTLib::MapFile(streamerFile, fileDataSizeInBytes);

                    auto ext = GTLib::Path::Extension(relativeFilePath);
                    if (GTLib::Strings::Equal<false>(ext, "wav"))
                    {
                        streamer = new GTEngine::SoundStreamer_WAV(fileData, fileDataSizeInBytes);
                    }
                    if (GTLib::Strings::Equal<false>(ext, "ogg"))
                    {
                        streamer = new SoundStreamer_Vorbis(fileData, fileDataSizeInBytes);
                    }
                }


                if (streamer != nullptr)
                {
                    if (streamer->Initialize())
                    {
                        m_openedSoundStreamers.Add(streamer, absoluteFilePath);
                        return streamer;
                    }
                    else
                    {
                        // Failed to open the streamer. Probably a corrupt or not the file type it's pretending to be.
                        delete streamer;
                        GTLib::CloseFile(streamerFile);

                        return nullptr;
                    }
                }
                else
                {
                    // Unknown or unsupported file format.
                    GTLib::CloseFile(streamerFile);
                    return nullptr;
                }
            }
            else
            {
                // Could not find the file.
                return false;
            }
        }

        void AssetLibrary::CloseSoundStreamer(GTEngine::SoundStreamer* streamer)
        {
            // We may need to close the file, depending on the counter.
            auto iSoundStreamer = m_openedSoundStreamers.Find(streamer);
            if (iSoundStreamer != nullptr)
            {
                auto iFile = m_openedSoundFiles.Find(iSoundStreamer->value.c_str());
                if (iFile != nullptr)
                {
                    assert(iFile->value.count > 0);
                    {
                        iFile->value.count -= 1;

                        if (iFile->value.count == 0)
                        {
                            GTLib::CloseFile(iFile->value.file);
                            m_openedSoundFiles.RemoveByIndex(iFile->index);
                        }
                    }
                }

                m_openedSoundStreamers.RemoveByIndex(iSoundStreamer->index);
                delete streamer;
            }
            else
            {
                // The sound streamer wasn't opened by OpenSoundStreamer().
                //assert(false);
            }            
        }
    }
}