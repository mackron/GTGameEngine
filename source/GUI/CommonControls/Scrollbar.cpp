// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/CommonControls/Scrollbar.hpp>
#include <GTGameEngine/WindowManager.hpp>   // For mouse button IDs.

namespace GT
{
    Scrollbar::Scrollbar(GUIContext &gui, HGUIElement hParent, ScrollbarOrientation orientation)
        : GUIControl(gui, hParent),
          m_hThumb(0),
          m_orientation(orientation),
          m_rangeMin(0), m_rangeMax(0), m_pageSize(0), m_scrollPos(0),
          m_dragRelativeMousePosX(0), m_dragRelativeMousePosY(0), m_isDraggingThumb(false),
          m_autoHideThumb(true),
          m_mouseWheelScale(1),
          m_onScroll(),
          m_thumbDefaultColor(0.4f, 0.4f, 0.4f), m_thumbHoveredColor(0.5f, 0.5f, 0.5f), m_thumbPressedColor(0.6f, 0.6f, 0.6f)
    {
        gui.SetElementPadding(m_hRootElement, 2);
        gui.SetElementBackgroundColor(m_hRootElement, GT::Color(0.3f, 0.3f, 0.3f));


        m_hThumb = gui.CreateElement();
        gui.SetElementParent(m_hThumb, m_hRootElement);
        gui.SetElementBackgroundColor(m_hThumb, m_thumbDefaultColor);

        if (orientation == ScrollbarOrientation::Vertical) {
            gui.SetElementWidthRatio(m_hThumb, 1);
        } else {
            gui.SetElementHeightRatio(m_hThumb, 1);
        }

        gui.OnElementMouseEnter(m_hThumb, [&]() {
            if (!m_isDraggingThumb) {
                gui.SetElementBackgroundColor(m_hThumb, m_thumbHoveredColor);
            }
        });
        gui.OnElementMouseLeave(m_hThumb, [&]() {
            if (!m_isDraggingThumb) {
                gui.SetElementBackgroundColor(m_hThumb, m_thumbDefaultColor);
            }
        });


        // Thumb dragging.
        gui.OnElementMouseButtonPressed(m_hThumb, [&](int button, int posX, int posY) {
            if (button == MouseButton_Left) {
                this->OnThumbPressed(posX, posY);
            }
        });
        gui.OnElementMouseButtonReleased(m_hThumb, [&](int button, int, int) {
            if (button == MouseButton_Left) {
                this->OnThumbReleased();
            }
        });
        gui.OnElementMouseMove(m_hThumb, [&](int posX, int posY) {
            this->OnThumbMouseMove(posX, posY);
        });


        // Mouse wheel.
        gui.OnElementMouseWheel(m_hRootElement, [&](int delta, int, int) {
            this->Scroll(-delta * m_mouseWheelScale);
        });
        gui.OnElementMouseWheel(m_hThumb, [&](int delta, int, int) {
            this->Scroll(-delta * m_mouseWheelScale);
        });


        // Page scrolling
        gui.OnElementMouseButtonPressed(m_hRootElement, [&](int button, int posX, int posY) {
            // This event is fired when the user clicks in the scrollbar region that is not consumed by the thumb.

            // If the left button is pressed, scroll by a page.
            if (button == MouseButton_Left) {
                int thumbPosX;
                int thumbPosY;
                m_gui.GetElementPosition(m_hThumb, thumbPosX, thumbPosY);

                if (m_orientation == ScrollbarOrientation::Vertical) {
                    if (posY < thumbPosY) {
                        this->Scroll(-m_pageSize);
                    } else if (posY > thumbPosY + m_gui.GetElementHeight(m_hThumb)) {
                        this->Scroll(m_pageSize);
                    }
                } else {
                    if (posX < thumbPosX) {
                        this->Scroll(-m_pageSize);
                    } else if (posX > thumbPosX + m_gui.GetElementWidth(m_hThumb)) {
                        this->Scroll(m_pageSize);
                    }
                }
            }

            // If the right button is pressed, scroll to the absolute position based on the center of the thumb.
            if (button == MouseButton_Right) {
                if (m_orientation == ScrollbarOrientation::Vertical) {
                    this->ScrollTo(this->CalculateScrollPositionFromThumbPosition(posY - int(m_gui.GetElementHeight(m_hThumb) / 2)));
                } else {
                    this->ScrollTo(this->CalculateScrollPositionFromThumbPosition(posX - int(m_gui.GetElementWidth(m_hThumb) / 2)));
                }
            }
        });
    }

    Scrollbar::~Scrollbar()
    {
        m_gui.DeleteElement(m_hThumb);
    }

    HGUIElement Scrollbar::GetThumbElement() const
    {
        return m_hThumb;
    }

    ScrollbarOrientation Scrollbar::GetOrientation() const
    {
        return m_orientation;
    }

