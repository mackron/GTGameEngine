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


        /// Determines whether or not the vertex data needs updating.
        bool DoesVertexDataNeedUpdating() const { return this->vertexDataNeedsUpdating; }

        /// Determines whether or not the index data needs updating.
        bool DoesIndexDataNeedUpdating() const { return this->indexDataNeedsUpdating; }


        /// Marks the vertex data as updated.
        void MarkVertexDataAsUpdated() const { this->vertexDataNeedsUpdating = false; }

        /// Marks the index data as updated.
        void MarkIndexDataAsUpdated() const { this->indexDataNeedsUpdating = false; }



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

        /// Keeps track of whether or not the vertex data needs updating on the renderer.
        mutable bool vertexDataNeedsUpdating;

        /// Keeps track of whether or not the index data needs updating on the renderer.
        mutable bool indexDataNeedsUpdating;
        
        
    private:    // No copying.
        VertexArray_OpenGL33(const VertexArray_OpenGL33 &);
        VertexArray_OpenGL33 & operator=(const VertexArray_OpenGL33 &);
    };
}

#endif
