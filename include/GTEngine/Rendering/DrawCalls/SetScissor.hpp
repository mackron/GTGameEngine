
#ifndef __GTEngine_DrawCall_SetScissor_hpp_
#define __GTEngine_DrawCall_SetScissor_hpp_

#include "../RenderCommands.hpp"

namespace GTEngine
{
    namespace DrawCalls
    {
        class SetScissor : public RenderCommand
        {
        public:

            SetScissor(int x, int y, unsigned int width, unsigned int height);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetScissor * Allocate(int x, int y, unsigned int width, unsigned int height)
            {
                return new SetScissor(x, y, width, height);
            }

            static void Deallocate(SetScissor *call)
            {
                delete call;
            }


        private:

            int x;
            int y;
            unsigned int width;
            unsigned int height;
        };
    }
}

#endif