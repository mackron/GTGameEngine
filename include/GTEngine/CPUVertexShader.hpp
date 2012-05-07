
#ifndef __GTEngine_CPUVertexShader_hpp_
#define __GTEngine_CPUVertexShader_hpp_

#include "Rendering/VertexFormat.hpp"
#include "Math.hpp"

namespace GTEngine
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

        /// Constructor for initialising in-place.
        CPUVertexShader(const float* input, size_t vertexCount, const VertexFormat &format, float* output);

        /// Destructor.
        virtual ~CPUVertexShader();


        /// Initialises the vertex shader.
        bool Initialise(const float* input, size_t vertexCount, const VertexFormat &format, float* output);

        /// Executes the vertex shader.
        ///
        /// @return True if the shader is executed successfully; false otherwise.
        bool Execute();


        




    // Processing.
    public:

        /// A virtual method that will be called when a vertex needs to be processed.
        virtual void ProcessVertex() = 0;

        /// Retrieves a vertex attribute as a vec4/float4.
        ///
        /// @param attribute [in] The index of the vertex attribute to retrieve. E.g. VertexAttribs::Position, VertexAttribute::Normal.
        ///
        /// @return A copy of the vertex attribute as a vec4. See remarks.
        ///
        /// @remarks
        ///     If the vertex attribute is not a vec4, the return value will be padded with 0.0f, except for the w component, which will be set to 1.0f.
        ///     @par
        ///     This should only be called in ProcessVertex().
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

        /// The input buffer.
        const float* input;

        /// The number of vertices.
        size_t vertexCount;

        /// The format of the input and output buffers.
        VertexFormat format;

        /// The size of 'format'. We keep this separate because format.GetSize() runs in linear time.
        size_t vertexSizeInFloats;


        /// The output buffer.
        float* output;


        /// The index of the vertex currently being processed.
        size_t currentVertexIndex;
    };
}


#endif