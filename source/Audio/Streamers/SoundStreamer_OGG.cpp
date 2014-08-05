// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "SoundStreamer_OGG.hpp"

namespace GT
{
    namespace Engine
    {
        SoundStreamer_Vorbis::SoundStreamer_Vorbis(const char* filePath)
            : SoundStreamer(filePath),
              m_file(0), m_fileDataPtr(0),
              m_vorbis(nullptr)
        {
        }

        SoundStreamer_Vorbis::~SoundStreamer_Vorbis()
        {
            GTLib::CloseFile(m_file);   // <-- This will also unmap.
        }



        bool SoundStreamer_Vorbis::Open()
        {
            if (m_file == 0)
            {
                m_file = GTLib::OpenFile(this->absolutePath.c_str(), GTLib::IO::OpenMode::Read);
                if (m_file != 0)
                {
                    m_fileDataPtr = GTLib::MapFile(m_file, static_cast<size_t>(GTLib::GetFileSize(m_file)));        // <-- Note the cast from int64_t to size_t means files are limited to 4GB on 32-bit architectures. Not a problem in practice.
                    if (m_fileDataPtr != nullptr)
                    {
                        int error;
                        m_vorbis = stb_vorbis_open_memory(reinterpret_cast<unsigned char*>(m_fileDataPtr), static_cast<int>(GTLib::GetFileSize(m_file)), &error, nullptr);
                        if (m_vorbis != nullptr)
                        {
                            return true;
                        }
                        else
                        {
                            // We failed top create the Vorbis streamer.
                            GTLib::CloseFile(m_file);
                            m_file = 0;

                            return false;
                        }
                    }
                    else
                    {
                        // We failed to map the file.
                        GTLib::CloseFile(m_file);
                        m_file = 0;

                        return false;
                    }
                }
                else
                {
                    // We failed to open the file.
                    return false;
                }
            }
            else
            {
                // The file is already open. This is an erroneous case.
                assert(false);
                return false;
            }
        }

        void SoundStreamer_Vorbis::Close()
        {
            GTLib::CloseFile(m_file);   // <-- This will also unmap.
            m_file = 0;
            m_fileDataPtr = 0;

            stb_vorbis_close(m_vorbis);
            m_vorbis = nullptr;
        }


        const void* SoundStreamer_Vorbis::ReadNextChunk(size_t &dataSizeOut)
        {
            (void)dataSizeOut;
            return nullptr;
        }


        void SoundStreamer_Vorbis::Seek(double time)
        {
            (void)time;
        }
    }
}