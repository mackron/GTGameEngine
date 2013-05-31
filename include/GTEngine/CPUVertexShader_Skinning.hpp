// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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
    GLM_ALIGN(16) class CPUVertexShader_Skinning : public CPUVertexShader
    {
    public:

        /// Constructor.
        CPUVertexShader_Skinning();

        /// Destructor.
        ~CPUVertexShader_Skinning();


        /// Sets the buffer containing the bones to index into.
        ///
        /// @param bones [in] A buffer containing pointers to bones that the vertex will index into for blending.
        void SetBoneBuffer(const Bone* const* bones);

        /// Sets the pointer to the buffer containing the skinning vertex attributes for each vertex.
        ///
        /// Note that this does not make a copy of the buffer; it simply sets the internal pointer.
        void SetSkinningVertexAttributes(const SkinningVertexAttribute* attributes);

        /// Retrieves the AABB of the vertices that were processed.
        ///
        /// @param aabbMinOut [in] A reference to the variable that will receive the low bounds.
        /// @param aabbMaxOut [in] A reference to the variable that will receive the high bounds.
        ///
        /// @remarks
        ///     This is only valid after the shader has been executed.
        void GetAABB(glm::vec3 &aabbMinOut, glm::vec3 &aabbMaxOut);


    protected:

        /// CPUVertexShader::OnStartExecute()
        void OnStartExecute();

        /// CPUVertexShader::ProcessVertex()
        void ProcessVertex(Vertex &vertex);


    private:

        /// A pointer to the buffer containing the bones to index into.
        const Bone* const* bones;

        /// A pointer to the buffer containing the skinning vertex attributes.
        const SkinningVertexAttribute* skinningVertexAttributes;

        /// The AABB containing each vertex.
        glm::simdVec4 aabbMin;
        glm::simdVec4 aabbMax;
        
    
    private:    // No copying.
        CPUVertexShader_Skinning(const CPUVertexShader_Skinning &);
        CPUVertexShader_Skinning & operator=(const CPUVertexShader_Skinning &);
    };
}

#endif
