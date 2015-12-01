// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/Rendering/GUIRenderer.hpp>
#include <GTEngine/GUI/GUIServer.hpp>

namespace GT
{
    GUIRenderer::GUIRenderer()
        : currentTexture(0), isScissorSetToViewport(false), isBlendingEnabled(false)
    {
    }

    GUIRenderer::~GUIRenderer()
    {
    }



    ////////////////////////////////////////////////////////////////////////
    // Virtual Methods

    void GUIRenderer::Begin(const GUIServer &)
    {
    }

    void GUIRenderer::End()
    {
    }

    void GUIRenderer::SetScissor(int, int, unsigned int, unsigned int)
    {
    }

    void GUIRenderer::BeginElementOnDrawEvent(GUIElement &)
    {
    }

    void GUIRenderer::EndElementOnDrawEvent(GUIElement &)
    {
    }

    void GUIRenderer::SetOffset(float, float)
    {
    }

    void GUIRenderer::EnableBlending()
    {
    }

    void GUIRenderer::DisableBlending()
    {
    }

    void GUIRenderer::SetTexture(GUIImageHandle)
    {
    }

    void GUIRenderer::Draw(const float*, size_t, const unsigned int*, size_t)
    {
    }



    ////////////////////////////////////////////////////////////////////////
    // Non-Virtual Methods

