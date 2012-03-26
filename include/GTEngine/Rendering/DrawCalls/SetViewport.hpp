
#ifndef __GTEngine_DrawCall_SetViewport_hpp_
#define __GTEngine_DrawCall_SetViewport_hpp_

#include "../RenderCommands.hpp"

namespace GTEngine
{
    namespace DrawCalls
    {
        class SetViewport : public RenderCommand
        {
        public:

            SetViewport(int x, int y, unsigned int width, unsigned int height);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetViewport * Allocate(int x, int y, unsigned int width, unsigned int height)
            {
                return new SetViewport(x, y, width, height);
            }

            static void Deallocate(SetViewport *call)
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