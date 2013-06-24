// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_CPUVertexShader_SimpleTransform_hpp_
#define __GTEngine_CPUVertexShader_SimpleTransform_hpp_

#include "CPUVertexShader.hpp"

namespace GTEngine
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
        glm::simdMat4 transform;

        /// The transformation matrix for normals.
        glm::mat3 normalTransform;

    };
}

#endif