    void Scrollbar::SetTrackColor(const GT::Color & color)
    {
        m_gui.SetElementBackgroundColor(m_hRootElement, color);
    }

    void Scrollbar::SetDefaultThumbColor(const GT::Color & color)
    {
        m_thumbDefaultColor = color;
        if (!m_isDraggingThumb && !m_gui.IsElementUnderMouse(m_hThumb)) {
            m_gui.SetElementBackgroundColor(m_hThumb, color);
        }
    }

    void Scrollbar::SetHoveredThumbColor(const GT::Color & color)
    {
        m_thumbHoveredColor = color;
        if (!m_isDraggingThumb && m_gui.IsElementUnderMouse(m_hThumb)) {
            m_gui.SetElementBackgroundColor(m_hThumb, color);
        }
    }

    void Scrollbar::SetPressedThumbColor(const GT::Color & color)
    {
        m_thumbPressedColor = color;
        if (m_isDraggingThumb) {
            m_gui.SetElementBackgroundColor(m_hThumb, color);
        }
    }


    void Scrollbar::EnableThumbAutoHide()
    {
        if (!m_autoHideThumb)
        {
            m_autoHideThumb = true;

            // We need to refresh the thumb in order for the auto-hide to potentially kick in.
            this->RefreshThumb();
        }
        
    }

    void Scrollbar::DisableThumbAutoHide()
    {
        if (m_autoHideThumb)
        {
            m_autoHideThumb = false;

            // Just refresh the thumb to show it again.
            this->RefreshThumb();
        }
    }


    void Scrollbar::SetMouseWheelScale(int scale)
    {
        m_mouseWheelScale = scale;
    }

    int Scrollbar::GetMouseWheelScale() const
    {
        return m_mouseWheelScale;
    }


    void Scrollbar::SetRange(int rangeMin, int rangeMax)
    {
        assert(rangeMax >= rangeMin);

        m_rangeMin = rangeMin;
        m_rangeMax = rangeMax;

        this->RefreshThumb();
    }

    void Scrollbar::GetRange(OUT int & rangeMin, OUT int & rangeMax) const
    {
        rangeMin = m_rangeMin;
        rangeMax = m_rangeMax;
    }

    void Scrollbar::SetPageSize(int pageSize)
    {
        m_pageSize = pageSize;

        this->RefreshThumb();
    }

    int Scrollbar::GetPageSize() const
    {
        return m_pageSize;
    }

    void Scrollbar::SetScrollPosition(int newScrollPos)
    {
        newScrollPos = clamp(newScrollPos, m_rangeMin, max(0, m_rangeMax - m_pageSize + 1));
        if (newScrollPos != m_scrollPos)
        {
            m_scrollPos = newScrollPos;

            this->RefreshThumbPosition();
        }
    }

    int Scrollbar::GetScrollPosition() const
    {
        return m_scrollPos;
    }


    void Scrollbar::Scroll(int offset)
    {
        this->ScrollTo(m_scrollPos + offset);
    }

    void Scrollbar::ScrollTo(int newScrollPos)
    {
        int oldScrollPos = m_scrollPos;
        this->SetScrollPosition(newScrollPos);
        
        if (oldScrollPos != m_scrollPos)
        {
            this->OnScroll();
        }
    }


    void Scrollbar::OnScroll()
    {
        for (size_t i = 0; i < m_onScroll.GetCount(); ++i)
        {
            m_onScroll[i](m_scrollPos);
        }
    }

    void Scrollbar::OnScroll(Scrollbar_OnScroll callback)
    {
        m_onScroll.PushBack(callback);
    }



    void Scrollbar::OnSize(unsigned int, unsigned int)
    {
        this->RefreshThumb();
    }



    ////////////////////////////////////////////////////////
    // Private

    void Scrollbar::RefreshThumbSize()
    {
        int thumbSize = this->CalculateThumbSize();

        if (m_orientation == ScrollbarOrientation::Vertical) {
            m_gui.SetElementHeight(m_hThumb, static_cast<unsigned int>(thumbSize));
        } else {
            m_gui.SetElementWidth(m_hThumb, static_cast<unsigned int>(thumbSize));
        }
    }

    void Scrollbar::RefreshThumbPosition()
    {
        // Calculate position.
        int thumbPos = this->CalculateThumbPosition();


        // Set position.
        float currentThumbPosX;
        float currentThumbPosY;
        m_gui.GetElementPosition(m_hThumb, currentThumbPosX, currentThumbPosY);

        if (m_orientation == ScrollbarOrientation::Vertical) {
            m_gui.SetElementPosition(m_hThumb, 0.0f, float(thumbPos));
        } else {
            m_gui.SetElementPosition(m_hThumb, float(thumbPos), 0.0f);
        }
    }

