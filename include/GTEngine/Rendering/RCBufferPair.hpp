#ifndef __GTEngine_RCBufferPair_hpp_
#define __GTEngine_RCBufferPair_hpp_

#include "RCQueue.hpp"

namespace GTEngine
{
    /**
    *   \brief  Small class representing a pair of render command queues.
    */
    class RCBufferPair
    {
    public:

        /**
        *   \brief  Constructor.
        */
        RCBufferPair()
            : back(nullptr), front(nullptr), buffers()
        {
            back  = &this->buffers[0];
            front = &this->buffers[1];
        }


        /**
        *   \brief  Swaps the back and front buffers.
        *
        *   \remarks
        *       This will clear the buffer that has just become the back buffer.
        */
        void Swap()
        {
            auto temp   = this->back;
            this->back  = this->front;
            this->front = temp;

            this->back->Clear();
        }


    public:

        /// The back RC buffer.
        RCQueue * back;

        /// The front RC buffer.
        RCQueue * front;


    private:

        /// The two buffers.
        RCQueue buffers[2];


    private:    // No copying.
        RCBufferPair(const RCBufferPair &);
        RCBufferPair & operator=(const RCBufferPair &);

    };
}

#endif