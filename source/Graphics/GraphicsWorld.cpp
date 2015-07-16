// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsWorld.hpp>
//#include <GTGameEngine/Graphics/GraphicsInterface.hpp>
//#include "OpenGL/OpenGL4/GPURenderingDevice_OpenGL4.hpp"

//#include <cstdio>

namespace GT
{
    GraphicsWorld::GraphicsWorld(GUIContext &gui)
        : m_gui(gui)
    {
    }

    GraphicsWorld::~GraphicsWorld()
    {
    }


    GUIContext & GraphicsWorld::GetGUI() const
    {
        return m_gui;
    }


#if 0
    GraphicsWorld::GraphicsWorld(GraphicsInterface &graphicsInterface)
        : m_graphicsInterface(graphicsInterface)
    {
    }

    GraphicsWorld::~GraphicsWorld()
    {
    }


    bool GraphicsWorld::Startup()
    {
        auto &graphicsInterfaceGL = reinterpret_cast<GPURenderingDevice_OpenGL4 &>(m_graphicsInterface);


        // Vertex Shader
        const char* vertexShaderSrc =
            "in vec3 VS_Position;\n"
            "in vec2 VS_TexCoord;\n"
            "in vec3 VS_Normal;\n"
            "out vec2 FS_TexCoord;\n"
            "out vec3 FS_Normal;\n"
            "layout(binding = 0, std140) uniform GlobalBlock\n"
            "{\n"
            "    mat4 Projection;\n"
            "    mat4 View;\n"
            "};\n"
            "layout(binding = 1, std140) uniform PerObjectBlock\n"
            "{\n"
            "    mat4 Model;\n"
            "};\n"
            "\n"
            "void main()\n"
            "{\n"
            "    FS_TexCoord = VS_TexCoord;\n"
            "    FS_Normal   = VS_Normal;\n"        // <-- This is incorrect. Need to multiply this by the normal matrix.
            ""
            "    gl_Position = Projection * View * Model * vec4(VS_Position, 1.0);\n"
            "}";

        GT::BasicBuffer vertexShaderCode;
        GT::BasicBuffer vertexMessages;
        graphicsInterfaceGL.CompileShader(vertexShaderSrc, 0, nullptr, GT::ShaderLanguage_GLSL_420, GT::ShaderType_Vertex, vertexShaderCode, vertexMessages);

        if (vertexMessages.GetDataSizeInBytes() > 1)
        {
            printf("--- Vertex Shader ---\n");
            printf("%s\n", reinterpret_cast<const char*>(vertexMessages.GetDataPointer()));

            return false;
        }

        m_hVertexShader = graphicsInterfaceGL.CreateShader(vertexShaderCode.GetDataPointer(), vertexShaderCode.GetDataSizeInBytes(), GT::ShaderType_Vertex);



        // Fragment Shader
        const char* fragmentShaderSrc =
            "in vec2 FS_TexCoord;\n"
            "in vec3 FS_Normal;\n"
            "layout(location = 0) out vec4 FS_ColorOut;"
            //"layout(binding = 0) uniform sampler2D g_Sampler;"
            "void main()\n"
            "{\n"
            //"    FS_ColorOut = texture(g_Sampler, FS_TexCoord);"
            //"    FS_ColorOut = vec4(1.0, 1.0, 1.0, 1.0);"
            "    FS_ColorOut = vec4(FS_Normal, 1.0);\n"
            "}";

        GT::BasicBuffer fragmentShaderCode;
        GT::BasicBuffer fragmentMessages;
        graphicsInterfaceGL.CompileShader(fragmentShaderSrc, 0, nullptr, GT::ShaderLanguage_GLSL_420, GT::ShaderType_Fragment, fragmentShaderCode, fragmentMessages);

        if (fragmentMessages.GetDataSizeInBytes() > 1)
        {
            printf("--- Fragment Shader ---\n");
            printf("%s\n", reinterpret_cast<const char*>(fragmentMessages.GetDataPointer()));
            
            return false;
        }

        m_hFragmentShader = graphicsInterfaceGL.CreateShader(fragmentShaderCode.GetDataPointer(), fragmentShaderCode.GetDataSizeInBytes(), GT::ShaderType_Fragment);



        // Temporary vertex input layout.
        GT::VSInputAttribFormat attribDesc[3];
        attribDesc[0] = {0, "VS_Position", GT::VertexAttribFormat_Float, 3, sizeof(float)*0, 0};
        attribDesc[1] = {0, "VS_TexCoord", GT::VertexAttribFormat_Float, 2, sizeof(float)*3, 0};
        attribDesc[2] = {0, "VS_Normal",   GT::VertexAttribFormat_Float, 3, sizeof(float)*5, 0};

        size_t attribDescCount = sizeof(attribDesc) / sizeof(GT::VSInputAttribFormat);
        m_hVertexLayout = graphicsInterfaceGL.CreateVSInputLayout(m_hVertexShader, attribDesc, attribDescCount);
        if (m_hVertexLayout != 0)
        {
            graphicsInterfaceGL.IASetInputLayout(m_hVertexLayout);
        }
        else
        {
            printf("Failed to create input layout\n");
            return false;
        }



        // Constant buffers.
        m_hCBGlobalBlock = graphicsInterfaceGL.CreateBuffer(GT::BufferType_Constant, GT::BufferUsage_Dynamic, GT::BufferCPUAccess_Write, sizeof(CBGlobalBlock), nullptr);
        m_hCBObjectBlock = graphicsInterfaceGL.CreateBuffer(GT::BufferType_Constant, GT::BufferUsage_Dynamic, GT::BufferCPUAccess_Write, sizeof(CBObjectBlock), nullptr);



        return true;
    }

