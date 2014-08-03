// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "SoundStreamer_WAV.hpp"
#include <GTEngine/Errors.hpp>
#include <GTLib/IO.hpp>

namespace GTEngine
{
    SoundStreamer_WAV::SoundStreamer_WAV(const char* fileName)
        : SoundStreamer(fileName), file(nullptr), dataStartPos(0), dataSize(0), readChunkReturnedZero(false)
    {
    }

    SoundStreamer_WAV::~SoundStreamer_WAV()
    {
        GTLib::IO::Close(this->file);
        this->file = nullptr;
    }

    bool SoundStreamer_WAV::Open()
    {
        // If the file is already open, we'll move back to the start.
        if (this->file != nullptr)
        {
            assert(false);
            return false;
        }
        else
        {
            this->file = GTLib::IO::Open(this->absolutePath.c_str(), GTLib::IO::OpenMode::Read);
            if (this->file != nullptr)
            {
                // First we will read the RIFF stuff from the WAV file. These functions will output the appropriate errors.
                if (this->ReadRIFFHeader())
                {
                    if (this->ReadRIFFChunks())
                    {
                        // We now need to determine our format.
                        this->CalculateFormat();

                        // Now we need to seek to the start of our data so that we can start reading it.
                        GTLib::IO::Seek(this->file, this->dataStartPos, GTLib::SeekOrigin::Start);

                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    void SoundStreamer_WAV::Close()
    {
        GTLib::IO::Close(this->file);
        this->file = nullptr;
    }


    uint32_t SoundStreamer_WAV::ReadChunk(void* data)
    {
        assert(data != nullptr);

        // If we're at the end of the data, we need to seek back to the start of the sound.
        if (static_cast<uint64_t>(GTLib::IO::Tell(this->file)) == this->dataSize + this->dataStartPos)
        {
            if (!this->readChunkReturnedZero)
            {
                this->readChunkReturnedZero = true;
                return 0;
            }
            else
            {
                GTLib::IO::Seek(this->file, this->dataStartPos, GTLib::SeekOrigin::Start);
            }
        }

        // If we make it here we shouldn't be returning 0.
        this->readChunkReturnedZero = false;

        // Grab the size of a chunk for ease of use.
        uint32_t chunk_size = this->GetChunkSize();

        // Grab the number of bytes from the current position to the end of the data.
        uint32_t bytes_left = static_cast<uint32_t>(this->dataSize - (static_cast<uint64_t>(GTLib::IO::Tell(this->file)) - this->dataStartPos));

        // The number of bytes to read.
        uint32_t bytes_to_read = (chunk_size < bytes_left) ? chunk_size : bytes_left;

        // Read the data from the file.
        GTLib::IO::Read(this->file, data, bytes_to_read);

        // Fill any unread part of the buffer with silence.
        memset(((char *)data) + bytes_to_read, 0, this->GetChunkSize() - bytes_to_read);

        // Return the number of bytes we have read. If were at the end of the file, we will return 0.
        return bytes_to_read;
    }

    void SoundStreamer_WAV::Seek(double time)
    {
        assert(time >= 0.0);

        // We can determine where we need to seek to by looking at our bytesPerSecond variable. To get good
        // precision, we will convert our input time to milliseconds and then we will retrieve the bytes
        // per millisecond. From here we can calculate our new position.
        uint32_t time_mills     = static_cast<uint32_t>(time * 1000);
        uint32_t bytes_per_mill = this->GetBytesPerSecond() / 1000;

        uint32_t new_position = bytes_per_mill * time_mills;
        GTLib::IO::Seek(this->file, this->dataStartPos + new_position, GTLib::SeekOrigin::Start);
    }

    uint64_t SoundStreamer_WAV::GetTotalPCMDataSize()
    {
        return this->dataSize;
    }




    bool SoundStreamer_WAV::ReadRIFFHeader()
    {
        // The first 4 bytes should read "RIFF".
	    char riff[4];
        GTLib::IO::Read(this->file, riff, 4);
        if (riff[0] != 'R' || riff[1] != 'I' || riff[2] != 'F' || riff[3] != 'F')
	    {
            GTEngine::PostError("SoundStreamer_WAV::ReadRIFFHeader() - File is not a RIFF file.");
		    return false;
	    }

	    // Grab the size of the file.
	    uint32_t fileSize;
        GTLib::IO::Read(this->file, &fileSize, 4);
	    if (fileSize == 0)
	    {
            GTEngine::PostError("SoundStreamer_WAV::ReadRIFFHeader() - File size is 0.");
		    return false;
	    }

	    // The next 4 bytes should read "WAVE". This indicates that the RIFF
	    // file is a WAVE file.
	    char wave[4];
        GTLib::IO::Read(this->file, wave, 4);
        if (wave[0] != 'W' || wave[1] != 'A' || wave[2] != 'V' || wave[3] != 'E')
	    {
            GTEngine::PostError("SoundStreamer_WAV::ReadRIFFHeader() - File is not a WAVE file.");
		    return false;
	    }

	    return true;
    }

    bool SoundStreamer_WAV::ReadRIFFChunks()
    {
        // The name of the chunk we're reading.
	    char chunk_name[5]; chunk_name[4] = '\0';

	    // The size of the chunk we're looking at so we can skip over it.
	    uint32_t chunk_size = 0;

	    // Determines if we've found our needed chunks.
	    bool found_fmt = false;
	    bool found_data = false;

        while ((!found_fmt || !found_data) && !GTLib::IO::AtEnd(this->file))
	    {
		    // Read in the name and size of the chunk.
            GTLib::IO::Read(this->file, chunk_name, 4);
            GTLib::IO::Read(this->file, &chunk_size, 4);

		    // Now check the name of the chunk and perform the appropriate operations.
            if (GTLib::Strings::Equal<false>(chunk_name, "fmt "))
		    {
			    // We've found the fmt chunk.
			    found_fmt = true;

			    // Grab the compression code. We only support a value of 1 (PCM).
			    uint16_t compression_code = 0;
                GTLib::IO::Read(this->file, &compression_code, 2);
			    if (compression_code != 1)
			    {
                    GTEngine::PostError("SoundStreamer_WAV::ReadRIFFChunks() - Unsupported compression format.");
				    return false;
			    }

			    // Read the number of channels.
                GTLib::IO::Read(this->file, &this->numChannels, 2);

			    // Read the sample rate.
                GTLib::IO::Read(this->file, &this->sampleRate, 4);

			    // We can skip over our bytes per second and block align. These can be calculated from other properties.
                GTLib::IO::Seek(this->file, 4, GTLib::SeekOrigin::Current);   // <-- bytes-per-second
                GTLib::IO::Seek(this->file, 2, GTLib::SeekOrigin::Current);   // <-- block align

			    // Read the significant bits per sample.
                GTLib::IO::Read(this->file, &this->bitsPerSample, 2);

			    // Now we need to check if we have to read the number of extra bytes. We can
			    // determine this by looking at the block size. If it is 16, we don't have
			    // the bytes for the extra part. If it is 18, we do.
			    if (chunk_size == 18)
			    {
				    // Read in the extra bytes and skip over them.
				    uint16_t extra = 0;
                    GTLib::IO::Read(this->file, &extra, 2);
                    GTLib::IO::Seek(this->file, extra, GTLib::SeekOrigin::Current);
			    }
		    }
            else if (GTLib::Strings::Equal<false>(chunk_name, "data"))
		    {
			    // We've found the data chunk.
			    found_data = true;

			    // Our current position is the start of the data.
                this->dataStartPos = static_cast<uint64_t>(GTLib::IO::Tell(this->file));

			    // Set the size of the data.
			    this->dataSize = chunk_size;

			    // Skip over the data. We will move back here when it is time to read the data.
                GTLib::IO::Seek(this->file, this->dataSize, GTLib::SeekOrigin::Current);
		    }
		    else
		    {
			    // We're not interested in the chunk, so skip over it.
                GTLib::IO::Seek(this->file, chunk_size, GTLib::SeekOrigin::Current);
		    }
	    }

	    if (!found_fmt || !found_data)
	    {
            GTEngine::PostError("SoundStreamer_WAV::ReadRIFFChunks() - FMT and/or DATA chunks were not found.");
		    return false;
	    }

	    return true;
    }

    uint32_t SoundStreamer_WAV::GetBytesPerSecond() const
    {
	    return ((this->bitsPerSample / 8U) * this->numChannels) * this->sampleRate;
    }

    void SoundStreamer_WAV::CalculateFormat()
    {
        if (this->bitsPerSample == 8)
        {
            if (this->numChannels == 1)
            {
                this->format = AudioDataFormat_Mono8;
            }
            else if (this->numChannels == 2)
            {
                this->format = AudioDataFormat_Stereo8;
            }
        }
        else if (this->bitsPerSample == 16)
        {
            if (this->numChannels == 1)
            {
                this->format = AudioDataFormat_Mono16;
            }
            else if (this->numChannels == 2)
            {
                this->format = AudioDataFormat_Stereo16;
            }
        }
        else if (this->bitsPerSample == 24)
        {
            if (this->numChannels == 1)
            {
                this->format = AudioDataFormat_Mono24;
            }
            else if (this->numChannels == 2)
            {
                this->format = AudioDataFormat_Stereo24;
            }
        }
    }
}
