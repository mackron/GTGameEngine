// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/Rendering/ElementRenderingData.hpp>
#include <GTEngine/GUI/Element.hpp>
#include <GTEngine/GUI/Server.hpp>
#include <GTEngine/Core/TextManager.hpp>
#include <cmath>


namespace GTGUI
{
    // Some math stuff, mainly for rounded corners.
    static const float pi = 3.14159265f;

    float radians(float degrees)
    {
        return degrees * (pi / 180.0f);
    }



    ElementRenderingData::ElementRenderingData(GarbageCollector &gcIn)
        : gc(gcIn),
          textMeshes(), shadowMesh(nullptr), borderMesh(nullptr), backgroundMesh(nullptr), backgroundImageMesh(nullptr),
          absoluteOpacity(1.0f)
    {
    }

    ElementRenderingData::~ElementRenderingData()
    {
        this->DeleteTextMeshes();
        this->DeleteShadowMesh();
        this->DeleteBorderMesh();
        this->DeleteBackgroundMeshes();
    }


    void ElementRenderingData::UpdateTextMeshes(const Element &element)
    {
        this->DeleteTextMeshes();

        if (element.HasText())
        {
            // First we use GTType to render the info into it's own format.
            GTLib::TextManagerRenderingOptions options;
            options.alpha = element.GetAbsoluteOpacity();

            GTLib::TextManagerRenderingInfo renderingInfo;
            element.textManager.Render(options, renderingInfo);


            // Now we take the info generated by GTType and convert it to Mesh objects. Conveniently, GTType uses the same vertex format as us which means we can just use a simple cast.
            for (auto iMesh = renderingInfo.meshes.root; iMesh != nullptr; iMesh = iMesh->next)
            {
                auto meshIn = iMesh->value;
                assert(meshIn != nullptr);
                
                auto vertices    = reinterpret_cast<const MeshVertex*>(meshIn->GetVertices());
                auto vertexCount = meshIn->GetVertexCount();
                auto indices     = meshIn->GetIndices();
                auto indexCount  = meshIn->GetIndexCount();
                auto glyphMap    = meshIn->GetGlyphMapHandle();     // <-- In GTGUI, a GTLib::GlyphMapHandle is just a GTLib::Image*. This can be null.

                auto mesh = new Mesh(vertices, vertexCount, indices, indexCount, glyphMap);
                this->textMeshes.PushBack(mesh);
            }
        }
    }

    void ElementRenderingData::UpdateTextMeshesColour(const GTLib::Colour &colour)
    {
        for (size_t i = 0; i < this->textMeshes.count; ++i)
        {
            this->textMeshes[i]->SetVertexColours(colour);
        }
    }