    void GraphicsWorld::Shutdown()
    {
        auto &graphicsInterfaceGL = reinterpret_cast<GPURenderingDevice_OpenGL4 &>(m_graphicsInterface);

        graphicsInterfaceGL.DeleteShader(m_hVertexShader);
        graphicsInterfaceGL.DeleteShader(m_hFragmentShader);
    }


    void GraphicsWorld::Render(const mat4 &projection, const mat4 &view, HFramebuffer hFramebuffer, const GraphicsViewport &viewport)
    {
        auto &graphicsInterfaceGL = reinterpret_cast<GPURenderingDevice_OpenGL4 &>(m_graphicsInterface);


        graphicsInterfaceGL.BindFramebuffer(hFramebuffer);
        graphicsInterfaceGL.RSSetViewports(&viewport, 1);


        // Update the global constant buffer once right at the start.
        mat4* cb0Ptr = reinterpret_cast<mat4*>(graphicsInterfaceGL.MapBuffer(m_hCBGlobalBlock, GT::BufferMapType_Write_Discard));
        if (cb0Ptr != nullptr)
        {
            cb0Ptr[0] = projection;
            cb0Ptr[1] = view;

            graphicsInterfaceGL.UnmapBuffer(m_hCBGlobalBlock);
        }

        graphicsInterfaceGL.BindConstantBuffer(0, m_hCBGlobalBlock);




        // For now we're using the same shader for each mesh.
        graphicsInterfaceGL.SetVertexShader(m_hVertexShader);
        graphicsInterfaceGL.SetFragmentShader(m_hFragmentShader);

        for (size_t iMesh = 0; iMesh < m_meshes.GetCount(); ++iMesh)
        {
            auto pMesh = m_meshes[iMesh];
            assert(pMesh != nullptr);
            {
                auto pMeshDesc = reinterpret_cast<MeshDesc*>(pMesh->GetDescriptor());
                assert(pMeshDesc != nullptr);
                {
                    //graphicsInterfaceGL.IASetPrimitiveTopology(pMeshDesc->topology);

                    // TODO: Delete these two lines later and replace with correct versions based on the mesh descriptor.
                    graphicsInterfaceGL.IASetPrimitiveTopology(PrimitiveTopology_Triangle);
                    graphicsInterfaceGL.IASetInputLayout(m_hVertexLayout);


                    // Update the per-object constant buffer.
                    mat4* cb1Ptr = reinterpret_cast<mat4*>(graphicsInterfaceGL.MapBuffer(m_hCBObjectBlock, GT::BufferMapType_Write_Discard));
                    if (cb1Ptr != nullptr)
                    {
                        cb1Ptr[0] = pMesh->CalculateTransformMatrix();

                        graphicsInterfaceGL.UnmapBuffer(m_hCBObjectBlock);
                    }
                    graphicsInterfaceGL.BindConstantBuffer(1, m_hCBObjectBlock);



                    graphicsInterfaceGL.IASetVertexBuffer(0, pMeshDesc->hVertexBuffer, pMeshDesc->vertexStride, 0);
                    graphicsInterfaceGL.IASetIndexBuffer(pMeshDesc->hIndexBuffer, pMeshDesc->indexFormat, 0);

                    graphicsInterfaceGL.Draw(pMeshDesc->indexCount, 0, 0);
                }
            }
        }
    }


