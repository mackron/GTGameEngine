
#include <GTEngine/CPUVertexShader.hpp>
#include <GTCore/Timing.hpp>

// Benchmarking Notes:
//
// NOTE: Clear these notes when the test data changes.
//
// 16/05/2012 - i7 2600K @ 4.6GHz
//    Debug   (Pure / SSE): ~0.005sec   / ~0.001sec    : +5x
//    Release (Pure / SSE): ~0.00011sec / ~0.000075sec : +1.46x

// Helpers
namespace GTEngine
{
    inline glm::simdVec4 GetVertexAttribute4(const float* data, size_t componentCount, size_t offset)
    {
        if (data != nullptr)
        {
            data += offset;

            // We'll try and be clever here and predict the most common scenarios and put them first.
            if (componentCount == 3)
            {
                return glm::simdVec4(data[0], data[1], data[2], 0.0f);
            }
            else if (componentCount == 2)
            {
                return glm::simdVec4(data[0], data[1], 0.0f, 0.0f);
            }
            else if (componentCount == 4)
            {
                return glm::simdVec4(data[0], data[1], data[2], data[3]);
            }
            else if (componentCount == 1)
            {
                return glm::simdVec4(data[0], 0.0f, 0.0f, 0.0f);
            }
            else
            {
                return glm::simdVec4(0.0f, 0.0f, 0.0f, 0.0f);
            }
        }
        else
        {
            return glm::simdVec4();
        }
    }

    inline void SetVertexAttribute4(float* data, size_t componentCount, size_t offset, const glm::simdVec4 &value)
    {
        if (data != nullptr)
        {
            data += offset;

            // As above, we'll try and be clever here and predict the most common scenarios and put them first.
            /*
            if (componentCount == 3)
            {
                data[0] = value.x;
                data[1] = value.y;
                data[2] = value.z;
            }
            else if (componentCount == 2)
            {
                data[0] = value.x;
                data[1] = value.y;
            }
            else if (componentCount == 4)
            {
                data[0] = value.x;
                data[1] = value.y;
                data[2] = value.z;
                data[3] = value.w;
            }
            else if (componentCount == 1)
            {
                data[0] = value.x;
            }
            */

            
            if (componentCount == 3)
            {
                data[0] = value.Data.m128_f32[0];
                data[1] = value.Data.m128_f32[1];
                data[2] = value.Data.m128_f32[2];
            }
            else if (componentCount == 2)
            {
                data[0] = value.Data.m128_f32[0];
                data[1] = value.Data.m128_f32[1];
            }
            else if (componentCount == 4)
            {
                data[0] = value.Data.m128_f32[0];
                data[1] = value.Data.m128_f32[1];
                data[2] = value.Data.m128_f32[2];
                data[3] = value.Data.m128_f32[3];
            }
            else if (componentCount == 1)
            {
                data[0] = value.Data.m128_f32[0];
            }
            
        }
    }
}