    void GUIRenderer::RenderElement(GUIServer &server, GUIElement &element)
    {
        // Helpers to make things a bit easier to use.
        const GT::Rect<int> &clippingRect      = element.layout.clippingRect;
        const GT::Rect<int> &clippingRectInner = element.layout.clippingRectInner;

        // We're not going to draw anything if the rectangle is an invalid size.
        if (clippingRect.right > clippingRect.left && clippingRect.bottom > clippingRect.top)
        {
            // We're going to grab the text cursor mesh for use later on.
            GUIMesh* caretMesh = nullptr;

            auto &caret = server.GetCaret();
            if (caret.IsVisible() && &element == caret.GetOwner())
            {
                caret.ValidateRenderingData();
                caretMesh = caret.GetMesh();
            }


            // For ease of use.
            const float absoluteX = static_cast<float>(element.layout.absoluteX);
            const float absoluteY = static_cast<float>(element.layout.absoluteY);
            const float opacity   = element.GetAbsoluteOpacity();

            
            // Initial offset.
            this->_SetOffset(absoluteX, absoluteY);

            // Initial texture.
            this->_SetTexture(0);

            // Initial blending state.
            if (opacity < 1.0f)
            {
                this->_EnableBlending();
            }
            else
            {
                this->_DisableBlending();
            }


            // If we have a shadow, that needs to be the first thing we draw. We clip shadows against the parent's scissor rectangle
            if (element.style.enableShadow->value)
            {
                // The shadow rendering data needs to be validated before rendering. This returns immediately if the data is already valid, so it's OK
                // to skip an explicit check for that here.
                element.ValidateShadowRenderingData();

                auto shadowMesh = element.renderingData.GetShadowMesh();
                if (shadowMesh != nullptr)
                {
                    // Scissor.
                    if (element.style.positioning->value == GUIPositioning_Auto)
                    {
                        this->_SetScissor(element.parent->layout.clippingRect.left, element.parent->layout.clippingRect.top, element.parent->layout.clippingRect.GetWidth(), element.parent->layout.clippingRect.GetHeight());
                    }
                    else
                    {
                        this->_SetScissor(0, 0, server.GetViewportWidth(), server.GetViewportHeight(), true);
                    }

                    // Draw.
                    if (opacity >= 1.0f) this->_EnableBlending();
                    {
                        this->_Draw(shadowMesh);
                    }
                    if (opacity >= 1.0f) this->_DisableBlending();
                }
            }



            // Rendering data should be validated.
            element.ValidateBackgroundRenderingData();
            element.ValidateBorderRenderingData();

            auto backgroundMesh      = element.renderingData.GetBackgroundMesh();
            auto backgroundImageMesh = element.renderingData.GetBackgroundImageMesh();
            auto borderMesh          = element.renderingData.GetBorderMesh();

            // Now we render, but only if there is actually something to render.
            if (element.IsHandlingOnDraw() || backgroundMesh != nullptr || backgroundImageMesh != nullptr || borderMesh != nullptr)
            {
                // The shadow is drawn. Now for the element itself.
                //
                // The scissor should only be called if we actually need clipping.
                if (element.isClippedByParent || element.IsHandlingOnDraw())
                {
                    this->_SetScissor(clippingRect.left, clippingRect.top, clippingRect.GetWidth(), clippingRect.GetHeight());
                }
                else
                {
                    if (!this->isScissorSetToViewport)
                    {
                        this->_SetScissor(0, 0, server.GetViewportWidth(), server.GetViewportHeight(), true);
                    }
                }


                // The OnDraw event needs to be called. It needs to be after the scissor rectangle so that content is clipped, but before anything
                // else is drawn. We do it before everything else because that way it's guaranteed that other stuff will be visible such as borders.
                if (element.IsHandlingOnDraw())
                {
                    this->BeginElementOnDrawEvent(element);
                    {
                        element.OnDraw();
                    }
                    this->EndElementOnDrawEvent(element);
                }


                    
                if (backgroundMesh != nullptr)
                {
                    this->_Draw(backgroundMesh);
                }
                    
                if (backgroundImageMesh != nullptr)
                {
                    bool textureNeedsBlending = server.GetImageManager()->GetImageFormat(backgroundImageMesh->GetTexture()) == GUIImageFormat_RGBA8;

                    this->_SetTexture(backgroundImageMesh->GetTexture());
                    {
                        if (opacity >= 1.0f && textureNeedsBlending) this->_EnableBlending();
                        {
                            this->_Draw(backgroundImageMesh);
                        }
                        if (opacity >= 1.0f && textureNeedsBlending) this->_DisableBlending();
                    }
                    this->_SetTexture(0);
                }

                if (borderMesh != nullptr)
                {
                    this->_Draw(borderMesh);
                }


                // Draw the text cursor.
                if (caretMesh != nullptr)
                {
                    this->_Draw(caretMesh);
                }
            }
            else
            {
                // Draw the text cursor.
                if (caretMesh != nullptr)
                {
                    this->_SetScissor(clippingRect.left, clippingRect.top, clippingRect.GetWidth(), clippingRect.GetHeight());
                    this->_Draw(caretMesh);
                }
            }



            // Now we draw text. Text needs to be clipped against the same rectangle as children would be.
            if (element.HasText() && (clippingRectInner.right > clippingRectInner.left && clippingRectInner.bottom > clippingRectInner.top))
            {
                element.ValidateTextRenderingData();
                    
                auto &textMeshes = element.renderingData.GetTextMeshes();
                if (textMeshes.count > 0)
                {
                    // Scissor.
                    GT::Rect<int> textRect;
                    element.textManager.GetTextRect(textRect);

                    if (textRect.left + element.layout.absoluteX < clippingRectInner.left || textRect.right  + element.layout.absoluteX > clippingRectInner.right ||
                        textRect.top  + element.layout.absoluteY < clippingRectInner.top  || textRect.bottom + element.layout.absoluteY > clippingRectInner.bottom)
                    {
                        this->_SetScissor(clippingRectInner.left, clippingRectInner.top, clippingRectInner.GetWidth(), clippingRectInner.GetHeight());
                    }
                    else
                    {
                        if (!this->isScissorSetToViewport)
                        {
                            this->_SetScissor(0, 0, server.GetViewportWidth(), server.GetViewportHeight(), true);
                        }
                    }

                    // Offset.
                    this->_SetOffset(absoluteX + element.GetLeftPadding(), absoluteY + element.GetTopPadding());

                    // Draw.
                    for (size_t i = 0; i < textMeshes.count; ++i)
                    {
                        auto mesh = textMeshes[i];
                        assert(mesh != nullptr);
                        {
                            this->_SetTexture(mesh->GetTexture());
                            this->_EnableBlending();
                            this->_Draw(mesh);
                        }
                    }
                }
            }

                

            // And now we need to draw the children. Remember that any childing using the z-index will be drawn separately, thus we don't
            // draw them here.
            for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
            {
                if (!iChild->UsesZIndex() && iChild->style.visible->value)
                {
                    this->RenderElement(server, *iChild);
                }
            }
        }
    }

    void GUIRenderer::_SetScissor(int x, int y, unsigned int width, unsigned int height, bool isSetToWholeViewport)
    {
        this->SetScissor(x, y, width, height);
        this->isScissorSetToViewport = isSetToWholeViewport;
    }

    void GUIRenderer::_SetOffset(float offsetX, float offsetY)
    {
        this->SetOffset(offsetX, offsetY);
    }

    void GUIRenderer::_SetTexture(GUIImageHandle texture)
    {
        if (texture != this->currentTexture)
        {
            this->SetTexture(texture);
            this->currentTexture = texture;
        }
    }

    void GUIRenderer::_EnableBlending()
    {
        if (!this->isBlendingEnabled)
        {
            this->EnableBlending();
        }
    }

    void GUIRenderer::_DisableBlending()
    {
        if (this->isBlendingEnabled)
        {
            this->DisableBlending();
        }
    }

    void GUIRenderer::_Draw(GUIMesh* mesh)
    {
        this->Draw(mesh->GetVertices(), mesh->GetVertexCount(), mesh->GetIndices(), mesh->GetIndexCount());
    }
}