    HGraphicsWorldMeshDescriptor GraphicsWorld::CreateMeshDescriptor(const GraphicsWorldMeshDesc &descIn)
    {
        auto &graphicsInterfaceGL = reinterpret_cast<GPURenderingDevice_OpenGL4 &>(m_graphicsInterface);


        if (descIn.pVertexData != nullptr && descIn.vertexDataSize > 0 &&
            descIn.pIndexData  != nullptr && descIn.indexDataSize  > 0)
        {
            HBuffer hVertexBuffer = graphicsInterfaceGL.CreateBuffer(BufferType_Vertex, BufferUsage_Immutable, BufferCPUAccess_None, descIn.vertexDataSize, descIn.pVertexData);
            if (hVertexBuffer != 0)
            {
                HBuffer hIndexBuffer = graphicsInterfaceGL.CreateBuffer(BufferType_Index, BufferUsage_Immutable, BufferCPUAccess_None, descIn.indexDataSize, descIn.pIndexData);
                if (hIndexBuffer != 0)
                {
                    // TODO: Handle the vertex attribute formats.

                    MeshDesc* pDesc = new MeshDesc;
                    pDesc->topology      = descIn.topology;
                    pDesc->hVertexBuffer = hVertexBuffer;
                    pDesc->hIndexBuffer  = hIndexBuffer;
                    pDesc->vertexStride  = descIn.vertexStride;
                    pDesc->indexCount    = descIn.indexCount;
                    pDesc->indexFormat   = descIn.indexFormat;
                    return reinterpret_cast<HGraphicsWorldMeshDescriptor>(pDesc);
                }
                else
                {
                    // Failed to create the index buffer.
                    graphicsInterfaceGL.DeleteBuffer(hVertexBuffer);
                    return 0;
                }
            }
            else
            {
                // Failed to create the vertex buffer.
                return 0;
            }
        }
        else
        {
            // No data for either vertices or indices.
            return 0;
        }
    }

    void GraphicsWorld::DeleteMeshDescriptor(HGraphicsWorldMeshDescriptor hDesc)
    {
        auto &graphicsInterfaceGL = reinterpret_cast<GPURenderingDevice_OpenGL4 &>(m_graphicsInterface);


        auto pDesc = reinterpret_cast<MeshDesc*>(hDesc);
        if (pDesc != nullptr)
        {
            graphicsInterfaceGL.DeleteBuffer(pDesc->hIndexBuffer);
            graphicsInterfaceGL.DeleteBuffer(pDesc->hVertexBuffer);

            delete pDesc;
        }
    }


    GraphicsWorldObject* GraphicsWorld::CreateMesh(HGraphicsWorldMeshDescriptor hDescriptor, const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        GraphicsWorldMesh* pMesh = new GraphicsWorldMesh(hDescriptor);
        pMesh->SetPosition(position);
        pMesh->SetRotation(rotation);
        pMesh->SetScale(scale);

        m_meshes.PushBack(pMesh);

        return pMesh;
    }

    void GraphicsWorld::DeleteObject(GraphicsWorldObject* pObject)
    {
        if (pObject != nullptr)
        {
            switch (pObject->GetType())
            {
            case GraphicsWorldObjectType::Mesh:
                {
                    GraphicsWorldMesh* pMesh = reinterpret_cast<GraphicsWorldMesh*>(pObject);
                    m_meshes.RemoveFirstOccuranceOf(pMesh);

                    break;
                }

            default:
                {
                    break;
                }
            }


            delete pObject;
        }
    }
#endif
}