namespace GTEngine
{
    void ProcessVertexShader(CPUVertexShader &shader, size_t firstVertexID, size_t lastVertexID)
    {
        // We need to iterate over each vertex and process it.
        for (size_t i = firstVertexID; i <= lastVertexID; ++i)
        {
            // The first step is to copy the vertex data to the output buffer.
            auto vertexOutput = shader.output + (i * shader.vertexSizeInFloats);
            auto vertexInput  = shader.input  + (i * shader.vertexSizeInFloats);
            //memcpy(vertexOutput, vertexInput, shader.vertexSizeInFloats * sizeof(float));

            // Now we can process the vertex.
            CPUVertexShader::Vertex vertex(i, vertexOutput, shader.format);
            if (shader.usingPosition)  vertex.Position  = GetVertexAttribute4(vertexInput, shader.positionComponentCount,  shader.positionOffset);
            if (shader.usingTexCoord)  vertex.TexCoord  = GetVertexAttribute4(vertexInput, shader.texCoordComponentCount,  shader.texCoordOffset);
            if (shader.usingNormal)    vertex.Normal    = GetVertexAttribute4(vertexInput, shader.normalComponentCount,    shader.normalOffset);
            if (shader.usingTangent)   vertex.Tangent   = GetVertexAttribute4(vertexInput, shader.tangentComponentCount,   shader.tangentOffset);
            if (shader.usingBitangent) vertex.Bitangent = GetVertexAttribute4(vertexInput, shader.bitangentComponentCount, shader.bitangentOffset);

            shader.ProcessVertex(vertex);

            if (shader.usingPosition)  SetVertexAttribute4(vertexOutput, shader.positionComponentCount,  shader.positionOffset,  vertex.Position);
            if (shader.usingTexCoord)  SetVertexAttribute4(vertexOutput, shader.texCoordComponentCount,  shader.texCoordOffset,  vertex.TexCoord);
            if (shader.usingNormal)    SetVertexAttribute4(vertexOutput, shader.normalComponentCount,    shader.normalOffset,    vertex.Normal);
            if (shader.usingTangent)   SetVertexAttribute4(vertexOutput, shader.tangentComponentCount,   shader.tangentOffset,   vertex.Tangent);
            if (shader.usingBitangent) SetVertexAttribute4(vertexOutput, shader.bitangentComponentCount, shader.bitangentOffset, vertex.Bitangent);
        }
    }


    /// Class representing the threading job to execute for processing a chunk of vertices.
    class ProcessVertexShaderJob : public GTCore::Threading::Job
    {
    public:

        /// Constructor.
        /*
        ProcessVertexShaderJob(CPUVertexShader &shader)
            : shader(shader), firstVertexID(0), lastVertexID(0)
        {
        }
        */

        /// Constructor.
        ProcessVertexShaderJob()
            : shader(nullptr), firstVertexID(0), lastVertexID(0)
        {
        }


        /// Sets the range of vertices to execute.
        void SetVertexRange(CPUVertexShader &shader, size_t firstVertexID, size_t lastVertexID)
        {
            this->shader        = &shader;
            this->firstVertexID = firstVertexID;
            this->lastVertexID  = lastVertexID;
        }


        void Run()
        {
            if (this->shader != nullptr)
            {
                ProcessVertexShader(*this->shader, this->firstVertexID, this->lastVertexID);
            }
        }


    private:

        /// A pointer to the shader this job will be working on.
        CPUVertexShader* shader;

        /// The index of the first vertex to work on.
        size_t firstVertexID;

        /// the index of the last vertex to work on.
        size_t lastVertexID;
    };
}


namespace GTEngine
{
    // Temp benchmarker.
    GTCore::Benchmarker benchmarker;

    // This is a temporary list of threads for testing how a pool of threads may help.
    GTCore::Vector<GTCore::Thread*>* ShaderThreadPool = nullptr;
    GTCore::Vector<ProcessVertexShaderJob*>* ShaderJobPool = nullptr;


    CPUVertexShader::CPUVertexShader()
        : input(nullptr), vertexCount(0), format(), vertexSizeInFloats(format.GetSize()), output(nullptr)
    {
        if (ShaderThreadPool == nullptr)
        {
            ShaderThreadPool = new GTCore::Vector<GTCore::Thread*>(8);
            ShaderJobPool    = new GTCore::Vector<ProcessVertexShaderJob*>(8);
            
            for (size_t i = 0; i < 8; ++i)
            {
                ShaderThreadPool->PushBack(new GTCore::Thread);
                ShaderJobPool->PushBack(new ProcessVertexShaderJob);
            }
        }
    }

    CPUVertexShader::~CPUVertexShader()
    {
        // All threads need to be deleted.
        /*
        for (size_t i = 0; i < this->helperThreads.count; ++i)
        {
            delete this->helperThreads[i];
        }
        */
    }


