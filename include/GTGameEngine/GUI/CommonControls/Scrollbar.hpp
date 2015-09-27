// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_Scrollbar
#define GT_Scrollbar

#include "GUIControl.hpp"

namespace GT
{
    static const int MinScrollbarThumbSize = 8;

    typedef std::function<void (int scrollPos)> Scrollbar_OnScroll;

    enum class ScrollbarOrientation
    {
        Vertical,
        Horizontal
    };

    class Scrollbar : public GUIControl
    {
    public:

        /// Constructor
        Scrollbar(GUIContext &gui, HGUIElement hParent, ScrollbarOrientation orientation);

        /// Destructor.
        ~Scrollbar();


        /// Retrieves a handle to the GUI element for the thumb.
        ///
        /// @remarks
        ///     You can use this if you need to give the thumb a custom style, however it is not recommended to manually set it's position and size.
        HGUIElement GetThumbElement() const;

        /// Retrieves the orientation of the scrollbar.
        ScrollbarOrientation GetOrientation() const;


        /// Sets the color of the track.
        ///
        /// @remarks
        ///     The track is actually the root element, so this is equality to doing gui.SetElementBackgroundColor(scrollbar.GetRootElement(), color).
        void SetTrackColor(const GT::Color &color);

        /// Sets the default color of the thumb.
        void SetDefaultThumbColor(const GT::Color &color);

        /// Sets the hovered color of the thumb.
        void SetHoveredThumbColor(const GT::Color &color);

        /// Sets the pressed color of the thumb.
        void SetPressedThumbColor(const GT::Color &color);


        /// Enables auto-hiding of the thumb.
        void EnableThumbAutoHide();

        /// Disables auto-hiding of the thumb.
        void DisableThumbAutoHide();

        /// Determines whether or not thumb auto-hiding is enabled.
        bool IsThumbAutoHideEnabled() const { return m_autoHideThumb; }


        /// Sets the mouse wheel scale.
        ///
        /// @remarks
        ///     Set this to a negative value to reverse the direction.
        void SetMouseWheelScale(int scale);

        /// Retrieves the mouse wheel scale.
        int GetMouseWheelScale() const;


        /// Sets the scrollbar's range.
        void SetRange(int rangeMin, int rangeMax);

        /// Retrieves the scrollbar's range.
        void GetRange(OUT int &rangeMin, OUT int &rangeMax) const;


        /// Sets the page size.
        ///
        /// @remarks
        ///     This controls the size of the thumb.
        void SetPageSize(int pageSize);

        /// Retrieves the page size.
        int GetPageSize() const;


        /// Explicitly sets the scroll position.
        ///
        /// @remarks
        ///     This will move the thumb, but not post the OnScroll event.
        ///     @par
        ///     The scroll position will be clamped to the current range, minus the page size.
        void SetScrollPosition(int position);

        /// Retrieves the scroll position.
        int GetScrollPosition() const;



        /// Scrolls by the given amount.
        ///
        /// @remarks
        ///     If the resulting scroll position differs from the old one, the on OnScroll will be posted.
        void Scroll(int offset);

        /// Scrolls to the given position.
        ///
        /// @remarks
        ///     This differs from SetScrollPosition in that it will post the OnScroll event.
        ///     @par
        ///     Note that the actual maximum scrollable position is equal to the maximum range value minus the page size.
        void ScrollTo(int newScrollPos);


        /// Posts the OnScroll event.
        void OnScroll();

        /// Registers an OnScroll event handler.
        void OnScroll(Scrollbar_OnScroll callback);



        /// Handles the OnSize event for the container element.
        ///
        /// @remarks
        ///     When the container is resized, the thumb size and position needs to be refreshed.
        void OnSize(unsigned int width, unsigned int height) override;


    private:

        /// Updates the size of the thumb.
        void RefreshThumbSize();

        /// Updates the position of the thumb.
        void RefreshThumbPosition();

        /// Updates the size and position of the thumb.
        void RefreshThumb();

        /// Helper for retrieving the size of the container element based on the orientation.
        int GetContainerSize() const;

        /// Helper for retrieving the size of the thumb element based on the orientation.
        int GetThumbSize() const;

        /// Calculates the size of the thumb.
        int CalculateThumbSize() const;

        /// Calculates the position of the thumb.
        int CalculateThumbPosition() const;


        /// Called when the thumb is pressed.
        void OnThumbPressed(int posX, int posY);

        /// Called when the thumb is released.
        void OnThumbReleased();

        /// Called when the thumb receives a mouse move event.
        void OnThumbMouseMove(int posX, int posY);


        /// Calculates the scroll position based on a pixel position where the thumb could potentially be placed.
        int CalculateScrollPositionFromThumbPosition(int thumbPos) const;


    private:

        /// The element representing the scrollbar's thumb.
        HGUIElement m_hThumb;


        /// The orientation of the scrollbar.
        ScrollbarOrientation m_orientation;

        /// The scrollbar's range.
        int m_rangeMin;
        int m_rangeMax;

        /// The size of a page. This is used to control how big the thumb should be.
        int m_pageSize;

        /// The current scroll position.
        int m_scrollPos;


        /// The position of the mouse relative to the thumb at the time is was pressed. This is used to calculate the position
        /// of the thumb as it is getting dragged.
        int m_dragRelativeMousePosX;
        int m_dragRelativeMousePosY;

        /// Keeps track of whether or not we are dragging the thumb.
        bool m_isDraggingThumb;

        /// Keeps track of whether or not the thumb should be hidden automatically when the page size is larger than the range.
        bool m_autoHideThumb;


        /// The scale to apply to scroll wheel scrolls.
        int m_mouseWheelScale;


        /// The list of OnScroll event handlers.
        GTLib::Vector<Scrollbar_OnScroll> m_onScroll;


        /// The default color of the thumb.
        GT::Color m_thumbDefaultColor;

        /// Sets the color of the thumb when the mouse is hovered over it.
        GT::Color m_thumbHoveredColor;

        /// Sets the color of the thumb when the mouse is pressed on the thumb.
        GT::Color m_thumbPressedColor;
    };
}

#endif