    void ElementRenderingData::UpdateShadowMesh(const Element &element)
    {
        this->DeleteShadowMesh();

        if (element.style.enableShadow->value)
        {
            // For the shadow to look OK, we will need to give it rounded corners.
            //
            // There are two rectangles for each shadow. The inner rectangle is a solid colour. The outer rectangle is a transparent colour. The colours will
            // be interpolated to give a pseudo blur effect.
            GTLib::Vector<MeshVertex>   vertices;
            GTLib::Vector<unsigned int> indices;

            float radius = element.style.shadowBlurRadius->value;

            float shadowExtrusionX = element.style.shadowExtrusionX->value;
            float shadowExtrusionY = element.style.shadowExtrusionY->value;

            float shadowOffsetX = element.style.shadowOffsetX->value;
            float shadowOffsetY = element.style.shadowOffsetY->value;

            float shadowInnerLeft   = 0.0f                                    - (shadowExtrusionX * 0.5f) + radius + shadowOffsetX;
            float shadowInnerRight  = static_cast<float>(element.GetWidth())  + (shadowExtrusionX * 0.5f) - radius + shadowOffsetX;
            float shadowInnerTop    = 0.0f                                    - (shadowExtrusionY * 0.5f) + radius + shadowOffsetY;
            float shadowInnerBottom = static_cast<float>(element.GetHeight()) + (shadowExtrusionY * 0.5f) - radius + shadowOffsetY;

            

            MeshVertex vertex;
            vertex.texCoordX = 0.0f; vertex.texCoordY = 0.0f;

            auto &colour  = element.style.shadowColour->value;
            auto  opacity = element.GetAbsoluteShadowOpacity();


            

            // We will now construct the inner rectangle.
            //
            // 3     2
            //
            // 0     1
            //
            vertex.colourR = colour.r; vertex.colourG = colour.g; vertex.colourB = colour.b; vertex.colourA = opacity;

            // 0 - Left/Bottom
            vertex.positionX = shadowInnerLeft;  vertex.positionY = shadowInnerBottom;
            vertices.PushBack(vertex);

            // 1 - Right/Bottom
            vertex.positionX = shadowInnerRight; vertex.positionY = shadowInnerBottom;
            vertices.PushBack(vertex);

            // 2 - Right/Top
            vertex.positionX = shadowInnerRight; vertex.positionY = shadowInnerTop;
            vertices.PushBack(vertex);

            // 3 - Left/Top
            vertex.positionX = shadowInnerLeft;  vertex.positionY = shadowInnerTop;
            vertices.PushBack(vertex);


            // Inner rectangle indices.
            indices.PushBack(0);
            indices.PushBack(1);
            indices.PushBack(2);
            indices.PushBack(2);
            indices.PushBack(3);
            indices.PushBack(0);


            // Now we will do the outer part. This is the part that will have rounded corners. For each corner, we start with the rounded part, and then
            // move counter-clockwise.
            //
            // We only need to do this if we actually have a radius > 0.0.
            if (radius > 0.0f)
            {
                int   segmentsPerCorner   = (static_cast<int>(2.0f * pi * radius) / 4) + 1;
                float segmentAngleRadians = radians(90.0f / static_cast<float>(segmentsPerCorner));

                const unsigned int firstOuterIndex = 4;
                      unsigned int lastOuterIndex  = 4;
                


                // we can set the colour once.
                vertex.colourA = colour.r; vertex.colourG = colour.g; vertex.colourB = colour.b; vertex.colourA = 0.0f;


                // Left/Bottom Corner
                vertex.positionX = shadowInnerLeft - radius;
                vertex.positionY = shadowInnerBottom;
                vertices.PushBack(vertex);

                for (int i = 1; i <= segmentsPerCorner; ++i)
                {
                    vertex.positionX = shadowInnerLeft   + std::cos(radians(180.0f) + (segmentAngleRadians * static_cast<float>(i))) * radius;
                    vertex.positionY = shadowInnerBottom - std::sin(radians(180.0f) + (segmentAngleRadians * static_cast<float>(i))) * radius;
                    vertices.PushBack(vertex);

                    indices.PushBack(0);                    // Left/Bottom inner corner index.
                    indices.PushBack(lastOuterIndex++);
                    indices.PushBack(lastOuterIndex);
                }


                // Bottom Bridge
                vertex.positionX = shadowInnerRight;
                vertex.positionY = shadowInnerBottom + radius;
                vertices.PushBack(vertex);
                
                indices.PushBack(0);
                indices.PushBack(lastOuterIndex++);
                indices.PushBack(lastOuterIndex);
                indices.PushBack(lastOuterIndex);
                indices.PushBack(1);
                indices.PushBack(0);


                
                // Right/Bottom Corner
                for (int i = 1; i <= segmentsPerCorner; ++i)
                {
                    vertex.positionX = shadowInnerRight  + std::cos(radians(270.0f) + (segmentAngleRadians * static_cast<float>(i))) * radius;
                    vertex.positionY = shadowInnerBottom - std::sin(radians(270.0f) + (segmentAngleRadians * static_cast<float>(i))) * radius;
                    vertices.PushBack(vertex);

                    indices.PushBack(1);                    // Right/Bottom inner corner index.
                    indices.PushBack(lastOuterIndex++);
                    indices.PushBack(lastOuterIndex);
                }


                // Right Bridge
                vertex.positionX = shadowInnerRight + radius;
                vertex.positionY = shadowInnerTop;
                vertices.PushBack(vertex);

                indices.PushBack(1);
                indices.PushBack(lastOuterIndex++);
                indices.PushBack(lastOuterIndex);
                indices.PushBack(lastOuterIndex);
                indices.PushBack(2);
                indices.PushBack(1);



                // Right/Top
                for (int i = 1; i <= segmentsPerCorner; ++i)
                {
                    vertex.positionX = shadowInnerRight + std::cos(radians(0.0f) + (segmentAngleRadians * static_cast<float>(i))) * radius;
                    vertex.positionY = shadowInnerTop   - std::sin(radians(0.0f) + (segmentAngleRadians * static_cast<float>(i))) * radius;
                    vertices.PushBack(vertex);

                    indices.PushBack(2);                    // Right/Top inner corner index.
                    indices.PushBack(lastOuterIndex++);
                    indices.PushBack(lastOuterIndex);
                }

                
                // Top Bridge
                vertex.positionX = shadowInnerLeft;
                vertex.positionY = shadowInnerTop - radius;
                vertices.PushBack(vertex);

                indices.PushBack(2);
                indices.PushBack(lastOuterIndex++);
                indices.PushBack(lastOuterIndex);
                indices.PushBack(lastOuterIndex);
                indices.PushBack(3);
                indices.PushBack(2);


                // Left/Top
                for (int i = 1; i <= segmentsPerCorner; ++i)
                {
                    vertex.positionX = shadowInnerLeft + std::cos(radians(90.0f) + (segmentAngleRadians * static_cast<float>(i))) * radius;
                    vertex.positionY = shadowInnerTop  - std::sin(radians(90.0f) + (segmentAngleRadians * static_cast<float>(i))) * radius;
                    vertices.PushBack(vertex);

                    indices.PushBack(3);                    // Right/Top inner corner index.
                    indices.PushBack(lastOuterIndex++);
                    indices.PushBack(lastOuterIndex);
                }


                // Left Bridge. No need for an additional vertex here.
                indices.PushBack(3);
                indices.PushBack(lastOuterIndex);
                indices.PushBack(firstOuterIndex);
                indices.PushBack(firstOuterIndex);
                indices.PushBack(0);
                indices.PushBack(3);
            }

            this->shadowMesh = new Mesh(&vertices[0], vertices.count, &indices[0], indices.count);
        }
    }