    bool CPUVertexShader::Execute(const float* input, size_t vertexCount, const VertexFormat &format, float* output, size_t threadCount)
    {
        if (input != nullptr && output != nullptr)
        {
            benchmarker.Start();

            this->input              = input;
            this->vertexCount        = vertexCount;
            this->format             = format;
            this->vertexSizeInFloats = this->format.GetSize();
            this->output             = output;

            this->usingPosition  = this->format.GetAttributeInfo(VertexAttribs::Position,  this->positionComponentCount,  this->positionOffset);
            this->usingTexCoord  = this->format.GetAttributeInfo(VertexAttribs::TexCoord,  this->texCoordComponentCount,  this->texCoordOffset);
            this->usingNormal    = this->format.GetAttributeInfo(VertexAttribs::Normal,    this->normalComponentCount,    this->normalOffset);
            this->usingTangent   = this->format.GetAttributeInfo(VertexAttribs::Tangent,   this->tangentComponentCount,   this->tangentOffset);
            this->usingBitangent = this->format.GetAttributeInfo(VertexAttribs::Bitangent, this->bitangentComponentCount, this->bitangentOffset);


            // We only do multithreading if we have enough vertices to warrant it.
            if (this->vertexCount >= 64 && threadCount > 1)
            {
                // What we do here is modify <threadCount> to store the number of helper threads. That is, the number of thread, not including the calling thread.
                --threadCount;

                // If we dont already have enough threads, we need to create them.
                /*
                for (size_t i = this->helperThreads.count; i < threadCount; ++i)
                {
                    this->helperThreads.PushBack(new GTCore::Thread);
                }
                */

                // The list of jobs each corresponding to a helper thread.
                /*
                GTCore::Vector<ProcessVertexShaderJob*> jobs(threadCount);
                for (size_t i = 0; i < threadCount; ++i)
                {
                    jobs.PushBack(new ProcessVertexShaderJob(*this));
                }
                */


                // With the helper threads created, we can now divide up the work and start executing.
                size_t vertexChunkSize = this->vertexCount / (threadCount + 1);
                size_t firstVertexID   = 0;

                for (size_t i = 0; (i < threadCount) && (firstVertexID < this->vertexCount - 1); ++i)
                {
                    //auto thread = this->helperThreads[i];
                    //auto job    = jobs[i];
                    auto thread = ShaderThreadPool->Get(i);
                    auto job    = ShaderJobPool->Get(i);

                    assert(thread != nullptr && job != nullptr);

                    size_t lastVertexID = GTCore::Min(firstVertexID + vertexChunkSize, this->vertexCount - 1);
                    job->SetVertexRange(*this, firstVertexID, lastVertexID);
                        
                    
                    thread->Start(*job, false);     // <-- second argument specifies not to wait for the execution of the current procedure to complete. It will be guaranteed that the thread won't already be running.

                    

                    // We have a new first index.
                    firstVertexID = lastVertexID + 1;
                }


                // We will get the calling thread to process whatever is left over.
                ProcessVertexShader(*this, firstVertexID, this->vertexCount - 1);


                // Now we need to do a cleanup.
                for (size_t i = 0; i < threadCount; ++i)
                {
                    //this->helperThreads[i]->Wait();
                    
                    ShaderThreadPool->Get(i)->Wait();
                    

                    
                    //delete jobs[i];
                }
            }
            else
            {
                // We'll get here if we're not bothering with multithreading.
                ProcessVertexShader(*this, 0, this->vertexCount - 1);
            }



            /*
            // We need to iterate over each vertex and process it.
            for (size_t i = 0; i < this->vertexCount; ++i)
            {
                // The first step is to copy the vertex data to the output buffer.
                auto vertexOutput = this->output + (i * this->vertexSizeInFloats);
                auto vertexInput  = this->input  + (i * this->vertexSizeInFloats);
                //memcpy(vertexOutput, vertexInput, this->vertexSizeInFloats * sizeof(float));

                // Now we can process the vertex.
                Vertex vertex(i, vertexOutput, this->format);
                if (this->usingPosition)  vertex.Position  = GetVertexAttribute4(vertexInput, this->positionComponentCount,  this->positionOffset);
                if (this->usingTexCoord)  vertex.TexCoord  = GetVertexAttribute4(vertexInput, this->texCoordComponentCount,  this->texCoordOffset);
                if (this->usingNormal)    vertex.Normal    = GetVertexAttribute4(vertexInput, this->normalComponentCount,    this->normalOffset);
                if (this->usingTangent)   vertex.Tangent   = GetVertexAttribute4(vertexInput, this->tangentComponentCount,   this->tangentOffset);
                if (this->usingBitangent) vertex.Bitangent = GetVertexAttribute4(vertexInput, this->bitangentComponentCount, this->bitangentOffset);

                this->ProcessVertex(vertex);

                if (this->usingPosition)  SetVertexAttribute4(vertexOutput, this->positionComponentCount,  this->positionOffset,  vertex.Position);
                if (this->usingTexCoord)  SetVertexAttribute4(vertexOutput, this->texCoordComponentCount,  this->texCoordOffset,  vertex.TexCoord);
                if (this->usingNormal)    SetVertexAttribute4(vertexOutput, this->normalComponentCount,    this->normalOffset,    vertex.Normal);
                if (this->usingTangent)   SetVertexAttribute4(vertexOutput, this->tangentComponentCount,   this->tangentOffset,   vertex.Tangent);
                if (this->usingBitangent) SetVertexAttribute4(vertexOutput, this->bitangentComponentCount, this->bitangentOffset, vertex.Bitangent);
            }
            */


            benchmarker.End();

            if (benchmarker.counter == 200)
            {
                printf("CPU Vertex Shader Time: %f\n", static_cast<float>(benchmarker.GetAverageTime()));
                benchmarker.Reset();
            }


            return true;
        }
        
        return false;
    }
}


