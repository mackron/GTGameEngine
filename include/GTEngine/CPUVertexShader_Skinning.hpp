
#ifndef __GTEngine_CPUVertexShader_Skinning_hpp_
#define __GTEngine_CPUVertexShader_Skinning_hpp_

#include "CPUVertexShader.hpp"
#include "SkinningVertexAttribute.hpp"

namespace GTEngine
{
    /// A CPU vertex shader for applying skeletal vertex blending.
    ///
    /// The bones affecting each vertex is stored as a vertex attribute, which is simply an array that's indexable by the
    /// vertex ID.
    class CPUVertexShader_Skinning : public CPUVertexShader
    {
    public:

        /// Constructor.
        CPUVertexShader_Skinning();

        /// Destructor.
        ~CPUVertexShader_Skinning();


        /// Sets the pointer to the buffer containing the skinning vertex attributes for each vertex.
        ///
        /// Note that this does not make a copy of the buffer; it simply sets the internal pointer.
        void SetSkinningVertexAttributes(const SkinningVertexAttribute* attributes);


    protected:

        /// CPUVertexShader::ProcessVertex()
        void ProcessVertex(Vertex &vertex);


    private:

        /// A pointer to the buffer containing the skinning vertex attributes.
        const SkinningVertexAttribute* skinningVertexAttributes;
    };
}

#endif