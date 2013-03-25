// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/EditorAxisArrows.hpp>

namespace GTEngine
{
    EditorAxisArrows::EditorAxisArrows()
        : xArrowLine(), yArrowLine(), zArrowLine(),
          xArrowHead(), yArrowHead(), zArrowHead(),
          materialDefinition(),
          localPosition(), localScale(1.0f, 1.0f, 1.0f)
    {
        this->materialDefinition.LoadFromXML
        (
            "<material>"
                "<diffuse   id='Material_NoDiffuse' />"
                "<shininess id='Material_NoShininess' />"
                "<emissive  id='Material_SimpleEmissive' />"
            
                "<defaultproperties>"
                    "<float3 name='EmissiveColour' >1.0 1.0 1.0</float3>"
                "</defaultproperties>:"
            "</material>"
        );

        // Materials.
        this->xArrowLine.material = new Material(this->materialDefinition);
        this->yArrowLine.material = new Material(this->materialDefinition);
        this->zArrowLine.material = new Material(this->materialDefinition);
        this->xArrowHead.material = new Material(this->materialDefinition);
        this->yArrowHead.material = new Material(this->materialDefinition);
        this->zArrowHead.material = new Material(this->materialDefinition);

        this->xArrowLine.material->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        this->yArrowLine.material->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        this->zArrowLine.material->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);
        this->xArrowHead.material->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        this->yArrowHead.material->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        this->zArrowHead.material->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);


        // Flags.
        this->xArrowLine.flags = this->yArrowLine.flags = this->zArrowLine.flags = SceneRendererMesh::NoDepthWrite | SceneRendererMesh::NoDepthTest | SceneRendererMesh::DrawLast;
        this->xArrowHead.flags = this->yArrowHead.flags = this->zArrowHead.flags = SceneRendererMesh::NoDepthWrite | SceneRendererMesh::NoDepthTest | SceneRendererMesh::DrawLast;


        // Draw types.
        this->xArrowLine.drawMode = this->yArrowLine.drawMode = this->zArrowLine.drawMode = DrawMode_Lines;


        // Meshes.
        this->xArrowLine.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->yArrowLine.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->zArrowLine.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->xArrowHead.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->yArrowHead.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->zArrowHead.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);


        /// The meshes need to be created properly here.
        this->CreateArrowVertexArray(glm::mat4_cast(glm::angleAxis(-90.0f,  glm::vec3(0.0f, 1.0f, 0.0f))), this->xArrowLine.vertexArray, this->xArrowHead.vertexArray);
        this->CreateArrowVertexArray(glm::mat4_cast(glm::angleAxis( 90.0f,  glm::vec3(1.0f, 0.0f, 0.0f))), this->yArrowLine.vertexArray, this->yArrowHead.vertexArray);
        this->CreateArrowVertexArray(glm::mat4_cast(glm::angleAxis( 180.0f, glm::vec3(1.0f, 0.0f, 0.0f))), this->zArrowLine.vertexArray, this->zArrowHead.vertexArray);
    }


    EditorAxisArrows::~EditorAxisArrows()
    {
        delete this->xArrowLine.material;
        delete this->yArrowLine.material;
        delete this->zArrowLine.material;
        delete this->xArrowHead.material;
        delete this->yArrowHead.material;
        delete this->zArrowHead.material;

        Renderer::DeleteVertexArray(this->xArrowLine.vertexArray);
        Renderer::DeleteVertexArray(this->yArrowLine.vertexArray);
        Renderer::DeleteVertexArray(this->zArrowLine.vertexArray);
        Renderer::DeleteVertexArray(this->xArrowHead.vertexArray);
        Renderer::DeleteVertexArray(this->yArrowHead.vertexArray);
        Renderer::DeleteVertexArray(this->zArrowHead.vertexArray);
    }



    void EditorAxisArrows::Show(SceneRenderer &renderer)
    {
        renderer.AddExternalMesh(this->xArrowLine);
        renderer.AddExternalMesh(this->yArrowLine);
        renderer.AddExternalMesh(this->zArrowLine);
        //renderer.AddExternalMesh(this->xArrowHead);
        //renderer.AddExternalMesh(this->yArrowHead);
        //renderer.AddExternalMesh(this->zArrowHead);
    }

    void EditorAxisArrows::Hide(SceneRenderer &renderer)
    {
        renderer.RemoveExternalMesh(this->xArrowLine);
        renderer.RemoveExternalMesh(this->yArrowLine);
        renderer.RemoveExternalMesh(this->zArrowLine);
        //renderer.RemoveExternalMesh(this->xArrowHead);
        //renderer.RemoveExternalMesh(this->yArrowHead);
        //renderer.RemoveExternalMesh(this->zArrowHead);
    }


    void EditorAxisArrows::UpdateTransform(const SceneNode &camera)
    {
        glm::vec3 cameraPosition    = camera.GetWorldPosition();
        glm::quat cameraOrientation = camera.GetWorldOrientation();


        // Position.
        glm::vec3 position = cameraPosition + (cameraOrientation * this->localPosition);

        // Orientation.
        glm::quat orientation = glm::quat();

        // Scale.
        glm::vec3 scale = localScale;


        glm::mat4 transform;
        Math::CalculateTransformMatrix(position, orientation, scale, transform);

        this->xArrowLine.transform = transform;
        this->yArrowLine.transform = transform;
        this->zArrowLine.transform = transform;
        this->xArrowHead.transform = transform;
        this->yArrowHead.transform = transform;
        this->zArrowHead.transform = transform;
    }


    void EditorAxisArrows::SetLocalPosition(const glm::vec3 &newLocalPosition)
    {
        this->localPosition = newLocalPosition;
    }

    void EditorAxisArrows::SetLocalScale(const glm::vec3 &newLocalScale)
    {
        this->localScale = newLocalScale;
    }


    //////////////////////////////////////////
    // Private

    void EditorAxisArrows::CreateArrowVertexArray(const glm::mat4 &localTransform, VertexArray* lineOut, VertexArray* headOut)
    {
        ////////////////////////////
        // Line

        glm::vec3 arrowLineVertices[2] =
        {
            glm::vec3(localTransform * glm::vec4(0.0f, 0.0f,  0.0f, 1.0f)),
            glm::vec3(localTransform * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f))
        };
        unsigned int arrowLineIndices[2] = {0, 1};

        lineOut->SetData(&arrowLineVertices[0].x, 2, arrowLineIndices, 2);



        ////////////////////////////
        // Head

        float        arrowHeadLength       = 0.3f;
        float        arrowHeadRadius       = 0.075f;
        unsigned int arrowHeadSegments     = 16;
        float        arrowHeadSegmentAngle = glm::radians(360.0f / static_cast<float>(arrowHeadSegments));

        GTCore::Vector<glm::vec3>    arrowHeadVertices;
        GTCore::Vector<unsigned int> arrowHeadIndices;

        arrowHeadVertices.PushBack(glm::vec3(localTransform * glm::vec4(0.0f, 0.0f, -1.0f,                   1.0f)));       // First vertex is the point.
        arrowHeadVertices.PushBack(glm::vec3(localTransform * glm::vec4(0.0f, 0.0f, -1.0f + arrowHeadLength, 1.0f)));       // Second vertex is the center of the base.

        // Now we do the base vertices. There will be arrowHeadSegments vertices on the base.
        for (unsigned int i = 0; i < arrowHeadSegments; ++i)
        {
            glm::vec4 position;

            position.x = std::cos(arrowHeadSegmentAngle * i) * arrowHeadRadius;
            position.y = std::sin(arrowHeadSegmentAngle * i) * arrowHeadRadius;
            position.z = -1.0f + arrowHeadLength;
            position.w = 1.0f;

            arrowHeadVertices.PushBack(glm::vec3(localTransform * position));
        }

        // At this point we'll have all of the arrow head vertices setup. We now need to do the indices. We'll start with the base
        unsigned int pointIndex          = 0;
        unsigned int baseCenterIndex     = 1;
        unsigned int firstOuterBaseIndex = 2;

        for (unsigned int i = 0; i < arrowHeadSegments; ++i)
        {
            arrowHeadIndices.PushBack(baseCenterIndex);
            arrowHeadIndices.PushBack(firstOuterBaseIndex + i);
            arrowHeadIndices.PushBack(firstOuterBaseIndex + ((i + 1) % arrowHeadSegments));

            arrowHeadIndices.PushBack(pointIndex);
            arrowHeadIndices.PushBack(firstOuterBaseIndex + ((i + 1) % arrowHeadSegments));
            arrowHeadIndices.PushBack(firstOuterBaseIndex + i);
        }

        // Now we just set the data in the vertex array.
        headOut->SetData(&arrowHeadVertices[0].x, arrowHeadVertices.count, &arrowHeadIndices[0], arrowHeadIndices.count);
    }
}