    void ElementRenderingData::UpdateBorderMesh(const Element &element)
    {
        this->DeleteBorderMesh();

        auto opacity = element.GetAbsoluteOpacity();

        MeshVertex vertex;

        vertex.colourA   = opacity;
        vertex.texCoordX = 0.0f;
        vertex.texCoordY = 0.0f;



        GTLib::Vector<MeshVertex>   vertices;
        GTLib::Vector<unsigned int> indices;

        if (element.style.borderLeftWidth->value > 0.0f)
        {
            auto &colour = element.style.borderLeftColour->value;
            vertex.colourR = colour.r;
            vertex.colourG = colour.g;
            vertex.colourB = colour.b;


            float left   = 0.0f;
            float top    = static_cast<float>(element.style.borderTopWidth->value);
            float right  = static_cast<float>(element.style.borderLeftWidth->value);
            float bottom = static_cast<float>(element.GetHeight());


            vertex.positionX = left;  vertex.positionY = bottom;
            vertices.PushBack(vertex);

            vertex.positionX = right; vertex.positionY = bottom;
            vertices.PushBack(vertex);

            vertex.positionX = right; vertex.positionY = top;
            vertices.PushBack(vertex);

            vertex.positionX = left;  vertex.positionY = top;
            vertices.PushBack(vertex);
        }

        if (element.style.borderBottomWidth->value > 0.0f)
        {
            auto &colour = element.style.borderBottomColour->value;
            vertex.colourR = colour.r;
            vertex.colourG = colour.g;
            vertex.colourB = colour.b;


            float left   = static_cast<float>(element.style.borderLeftWidth->value);
            float top    = static_cast<float>(element.GetHeight()) - static_cast<float>(element.style.borderBottomWidth->value);
            float right  = static_cast<float>(element.GetWidth());
            float bottom = static_cast<float>(element.GetHeight());


            vertex.positionX = left;  vertex.positionY = bottom;
            vertices.PushBack(vertex);

            vertex.positionX = right; vertex.positionY = bottom;
            vertices.PushBack(vertex);

            vertex.positionX = right; vertex.positionY = top;
            vertices.PushBack(vertex);

            vertex.positionX = left;  vertex.positionY = top;
            vertices.PushBack(vertex);
        }

        if (element.style.borderRightWidth->value > 0.0f)
        {
            auto &colour = element.style.borderRightColour->value;
            vertex.colourR = colour.r;
            vertex.colourG = colour.g;
            vertex.colourB = colour.b;


            float left   = static_cast<float>(element.GetWidth()) - static_cast<float>(element.style.borderRightWidth->value);
            float top    = 0.0f;
            float right  = static_cast<float>(element.GetWidth());
            float bottom = static_cast<float>(element.GetHeight()) - static_cast<float>(element.style.borderBottomWidth->value);


            vertex.positionX = left;  vertex.positionY = bottom;
            vertices.PushBack(vertex);

            vertex.positionX = right; vertex.positionY = bottom;
            vertices.PushBack(vertex);

            vertex.positionX = right; vertex.positionY = top;
            vertices.PushBack(vertex);

            vertex.positionX = left;  vertex.positionY = top;
            vertices.PushBack(vertex);
        }

        if (element.style.borderTopWidth->value > 0.0f)
        {
            auto &colour = element.style.borderTopColour->value;
            vertex.colourR = colour.r;
            vertex.colourG = colour.g;
            vertex.colourB = colour.b;


            float left   = 0.0f;
            float top    = 0.0f;
            float right  = static_cast<float>(element.GetWidth()) - static_cast<float>(element.style.borderRightWidth->value);
            float bottom = static_cast<float>(element.style.borderTopWidth->value);


            vertex.positionX = left;  vertex.positionY = bottom;
            vertices.PushBack(vertex);

            vertex.positionX = right; vertex.positionY = bottom;
            vertices.PushBack(vertex);

            vertex.positionX = right; vertex.positionY = top;
            vertices.PushBack(vertex);

            vertex.positionX = left;  vertex.positionY = top;
            vertices.PushBack(vertex);
        }


        if (vertices.count > 0)
        {
            // We need to build the index list here. Every group of 4 vertices is a quad. We assert that the number of vertices is a multiple of 4.
            for (size_t i = 0; i < vertices.count; i += 4)
            {
                unsigned int startIndex = static_cast<unsigned int>(i);

                indices.PushBack(startIndex + 0);
                indices.PushBack(startIndex + 1);
                indices.PushBack(startIndex + 2);
                indices.PushBack(startIndex + 2);
                indices.PushBack(startIndex + 3);
                indices.PushBack(startIndex + 0);
            }

            this->borderMesh = new Mesh(&vertices[0], vertices.count, &indices[0], indices.count);
        }
    }