// CPUVertexShader::Vertex
namespace GTEngine
{
    CPUVertexShader::Vertex::Vertex(unsigned int id, float* data, const VertexFormat &format)
        : id(id), data(data), format(format),
          Position(0.0f, 0.0f, 0.0f, 1.0f),
          TexCoord(),
          Normal(),
          Tangent(),
          Bitangent()
    {
    }

    CPUVertexShader::Vertex::~Vertex()
    {
    }

    unsigned int CPUVertexShader::Vertex::GetID() const
    {
        return this->id;
    }

    glm::vec4 CPUVertexShader::Vertex::Get(int attribute)
    {
        glm::simdVec4 value(0.0f, 0.0f, 0.0f, 1.0f);

        size_t componentCount;
        size_t stride;
        if (this->format.GetAttributeInfo(attribute, componentCount, stride))
        {
            value = GetVertexAttribute4(this->data, componentCount, stride);
        }

        //return glm::vec4(value.x, value.y, value.z, value.w);
        return glm::vec4(value.Data.m128_f32[0], value.Data.m128_f32[1], value.Data.m128_f32[2], value.Data.m128_f32[3]);
    }

    void CPUVertexShader::Vertex::Set(int attribute, float value)
    {
        this->Set(attribute, glm::vec4(value, 0.0f, 0.0f, 1.0f));
    }

    void CPUVertexShader::Vertex::Set(int attribute, const glm::vec2 &value)
    {
        this->Set(attribute, glm::vec4(value, 0.0f, 1.0f));
    }

    void CPUVertexShader::Vertex::Set(int attribute, const glm::vec3 &value)
    {
        this->Set(attribute, glm::vec4(value, 1.0f));
    }

    void CPUVertexShader::Vertex::Set(int attribute, const glm::vec4 &value)
    {
        size_t componentCount;
        size_t stride;
        if (this->format.GetAttributeInfo(attribute, componentCount, stride))
        {
            SetVertexAttribute4(this->data, componentCount, stride, glm::simdVec4(value));
        }
    }
}
