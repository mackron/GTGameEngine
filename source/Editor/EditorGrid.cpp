// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Editor/EditorGrid.hpp>

namespace GT
{
    EditorGrid::EditorGrid(float spacing, unsigned int subdivisions, unsigned int lineCount)
        : innerSpacingMesh(), subdivisionMesh(), materialDefinition()
    {
        this->materialDefinition.LoadFromXML
        (
            "<material>"
                "<channel name='emissive' shaderid='Material_SimpleEmissive' />"
            
                "<defaultproperties>"
                    "<float3 name='EmissiveColour'>1.0 1.0 1.0</float3>"
                "</defaultproperties>:"
            "</material>"
        );

        // Materials.
        this->innerSpacingMesh.material = new Material(this->materialDefinition);
        this->innerSpacingMesh.material->SetParameter("EmissiveColour", 0.4f, 0.4f, 0.4f);

        this->subdivisionMesh.material  = new Material(this->materialDefinition);
        this->subdivisionMesh.material->SetParameter("EmissiveColour", 0.25f, 0.25f, 0.25f);


        // Meshes.
        this->innerSpacingMesh.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3);
        this->subdivisionMesh.vertexArray  = Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3);

        this->innerSpacingMesh.drawMode = DrawMode_Lines;
        this->subdivisionMesh.drawMode  = DrawMode_Lines;


        // Flags.
        this->innerSpacingMesh.flags = this->subdivisionMesh.flags = SceneRendererMesh::NoDepthWrite;


        // We just resize to give it an initial state.
        this->Resize(spacing, subdivisions, lineCount);
    }

    EditorGrid::~EditorGrid()
    {
        delete this->innerSpacingMesh.material;
        delete this->subdivisionMesh.material;

        Renderer::DeleteVertexArray(this->innerSpacingMesh.vertexArray);
        Renderer::DeleteVertexArray(this->subdivisionMesh.vertexArray);
    }


    void EditorGrid::Show(SceneRenderer &renderer)
    {
        renderer.AddExternalMesh(this->innerSpacingMesh);
        renderer.AddExternalMesh(this->subdivisionMesh);
    }

    void EditorGrid::Hide(SceneRenderer &renderer)
    {
        renderer.RemoveExternalMesh(this->innerSpacingMesh);
        renderer.RemoveExternalMesh(this->subdivisionMesh);
    }


    void EditorGrid::Resize(float spacing, unsigned int subdivisions, unsigned int lineCount)
    {
        auto subdivisionCount     = lineCount / subdivisions;
        auto halfSubdivisionCount = static_cast<int>(subdivisionCount / 2);
        auto halfLineCount        = static_cast<int>(lineCount / 2);
        auto subdivisionSpacing   = spacing * subdivisions;

        float leftEdge   = -halfLineCount * spacing;
        float rightEdge  =  halfLineCount * spacing;
        float topEdge    = -halfLineCount * spacing;
        float bottomEdge =  halfLineCount * spacing;


        Vector<glm::vec3> innerSpacingVertices;
        Vector<glm::vec3> subdivisionVertices;
        
        Vector<unsigned int> innerSpacingIndices;
        Vector<unsigned int> subdivisionIndices;



        /////////////////////////////////////////
        // Main lines.

        // Horizontal, origin -> right.
        float xPos = 0.0f;
        for (int i = 0; i < halfLineCount; ++i)
        {
            glm::vec3 topVertex    = glm::vec3(xPos, 0.0f, topEdge);
            glm::vec3 bottomVertex = glm::vec3(xPos, 0.0f, bottomEdge);

            innerSpacingVertices.PushBack(topVertex);
            innerSpacingIndices.PushBack(static_cast<unsigned int>(innerSpacingVertices.count) - 1);

            innerSpacingVertices.PushBack(bottomVertex);
            innerSpacingIndices.PushBack(static_cast<unsigned int>(innerSpacingVertices.count) - 1);
            

            xPos += spacing;
        }


        // Horizontal, left <- origin
        xPos = 0.0f;
        for (int i = 0; i < halfLineCount; ++i)
        {
            glm::vec3 topVertex    = glm::vec3(xPos, 0.0f, topEdge);
            glm::vec3 bottomVertex = glm::vec3(xPos, 0.0f, bottomEdge);

            innerSpacingVertices.PushBack(topVertex);
            innerSpacingIndices.PushBack(static_cast<unsigned int>(innerSpacingVertices.count) - 1);

            innerSpacingVertices.PushBack(bottomVertex);
            innerSpacingIndices.PushBack(static_cast<unsigned int>(innerSpacingVertices.count) - 1);


            xPos -= spacing;
        }


        // Vertical, origin -> bottom.
        float zPos = 0.0f;
        for (int i = 0; i < halfLineCount; ++i)
        {
            glm::vec3 topVertex    = glm::vec3(leftEdge,  0.0f, zPos);
            glm::vec3 bottomVertex = glm::vec3(rightEdge, 0.0f, zPos);

            innerSpacingVertices.PushBack(topVertex);
            innerSpacingIndices.PushBack(static_cast<unsigned int>(innerSpacingVertices.count) - 1);

            innerSpacingVertices.PushBack(bottomVertex);
            innerSpacingIndices.PushBack(static_cast<unsigned int>(innerSpacingVertices.count) - 1);
            

            zPos += spacing;
        }


        // Vertical, top <- origin
        zPos = 0.0f;
        for (int i = 0; i < halfLineCount; ++i)
        {
            glm::vec3 topVertex    = glm::vec3(leftEdge,  0.0f, zPos);
            glm::vec3 bottomVertex = glm::vec3(rightEdge, 0.0f, zPos);

            innerSpacingVertices.PushBack(topVertex);
            innerSpacingIndices.PushBack(static_cast<unsigned int>(innerSpacingVertices.count) - 1);

            innerSpacingVertices.PushBack(bottomVertex);
            innerSpacingIndices.PushBack(static_cast<unsigned int>(innerSpacingVertices.count) - 1);
            

            zPos -= spacing;
        }




        /////////////////////////////////////////
        // Darker lines.

        // Origin
        subdivisionVertices.PushBack(glm::vec3(leftEdge,  0.0f, 0.0f));
        subdivisionVertices.PushBack(glm::vec3(rightEdge, 0.0f, 0.0f));
        subdivisionVertices.PushBack(glm::vec3(0.0f,      0.0f, topEdge));
        subdivisionVertices.PushBack(glm::vec3(0.0f,      0.0f, bottomEdge));

        subdivisionIndices.PushBack(0);
        subdivisionIndices.PushBack(1);
        subdivisionIndices.PushBack(2);
        subdivisionIndices.PushBack(3);

        // Border.
        subdivisionVertices.PushBack(glm::vec3(leftEdge,  0.0f, bottomEdge));
        subdivisionVertices.PushBack(glm::vec3(rightEdge, 0.0f, bottomEdge));
        subdivisionVertices.PushBack(glm::vec3(rightEdge, 0.0f, topEdge));
        subdivisionVertices.PushBack(glm::vec3(leftEdge,  0.0f, topEdge));

        subdivisionIndices.PushBack(4);
        subdivisionIndices.PushBack(5);
        subdivisionIndices.PushBack(5);
        subdivisionIndices.PushBack(6);
        subdivisionIndices.PushBack(6);
        subdivisionIndices.PushBack(7);
        subdivisionIndices.PushBack(7);
        subdivisionIndices.PushBack(4);


        // Horizontal, origin -> right.
        xPos = 0.0f;
        for (int i = 0; i < halfSubdivisionCount; ++i)
        {
            glm::vec3 topVertex    = glm::vec3(xPos, 0.0f, topEdge);
            glm::vec3 bottomVertex = glm::vec3(xPos, 0.0f, bottomEdge);

            subdivisionVertices.PushBack(topVertex);
            subdivisionIndices.PushBack(static_cast<unsigned int>(subdivisionVertices.count) - 1);

            subdivisionVertices.PushBack(bottomVertex);
            subdivisionIndices.PushBack(static_cast<unsigned int>(subdivisionVertices.count) - 1);
            

            xPos += subdivisionSpacing;
        }


        // Horizontal, left <- origin
        xPos = 0.0f;
        for (int i = 0; i < halfSubdivisionCount; ++i)
        {
            glm::vec3 topVertex    = glm::vec3(xPos, 0.0f, topEdge);
            glm::vec3 bottomVertex = glm::vec3(xPos, 0.0f, bottomEdge);

            subdivisionVertices.PushBack(topVertex);
            subdivisionIndices.PushBack(static_cast<unsigned int>(subdivisionVertices.count) - 1);

            subdivisionVertices.PushBack(bottomVertex);
            subdivisionIndices.PushBack(static_cast<unsigned int>(subdivisionVertices.count) - 1);


            xPos -= subdivisionSpacing;
        }


        // Vertical, origin -> bottom.
        zPos = 0.0f;
        for (int i = 0; i < halfSubdivisionCount; ++i)
        {
            glm::vec3 topVertex    = glm::vec3(leftEdge,  0.0f, zPos);
            glm::vec3 bottomVertex = glm::vec3(rightEdge, 0.0f, zPos);

            subdivisionVertices.PushBack(topVertex);
            subdivisionIndices.PushBack(static_cast<unsigned int>(subdivisionVertices.count) - 1);

            subdivisionVertices.PushBack(bottomVertex);
            subdivisionIndices.PushBack(static_cast<unsigned int>(subdivisionVertices.count) - 1);
            

            zPos += subdivisionSpacing;
        }


        // Vertical, top <- origin
        zPos = 0.0f;
        for (int i = 0; i < halfSubdivisionCount; ++i)
        {
            glm::vec3 topVertex    = glm::vec3(leftEdge,  0.0f, zPos);
            glm::vec3 bottomVertex = glm::vec3(rightEdge, 0.0f, zPos);

            subdivisionVertices.PushBack(topVertex);
            subdivisionIndices.PushBack(static_cast<unsigned int>(subdivisionVertices.count) - 1);

            subdivisionVertices.PushBack(bottomVertex);
            subdivisionIndices.PushBack(static_cast<unsigned int>(subdivisionVertices.count) - 1);
            

            zPos -= subdivisionSpacing;
        }


        

        // Now all we need to do is give the data to the vertex arrays.
        this->innerSpacingMesh.vertexArray->SetData(&innerSpacingVertices[0].x, innerSpacingVertices.count, &innerSpacingIndices[0], innerSpacingIndices.count);
        this->subdivisionMesh.vertexArray->SetData(&subdivisionVertices[0].x, subdivisionVertices.count, &subdivisionIndices[0], subdivisionIndices.count);
    }
}