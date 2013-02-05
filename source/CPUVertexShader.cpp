// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/CPUVertexShader.hpp>
#include <GTEngine/ThreadCache.hpp>
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

        /// Default constructor.
        ProcessVertexShaderJob()
            : shader(nullptr), firstVertexID(0), lastVertexID(0)
        {
        }

        /// Constructor.
        ProcessVertexShaderJob(CPUVertexShader &shader, size_t firstVertexID, size_t lastVertexID)
            : shader(&shader), firstVertexID(firstVertexID), lastVertexID(lastVertexID)
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
        
        
    private:    // No copying.
        ProcessVertexShaderJob(const ProcessVertexShaderJob &);
        ProcessVertexShaderJob & operator=(const ProcessVertexShaderJob &);
    };
}


namespace GTEngine
{
    // Temp benchmarker.
    GTCore::Benchmarker benchmarker;

    CPUVertexShader::CPUVertexShader()
        : input(nullptr), vertexCount(0), format(), vertexSizeInFloats(format.GetSize()), output(nullptr),
          usingPosition(false), usingTexCoord(false), usingNormal(false), usingTangent(false), usingBitangent(false),
          positionComponentCount(0), positionOffset(0),
          texCoordComponentCount(0), texCoordOffset(0),
          normalComponentCount(0), normalOffset(0),
          tangentComponentCount(0), tangentOffset(0),
          bitangentComponentCount(0), bitangentOffset(0)
    {
    }

    CPUVertexShader::~CPUVertexShader()
    {
    }


    bool CPUVertexShader::Execute(const float* input, size_t vertexCount, const VertexFormat &format, float* output, size_t threadCount)
    {
        if (input != nullptr && output != nullptr)
        {
            //benchmarker.Start();

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

                // This is a temporary buffer containing the threads we've acquired.
                GTCore::Thread** threads      = new GTCore::Thread*[threadCount];
                ProcessVertexShaderJob** jobs = new ProcessVertexShaderJob*[threadCount];


                // First we will grab as many threads as we can.
                for (size_t i = 0; i < threadCount; ++i)
                {
                    auto thread = ThreadCache::AcquireThread();
                    if (thread != nullptr)
                    {
                        // We need to keep track of this thread so we can sync and unacquire afterwards.
                        threads[i] = thread;

                        // Now we need a job. We need to keep track of this job so we can delete it later.
                        auto job = new ProcessVertexShaderJob;
                        jobs[i] = job;
                    }
                    else
                    {
                        // If we get here there were not enough available threads. Thus, we will have a new thread count and we need to break and give the remaining vertices to the calling thread.
                        threadCount = i;
                        break;
                    }
                }


                // With the helper threads created, we can now divide up the work and start executing.
                size_t vertexChunkSize = this->vertexCount / (threadCount + 1);
                size_t firstVertexID   = 0;

                for (size_t i = 0; (i < threadCount) && (firstVertexID < this->vertexCount - 1); ++i)
                {
                    auto thread = threads[i];
                    auto job    = jobs[i];

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
                    threads[i]->Wait();
                    ThreadCache::UnacquireThread(threads[i]);
                    
                    delete jobs[i];
                }

                delete [] jobs;
                delete [] threads;
            }
            else
            {
                // We'll get here if we're not bothering with multithreading.
                ProcessVertexShader(*this, 0, this->vertexCount - 1);
            }


            /*
            benchmarker.End();

            if (benchmarker.counter == 200)
            {
                printf("CPU Vertex Shader Time: %f\n", static_cast<float>(benchmarker.GetAverageTime()));
                benchmarker.Reset();
            }
            */

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
        return glm::vec4(value.x, value.y, value.z, value.w);
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