    void Scrollbar::RefreshThumb()
    {
        this->RefreshThumbSize();
        this->RefreshThumbPosition();


        if (m_autoHideThumb && m_pageSize >= (m_rangeMax - m_rangeMin + 1))
        {
            m_gui.HideElement(m_hThumb);
        }
        else
        {
            m_gui.ShowElement(m_hThumb);
        }
    }

    int Scrollbar::GetContainerSize() const
    {
        if (m_orientation == ScrollbarOrientation::Vertical) {
            return int(m_gui.GetElementInnerHeight(m_hRootElement));
        } else {
            return int(m_gui.GetElementInnerWidth(m_hRootElement));
        }
    }

    int Scrollbar::GetThumbSize() const
    {
        if (m_orientation == ScrollbarOrientation::Vertical) {
            return int(m_gui.GetElementHeight(m_hThumb));
        } else {
            return int(m_gui.GetElementWidth(m_hThumb));
        }
    }

    int Scrollbar::CalculateThumbSize() const
    {
        // The size of the thumb is calculated based on the size of the page, the range, and the size of the root element which is the container
        // that houses the thumb.
        int containerSize = this->GetContainerSize();
        int range         = m_rangeMax - m_rangeMin + 1;
        
        int thumbSize = MinScrollbarThumbSize;
        if (range > 0)
        {
            thumbSize = int(GTLib::Round((containerSize / float(range)) * m_pageSize));
            thumbSize = clamp(thumbSize, MinScrollbarThumbSize, containerSize);
        }

        return thumbSize;
    }

    int Scrollbar::CalculateThumbPosition() const
    {
        // The position of the thumb is calculated based on the range, the current scroll position and the size of the container element.
        int containerSize = this->GetContainerSize();
        int thumbSize     = this->CalculateThumbSize();
        int range         = m_rangeMax - m_rangeMin + 1;

        // Calculate position.
        int thumbPos = 0;
        if (range > m_pageSize)
        {
            thumbPos = int(GTLib::Round((containerSize / float(range)) * m_scrollPos));
            thumbPos = clamp(thumbPos, 0, containerSize - thumbSize);
        }

        return thumbPos;
    }

    void Scrollbar::OnThumbPressed(int posX, int posY)
    {
        // The user is wanting to drag the thumb which means we need the thumb to capture mouse events.
        m_gui.SetMouseEventCapture(m_hThumb);

        // Set the color for visual feedback.
        m_gui.SetElementBackgroundColor(m_hThumb, m_thumbPressedColor);

        m_dragRelativeMousePosX = posX;
        m_dragRelativeMousePosY = posY;
        m_isDraggingThumb = true;
    }

    void Scrollbar::OnThumbReleased()
    {
        m_isDraggingThumb = false;

        if (m_gui.IsElementUnderMouse(m_hThumb)) {
            m_gui.SetElementBackgroundColor(m_hThumb, m_thumbHoveredColor);
        } else {
            m_gui.SetElementBackgroundColor(m_hThumb, m_thumbDefaultColor);
        }

        // We need to make sure the thumb is positioned correctly.
        this->RefreshThumb();

        // When the thumb was pressed we captured mouse events so now we need to release it.
        m_gui.ReleaseMouseEventCapture();
    }

    void Scrollbar::OnThumbMouseMove(int posX, int posY)
    {
        if (m_isDraggingThumb)
        {
            int containerSize = this->GetContainerSize();
            int thumbSize     = this->CalculateThumbSize();

            int thumbPosX;
            int thumbPosY;
            m_gui.GetElementPosition(m_hThumb, thumbPosX, thumbPosY);

            if (m_orientation == ScrollbarOrientation::Vertical) {
                int dragOffset = posY - m_dragRelativeMousePosY;
                thumbPosY = clamp(thumbPosY + dragOffset, 0, (containerSize - thumbSize));

                int newScrollPos = this->CalculateScrollPositionFromThumbPosition(thumbPosY);
                if (newScrollPos != m_scrollPos) {
                    m_scrollPos = newScrollPos;
                    this->RefreshThumbPosition();

                    this->OnScroll();
                }
            } else {
                int dragOffset = posX - m_dragRelativeMousePosX;
                thumbPosX = clamp(thumbPosX + dragOffset, 0, (containerSize - thumbSize));

                int newScrollPos = this->CalculateScrollPositionFromThumbPosition(thumbPosX);
                if (newScrollPos != m_scrollPos) {
                    m_scrollPos = newScrollPos;
                    this->RefreshThumbPosition();

                    this->OnScroll();
                }
            }
        }
    }

    int Scrollbar::CalculateScrollPositionFromThumbPosition(int thumbPos) const
    {
        int containerSize = this->GetContainerSize();
        int range         = m_rangeMax - m_rangeMin + 1;

        return int(GTLib::Round(thumbPos / ((containerSize / float(range)))));
    }
}
