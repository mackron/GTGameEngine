
#ifndef __GTEngine_DrawCall_Draw_hpp_
#define __GTEngine_DrawCall_Draw_hpp_

#include "../RenderCommand.hpp"
#include "../DrawModes.hpp"

namespace GTEngine
{
    class VertexArray;

    namespace DrawCalls
    {
        class Draw : public RenderCommand
        {
        public:

            Draw(const VertexArray *vertexArray, DrawMode mode);
            void Execute();
            void OnExecuted();


        public: // Allocation/Deallocation.

            static Draw * Allocate(const VertexArray *vertexArray, DrawMode mode)
            {
                return new Draw(vertexArray, mode);
            }

            static void Deallocate(Draw *call)
            {
                delete call;
            }


        private:

            /// The vertex array that's being drawn.
            const VertexArray *vertexArray;

            /// The draw mode to use when drawing (triangles, lines).
            DrawMode mode;


        private:    // No copying.
            Draw(const Draw &);
            Draw & operator=(const Draw &);
        };
    }
}

#endif