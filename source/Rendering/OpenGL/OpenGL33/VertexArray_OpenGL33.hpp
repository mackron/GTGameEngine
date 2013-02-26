// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_VertexArray_OpenGL33_hpp_
#define __GTEngine_VertexArray_OpenGL33_hpp_

#include <GTEngine/Rendering/VertexArray.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// OpenGL 3.3 vertex array.
    ///
    /// The object used here is a VAO object, but we also keep hold of the vertex and index buffer objects.
    class VertexArray_OpenGL33 : public VertexArray
    {
    public:

        /// Constructor.
        VertexArray_OpenGL33(VertexArrayUsage usage, const VertexFormat &format, GLuint* objectGLIn, GLuint* vertexObjectIn, GLuint* indexObjectIn);

        /// Destructor.
        ~VertexArray_OpenGL33();


        /// Retrieves a pointer to the main OpenGL VAO object.
        GLuint* GetOpenGLObjectPtr() const { return this->objectGL; }

        /// Retrieves a pointer to the OpenGL object containing the vertex data.
        GLuint* GetOpenGLVertexObjectPtr() const { return this->vertexObjectGL; }

        /// Retrieves a pointer to the OpenGL object containing the index data.
        GLuint* GetOpenGLIndexObjectPtr() const { return this->indexObjectGL; }



    private:

        ///////////////////////////////////////
        // Events.

        /// VertexArray::OnVertexDataChanged()
        void OnVertexDataChanged();

        /// VertexArray::OnIndexDataChanged()
        void OnIndexDataChanged();


    private:

        /// A pointer to the main VAO object.
        GLuint* objectGL;

        /// A pointer to the OpenGL object containing the vertex VBO object.
        GLuint* vertexObjectGL;

        /// A pointer to the OpenGL object container the index VBO object.
        GLuint* indexObjectGL;
    };
}

#endif