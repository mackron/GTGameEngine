// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_CPUVertexShader
#define GT_CPUVertexShader

#include "Rendering/VertexFormat.hpp"
#include "Math.hpp"
#include <GTGE/Core/Vector.hpp>
#include <GTGE/Core/Threading.hpp>

namespace GT
{
    /// Base class that can do vertex operations on the CPU.
    ///
    /// A CPU vertex shader takes an input buffer, performs an operation on each vertex, and then places the result into an output buffer.
    ///
    /// This class does not make copies of the buffers. Instead, the application is responsible for making sure the buffers stay valid throughout
    /// the running of the shader. (This may change later.)
    ///
    /// It's important to consider that this class does not operate in exactly the same way as a GPU vertex shader. For starters, there is no
    /// notion of in/out interpolated variables. These shouldn't be needed anyway. Next, if a vertex attribute is not explicitly set, the output
    /// will be set to the input. Thus, if the ProcessVertex() function is left empty it will perform a simple (and inefficient) copy. Don't use
    /// this class for copying...
    class CPUVertexShader
    {
    public:

        /// Default constructor.
        CPUVertexShader();

        /// Destructor.
        virtual ~CPUVertexShader();


        /// Executes the vertex shader.
        ///
        /// @return True if the shader is executed successfully; false otherwise.
        bool Execute(const float* input, size_t vertexCount, const VertexFormat &format, float* output, size_t threadCount = 1);


    // Processing.
    protected:

        /// Sub-class representing a vertex in a vertex shader. These vertex objects are passed to ProcessVertex() for processing. This system will make it
        /// easier to do multi-threaded vertex processing.
        class Vertex
        {
        public:

            Vertex(unsigned int id, float* data, const VertexFormat &format);
           ~Vertex();

            /// Retrieves the ID of the vertex.
            ///
            /// @remarks
            ///     Use this for accessing things like vertex attributes.
            unsigned int GetID() const;

            /// Retrieves a vertex attribute as a vec4/float4.
            ///
            /// @param attribute [in] The index of the vertex attribute to retrieve. E.g. VertexAttribs::Position, VertexAttribute::Normal.
            ///
            /// @return A copy of the vertex attribute as a vec4. See remarks.
            ///
            /// @remarks
            ///     If the vertex attribute is not a vec4, the return value will be padded with 0.0f, except for the w component, which will be set to 1.0f.
            glm::vec4 Get(int attribute);
            
            /// Sets a vertex attribute.
            ///
            /// @remarks
            ///     This should only be called in ProcessVertex().
            void Set(int attribute, float value);
            void Set(int attribute, const glm::vec2 &value);
            void Set(int attribute, const glm::vec3 &value);
            void Set(int attribute, const glm::vec4 &value);


        private:

            /// The ID of the vertex.
            unsigned int id;

            /// A pointer to the vertex's data.
            float* data;

            /// The format of the vertex.
            const VertexFormat &format;


        public:

            // The variables below are for fast-access to common attributes.
            glm::vec4 Position;
            glm::vec4 TexCoord;
            glm::vec4 Normal;
            glm::vec4 Tangent;
            glm::vec4 Bitangent;


        private:    // No copying.
            Vertex(const Vertex &);
            Vertex & operator=(const Vertex &);
        };


        /// A virtual method that will be called at the start of execution.
        virtual void OnStartExecute();

        /// A virtual method that will be called at the end of execution.
        virtual void OnEndExecute();

        /// A virtual method that will be called when a vertex needs to be processed.
        virtual void ProcessVertex(Vertex &vertex) = 0;



    private:

        /// The input buffer.
        const float* m_input;

        /// The number of vertices.
        size_t m_vertexCount;

        /// The format of the input and output buffers.
        VertexFormat m_format;

        /// The size of 'format'. We keep this separate because format.GetSize() runs in linear time.
        size_t m_vertexSizeInFloats;


        /// The output buffer.
        float* m_output;


        /// The variables below control whether or not the various common vertex attributes are used by the vertex shader.
        bool m_usingPosition;
        bool m_usingTexCoord;
        bool m_usingNormal;
        bool m_usingTangent;
        bool m_usingBitangent;

        size_t m_positionComponentCount;
        size_t m_positionOffset;

        size_t m_texCoordComponentCount;
        size_t m_texCoordOffset;

        size_t m_normalComponentCount;
        size_t m_normalOffset;

        size_t m_tangentComponentCount;
        size_t m_tangentOffset;

        size_t m_bitangentComponentCount;
        size_t m_bitangentOffset;


    private:    // No copying.
        CPUVertexShader(const CPUVertexShader &);
        CPUVertexShader & operator=(const CPUVertexShader &);

    friend void ProcessVertexShader(CPUVertexShader &shader, size_t firstVertexID, size_t lastVertexID);
    };
}


#endif
