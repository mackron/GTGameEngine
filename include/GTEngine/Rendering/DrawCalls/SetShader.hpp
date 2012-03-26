
#ifndef __GTEngine_DrawCall_SetShader_hpp_
#define __GTEngine_DrawCall_SetShader_hpp_

#include "../RenderCommands.hpp"

namespace GTEngine
{
    class Shader;

    namespace DrawCalls
    {
        class SetShader : public RenderCommand
        {
        public:

            SetShader(Shader *shader);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static SetShader * Allocate(Shader *shader)
            {
                return new SetShader(shader);
            }

            static void Deallocate(SetShader *call)
            {
                delete call;
            }


        private:

            Shader *    shader;

        private:    // No copying.
            SetShader(const SetShader &);
            SetShader & operator=(const SetShader &);
        };
    }
}

#endif