    void ElementRenderingData::UpdateBackgroundMeshes(const Element &element)
    {
        this->DeleteBackgroundMeshes();

        // Background Colour.
        if (!element.style.backgroundColour->isnone)
        {
            // For now we will do a quad covering the whole element, letting the scissor test take care of clipping. We don't want to draw anything
            // where the border is, since that will make transparency not look quite right.
            float left   = static_cast<float>(element.style.borderLeftWidth->value);
            float top    = static_cast<float>(element.style.borderTopWidth->value);
            float right  = static_cast<float>(element.GetWidth())  - static_cast<float>(element.style.borderRightWidth->value);
            float bottom = static_cast<float>(element.GetHeight()) - static_cast<float>(element.style.borderBottomWidth->value);

            auto &colour  = element.style.backgroundColour->value;
            auto  opacity = element.GetAbsoluteOpacity();


            MeshVertex vertices[4];

            vertices[0].positionX = left;     vertices[0].positionY = bottom;
            vertices[0].texCoordX = 0.0f;     vertices[0].texCoordY = 0.0f;
            vertices[0].colourR   = colour.r; vertices[0].colourG   = colour.g; vertices[0].colourB = colour.b; vertices[0].colourA = opacity;

            vertices[1].positionX = right;    vertices[1].positionY = bottom;
            vertices[1].texCoordX = 1.0f;     vertices[1].texCoordY = 0.0f;
            vertices[1].colourR   = colour.r; vertices[1].colourG   = colour.g; vertices[1].colourB = colour.b; vertices[1].colourA = opacity;

            vertices[2].positionX = right;    vertices[2].positionY = top;
            vertices[2].texCoordX = 1.0f;     vertices[2].texCoordY = 1.0f;
            vertices[2].colourR   = colour.r; vertices[2].colourG   = colour.g; vertices[2].colourB = colour.b; vertices[2].colourA = opacity;

            vertices[3].positionX = left;     vertices[3].positionY = top;
            vertices[3].texCoordX = 0.0f;     vertices[3].texCoordY = 1.0f;
            vertices[3].colourR   = colour.r; vertices[3].colourG   = colour.g; vertices[3].colourB = colour.b; vertices[3].colourA = opacity;


            unsigned int indices[6] =
            {
                0, 1, 2,
                2, 3, 0
            };


            this->backgroundMesh = new Mesh(vertices, 4, indices, 6);
        }


        // Background Image.
        if (element.style.backgroundImage->isset && element.style.backgroundImage->value != "none")
        {
            auto image = element.server.AcquireImage(element.style.backgroundImage->value.c_str());
            if (image != 0)
            {
                assert(element.server.GetImageManager() != nullptr);        // <-- We can make this assertion if the returned image is non-zero.
                
                float imageWidth;
                float imageHeight;
                element.server.GetImageManager()->GetImageDimensions(image, imageWidth, imageHeight);
                
                
                // What we're doing here is simply determining how to position and UV map the quad that will contain the image.
                float left   = static_cast<float>(element.style.borderLeftWidth->value);
                float top    = static_cast<float>(element.style.borderTopWidth->value);
                float right  = static_cast<float>(element.GetWidth())  - static_cast<float>(element.style.borderRightWidth->value);
                float bottom = static_cast<float>(element.GetHeight()) - static_cast<float>(element.style.borderBottomWidth->value);

                float uvLeft   = 0.0f;
                float uvTop    = 1.0f;
                float uvRight  = 1.0f;
                float uvBottom = 0.0f;

                auto &colour  = element.style.backgroundImageColour->value;
                auto  opacity = element.GetAbsoluteOpacity();

                float elementWidth  = static_cast<float>(right  - left);
                float elementHeight = static_cast<float>(bottom - top);

                // X
                if (element.style.backgroundRepeatX->value == "stretch")
                {
                    // Don't need to do anything here. All we're doing here is drawing a quad over the entire background.
                }
                else if (element.style.backgroundRepeatX->value == "repeat")
                {
                    // In this case, positions stay the same, but UVs need to change.
                    if (element.style.backgroundAlignX->value == "left")
                    {
                        uvLeft  = 0.0f;
                        uvRight = elementWidth / imageWidth;
                    }
                    else if (element.style.backgroundAlignX->value == "right")
                    {
                        uvLeft = -std::modf(elementWidth / imageWidth, &uvRight);
                    }
                    else    // center by default.
                    {
                        float units = elementWidth / imageWidth;

                        uvLeft  = -(units * 0.5f) + (1.0f / imageWidth * 0.5f) + 0.5f;
                        uvRight = +(units * 0.5f) + (1.0f / imageWidth * 0.5f) + 0.5f;
                    }
                }
                else    // 'none' by default.
                {
                    // In this case, we need to draw the image at it's native size, but taking alignment into account.
                    if (element.style.backgroundAlignX->value == "left")
                    {
                        // Nothing to do here.
                    }
                    else if (element.style.backgroundAlignX->value == "right")
                    {
                        left = right - imageWidth;
                    }
                    else    // center by default.
                    {
                        left = left + (static_cast<int>(right - left) / 2) - (static_cast<int>(imageWidth) / 2);            // <-- Note the integer arithmetic here.
                    }

                    right = left + imageWidth;
                }


                // Y
                if (element.style.backgroundRepeatY->value == "stretch")
                {
                    // Don't need to do anything here. All we're doing here is drawing a quad over the entire background.
                }
                else if (element.style.backgroundRepeatY->value == "repeat")
                {
                    // In this case, positions stay the same, but UVs need to change.
                    if (element.style.backgroundAlignY->value == "top")
                    {
                        uvBottom = -std::modf(elementHeight / imageHeight, &uvTop);
                    }
                    else if (element.style.backgroundAlignY->value == "bottom")
                    {
                        uvBottom = 0.0f;
                        uvTop    = elementHeight / imageHeight;
                    }
                    else    // center by default.
                    {
                        float units = elementHeight / imageHeight;

                        uvBottom = -(units * 0.5f) + (1.0f / imageHeight * 0.5f) + 0.5f;
                        uvTop    = +(units * 0.5f) + (1.0f / imageHeight * 0.5f) + 0.5f;
                    }
                }
                else    // none by default.
                {
                    if (element.style.backgroundAlignY->value == "top")
                    {
                        // Nothing to do here.
                    }
                    else if (element.style.backgroundAlignY->value == "bottom")
                    {
                        top = bottom - imageHeight;
                    }
                    else    // center by default.
                    {
                        top = top + (static_cast<int>(bottom - top) / 2) - (static_cast<int>(imageHeight) / 2);         // <-- Note the integer arithmetic here.
                    }

                    bottom = top + imageHeight;
                }


                // With the positions and UVs generated, we can now build the Mesh object.
                MeshVertex vertices[4];

                vertices[0].positionX = left;     vertices[0].positionY = bottom;
                vertices[0].texCoordX = uvLeft;   vertices[0].texCoordY = uvBottom;
                vertices[0].colourR   = colour.r; vertices[0].colourG   = colour.g; vertices[0].colourB = colour.b; vertices[0].colourA = opacity;

                vertices[1].positionX = right;    vertices[1].positionY = bottom;
                vertices[1].texCoordX = uvRight;  vertices[1].texCoordY = uvBottom;
                vertices[1].colourR   = colour.r; vertices[1].colourG   = colour.g; vertices[1].colourB = colour.b; vertices[1].colourA = opacity;

                vertices[2].positionX = right;    vertices[2].positionY = top;
                vertices[2].texCoordX = uvRight;  vertices[2].texCoordY = uvTop;
                vertices[2].colourR   = colour.r; vertices[2].colourG   = colour.g; vertices[2].colourB = colour.b; vertices[2].colourA = opacity;

                vertices[3].positionX = left;     vertices[3].positionY = top;
                vertices[3].texCoordX = uvLeft;   vertices[3].texCoordY = uvTop;
                vertices[3].colourR   = colour.r; vertices[3].colourG   = colour.g; vertices[3].colourB = colour.b; vertices[3].colourA = opacity;


                unsigned int indices[6] =
                {
                    0, 1, 2,
                    2, 3, 0
                };


                this->backgroundImageMesh = new Mesh(vertices, 4, indices, 6);
                this->backgroundImageMesh->SetTexture(image);
            }
        }
    }


    void ElementRenderingData::SetAbsoluteOpacity(float newAbsoluteOpacity)
    {
        this->absoluteOpacity = newAbsoluteOpacity;
    }

    float ElementRenderingData::GetAbsoluteOpacity() const
    {
        return this->absoluteOpacity;
    }



    ///////////////////////////////////////////////
    // Private

    void ElementRenderingData::DeleteTextMeshes()
    {
        for (size_t i = 0; i < this->textMeshes.count; ++i)
        {
            this->gc.MarkForCollection(*this->textMeshes[i]);
        }
        this->textMeshes.Clear();
    }

    void ElementRenderingData::DeleteShadowMesh()
    {
        this->gc.MarkForCollection(*this->shadowMesh);
        this->shadowMesh = nullptr;
    }

    void ElementRenderingData::DeleteBorderMesh()
    {
        this->gc.MarkForCollection(*this->borderMesh);
        this->borderMesh = nullptr;
    }

    void ElementRenderingData::DeleteBackgroundMeshes()
    {
        this->gc.MarkForCollection(*this->backgroundMesh);
        this->gc.MarkForCollection(*this->backgroundImageMesh);

        this->backgroundMesh      = nullptr;
        this->backgroundImageMesh = nullptr;
    }
}
