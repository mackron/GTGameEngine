
#ifndef __GTEngine_Audio_SoundStreamer_WAV_hpp_
#define __GTEngine_Audio_SoundStreamer_WAV_hpp_

#include <GTEngine/Audio/SoundStreamer.hpp>

namespace GTEngine
{
    class SoundStreamer_WAV : public SoundStreamer
    {
    public:

        /// Constructor.
        SoundStreamer_WAV(const char* fileName);

        /// Destructor.
        virtual ~SoundStreamer_WAV();


    // Virtual implementations.
    public:

        /// Open()
        bool Open();

        /// Close()
        void Close();


        /**
        *   \brief             Reads a chunk of data.
        *   \param  data [out] Pointer to the buffer that will recieve the data.
        *   \return            The number of bytes that was read.
        *
        *   \remarks
        *       The size of the buffer pointed to by \c data in bytes should be equal to value returned
        *       by SoundStreamer::GetChunkSize().
        *       \par
        *       When reading a chunk causes the streamer to reach the end of the file, it will not read
        *       from the start of the sound to fill up the chunk.
        *       \par
        *       When this function returns 0, it can be assumed that the sound is at the end of itself.
        *       A subsequent call to ReadChunk() will cause the streamer to move back to the start of
        *       the file.
        */
        uint32_t ReadChunk(void *data);

        /**
        *   \brief            Seeks the read positition of the streamer.
        *   \param  time [in] The time in seconds to seek to.
        *
        *   \remarks
        *       To seek to the start of the sound, call this method with a value of 0.
        *       \par
        *       The input time is always clamped between 0 and the length of the sound in seconds.
        */
        void Seek(double time);


        /**
        *   \brief  Retrieves the total size of the PCM data.
        *   \return The total size of the PCM data.
        */
        uint64_t GetTotalPCMDataSize();



    private:

        /**
        *   \brief  Reads the RIFF header from the WAV file.
        *   \return True if successful; false otherwise.
        */
        bool ReadRIFFHeader();

        /**
        *   \brief  Reads the RIFF chunks from the WAV file.
        *   \return True if successful; false otherwise.
        */
        bool ReadRIFFChunks();

        /**
        *   \brief  Determines our sound format.
        */
        void CalculateFormat();

        /**
        *   \brief  Retrieves the number of bytes of PCM data that is played per second.
        *   \return The number of PCM data that is played per second.
        */
	    uint32_t GetBytesPerSecond() const;



    private:

        /// The file that we're streaming from. This will opened in Open() and closed in Close().
        FILE* file;

        /// The position in the file of the beginning of the sound data.
        uint64_t dataStartPos;

        /// The size of the sound data.
        uint64_t dataSize;

        /// Determines if ReadChunk() had previously returned 0. We use this in determining whether or not the loop back to the start.
        bool readChunkReturnedZero;
        
        
    private:    // No copying.
        SoundStreamer_WAV(const SoundStreamer_WAV &);
        SoundStreamer_WAV & operator=(const SoundStreamer_WAV &);
    };
}


#endif
