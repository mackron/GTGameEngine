// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_CPUVertexShader_SimpleTransform
#define GT_CPUVertexShader_SimpleTransform

#include "CPUVertexShader.hpp"

namespace GT
{
    /// A CPU vertex shader for performing a simple matrix transformation.
    ///
    /// This performs a transformation on the position and normal.
    class CPUVertexShader_SimpleTransform : public CPUVertexShader
    {
    public:

        /// Constructor.
        CPUVertexShader_SimpleTransform(const glm::mat4 &transform);

        /// Destructor.
        ~CPUVertexShader_SimpleTransform();


        /// Sets the transformation matrix to use with this shader.
        void SetTransform(const glm::mat4 &transform);


    protected:

        /// CPUVertexShader::ProcessVertex()
        void ProcessVertex(Vertex &vertex);


    private:

        /// The matrix to apply to the vertex transformation.
        glm::mat4 m_transform;

        /// The transformation matrix for normals.
        glm::mat3 m_normalTransform;

    };
}

#endif