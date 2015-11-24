// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/AssetLibrary.hpp>
#include <GTEngine/GTEngine.hpp>
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
            char absoluteFilePath[EASYVFS_MAX_PATH];
            if (easyvfs_find_absolute_path(GTEngine::g_EngineContext->GetVFS(), relativeFilePath, absoluteFilePath, sizeof(absoluteFilePath)))
            {
                GTEngine::SoundStreamer* streamer     = nullptr;
                easyvfs_file*            streamerFile = nullptr;
                size_t fileSize = 0;
                void* fileData = nullptr;


                m_openedSoundFilesLock.Lock();
                auto iFile = m_openedSoundFiles.Find(absoluteFilePath);
                if (iFile != nullptr)
                {
                    streamerFile = iFile->value.m_file;
                    fileSize = iFile->value.dataSize;
                    fileData = iFile->value.pFileData;

                    iFile->value.m_count += 1;
                }
                else
                {
                    streamerFile = easyvfs_open(GTEngine::g_EngineContext->GetVFS(), absoluteFilePath, EASYVFS_READ, 0);
                    if (streamerFile != 0)
                    {
                        fileSize = (size_t)easyvfs_file_size(streamerFile);
                        fileData = malloc(fileSize);
                        easyvfs_read(streamerFile, fileData, fileSize, nullptr);

                        m_openedSoundFiles.Add(absoluteFilePath, FileCounter(streamerFile, 1, fileSize, fileData));
                    }
                }
                m_openedSoundFilesLock.Unlock();


                if (streamerFile != 0)
                {
                    auto ext = GTLib::Path::Extension(relativeFilePath);
                    if (GTLib::Strings::Equal<false>(ext, "wav"))
                    {
                        streamer = new GTEngine::SoundStreamer_WAV(fileData, fileSize);
                    }
                    if (GTLib::Strings::Equal<false>(ext, "ogg"))
                    {
                        streamer = new SoundStreamer_Vorbis(fileData, fileSize);
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
                        easyvfs_close(streamerFile);

                        return nullptr;
                    }
                }
                else
                {
                    // Unknown or unsupported file format.
                    easyvfs_close(streamerFile);
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
                m_openedSoundFilesLock.Lock();
                auto iFile = m_openedSoundFiles.Find(iSoundStreamer->value.c_str());
                if (iFile != nullptr)
                {
                    assert(iFile->value.m_count > 0);
                    {
                        iFile->value.m_count -= 1;

                        if (iFile->value.m_count == 0)
                        {
                            easyvfs_close(iFile->value.m_file);
                            free(iFile->value.pFileData);
                            m_openedSoundFiles.RemoveByIndex(iFile->index);
                        }
                    }
                }
                m_openedSoundFilesLock.Unlock();

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
