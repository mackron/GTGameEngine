
#ifndef __GTEngine_DrawCall_SetFramebuffer_hpp_
#define __GTEngine_DrawCall_SetFramebuffer_hpp_

#include "../RenderCommands.hpp"

namespace GTEngine
{
    class Framebuffer;

    namespace DrawCalls
    {
        class SetFramebuffer: public RenderCommand
        {
        public:

            SetFramebuffer(Framebuffer *framebuffer);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetFramebuffer * Allocate(Framebuffer *framebuffer)
            {
                return new SetFramebuffer(framebuffer);
            }

            static void Deallocate(SetFramebuffer *call)
            {
                delete call;
            }


        private:

            Framebuffer *framebuffer;

        private:    // No copying.
            SetFramebuffer(const SetFramebuffer &);
            SetFramebuffer & operator=(const SetFramebuffer &);
        };
    }
}

#endif