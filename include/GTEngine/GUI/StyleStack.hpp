// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_StyleStack_hpp_
#define __GTLib_GUI_StyleStack_hpp_

#include "StyleClass.hpp"
#include "StyleAttribute.hpp"
#include <GTEngine/Core/List.hpp>

namespace GTGUI
{
    // Element.hpp includes this file.
    class Element;
    
    /**
    *   \brief  Class containing the list of of style classes forming the style of an element.
    *
    *   A style stack must contain a default style, thus why one is required in the constructor. Usually this
    *   is set to the default style from the style server.
    *
    *   The default style, and any other attached style classes, must remain valid throughout the life of the
    *   style stack. Typically the instantiations of the styles will be stored in a style environment, which
    *   will be global. In GTGUI, there is a single StyleEnvironment instantiation in the Environment class
    *   which stores the instantiations of all style classes.
    */
    class StyleStack
    {
    public:
    
        /**
        *   \brief  Constructor.
        */
        StyleStack(Element &owner);
        
        /**
        *   \brief  Destructor.
        */
        ~StyleStack();
        
        /**
        *   \brief  Attaches a new style class to the start of the list, giving it the highest priority.
        *
        *   \remarks
        *       If the class is already in the list, it will be moved to the front.
        */
        void Attach(StyleClass &newClass, bool refresh = true);
        
        /**
        *   \brief  Removes a style class from the list.
        */
        void Detach(StyleClass &oldClass, bool refresh = true);
        
        /**
        *   \brief  Retrieves a reference to the root style. This is the first style in the stack.
        *   \return A pointer to the first style class on the stack.
        *
        *   \remarks
        *       If there are no classes in the stack, null is returned.
        */
        StyleClass* FirstStyleClass();


        /// Activates the modifier classes of the given type.
        void ActivateModifierClasses(StyleClassType type);

        /// Deactivates the modifier classes of the given type.
        void DeactivateModifierClasses(StyleClassType type);

        /// Deactivates every modifier class.
        void DeactivateAllModifierClasses();


        /**
        *   \brief  Increments the lock counter. Unlock refreshes with UnlockRefresh().
        */
        void LockRefresh();

        /**
        *   \brief  Decrements the lock counter.
        */
        void UnlockRefresh();

        /**
        *   \brief  Determines whether or not refreshing is locked.
        */
        bool IsRefreshLocked() const { return this->lockCount > 0; }

        
    public: // Refresh functions.
    
        /**
        *   \brief  Refreshes every style attribute.
        */
        void Refresh();
        
        
        /// Updates the 'left'/'right' and 'top'/'bottom' positioning priorities based on the current stack state.
        void UpdatePositioningPriorities();


    
    public:
    
        /// A pointer to the element that owns this style stack.
        Element &owner;
        
        /// The list of classes in the stack.
        GTLib::List<StyleClass*> classes;

        StyleAttribute_Number*         width;
        StyleAttribute_Number*         height;
        StyleAttribute_Number*         minWidth;
        StyleAttribute_Number*         maxWidth;
        StyleAttribute_Number*         minHeight;
        StyleAttribute_Number*         maxHeight;
        StyleAttribute_String*         relativeWidthMode;
        StyleAttribute_String*         relativeHeightMode;
        StyleAttribute_Boolean*        flexChildWidth;
        StyleAttribute_Boolean*        flexChildHeight;
        StyleAttribute_Colour*         backgroundColour;
        StyleAttribute_String*         backgroundImage;
        StyleAttribute_Colour*         backgroundImageColour;
        StyleAttribute_String*         backgroundAlignX;
        StyleAttribute_String*         backgroundAlignY;
        StyleAttribute_String*         backgroundRepeatX;
        StyleAttribute_String*         backgroundRepeatY;
        StyleAttribute_Number*         borderLeftWidth;
        StyleAttribute_Number*         borderRightWidth;
        StyleAttribute_Number*         borderTopWidth;
        StyleAttribute_Number*         borderBottomWidth;
        StyleAttribute_Colour*         borderLeftColour;
        StyleAttribute_Colour*         borderRightColour;
        StyleAttribute_Colour*         borderTopColour;
        StyleAttribute_Colour*         borderBottomColour;
        StyleAttribute_Number*         paddingLeft;
        StyleAttribute_Number*         paddingRight;
        StyleAttribute_Number*         paddingTop;
        StyleAttribute_Number*         paddingBottom;
        StyleAttribute_Number*         marginLeft;
        StyleAttribute_Number*         marginRight;
        StyleAttribute_Number*         marginTop;
        StyleAttribute_Number*         marginBottom;
        StyleAttribute_Plane*          childPlane;
        StyleAttribute_Align*          horizontalAlign;
        StyleAttribute_Align*          verticalAlign;
        StyleAttribute_String*         cursor;
        StyleAttribute_Boolean*        visible;
        StyleAttribute_Number*         zIndex;
        StyleAttribute_Boolean*        transparentMouseInput;
        StyleAttribute_Boolean*        enabled;
        StyleAttribute_Colour*         textCursorColour;
        StyleAttribute_Boolean*        canReceiveFocusFromMouse;
        StyleAttribute_Positioning*    positioning;
        StyleAttribute_Number*         left;
        StyleAttribute_Number*         right;
        StyleAttribute_Number*         top;
        StyleAttribute_Number*         bottom;
        StyleAttribute_PositionOrigin* positionOrigin;
        StyleAttribute_Number*         innerOffsetX;
        StyleAttribute_Number*         innerOffsetY;
        StyleAttribute_String*         fontFamily;
        StyleAttribute_Number*         fontSize;
        StyleAttribute_FontWeight*     fontWeight;
        StyleAttribute_FontSlant*      fontSlant;
        StyleAttribute_Colour*         textColour;
        StyleAttribute_Colour*         textSelectionColour;
        StyleAttribute_Colour*         textSelectionBackgroundColour;
        StyleAttribute_Colour*         textSelectionBackgroundColourBlurred;
        StyleAttribute_Boolean*        editableText;
        StyleAttribute_Boolean*        singleLineText;
        StyleAttribute_Number*         opacity;
        StyleAttribute_Boolean*        compoundOpacity;
        StyleAttribute_Boolean*        enableShadow;
        StyleAttribute_Colour*         shadowColour;
        StyleAttribute_Number*         shadowBlurRadius;
        StyleAttribute_Number*         shadowOffsetX;
        StyleAttribute_Number*         shadowOffsetY;
        StyleAttribute_Number*         shadowExtrusionX;
        StyleAttribute_Number*         shadowExtrusionY;
        StyleAttribute_Number*         shadowOpacity;
        StyleAttribute_Boolean*        allowMouseDrag;
        StyleAttribute_Boolean*        constrainMouseDragX;
        StyleAttribute_Boolean*        constrainMouseDragY;
        StyleAttribute_String*         mouseDragClampModeX;
        StyleAttribute_String*         mouseDragClampModeY;
        StyleAttribute_Boolean*        allowMouseResize;
        StyleAttribute_Number*         leftGripperWidth;
        StyleAttribute_Number*         rightGripperWidth;
        StyleAttribute_Number*         topGripperWidth;
        StyleAttribute_Number*         bottomGripperWidth;

        // Whether or not 'right' has priority over 'left'.
        bool rightHasPriority;

        // Whether or not 'bottom' has priority over 'top'.
        bool bottomHasPriority;

        /// Keeps track of whether or not refreshing is locked. We use this so we can selectively avoid refreshes in cases
        /// where it isn't needed. An example is when we want to refresh attributes in groups rather than individually for
        /// the sake of efficiency.
        ///
        /// Calling LockRefresh() will increment the count. UnlockRefresh() will decrement. Refreshes will be locked so long
        /// as this variable is larger than 0. Defaults to 0 (unlocked). Will never be < 0.
        int lockCount;

        /// The modifiers currently applied.
        bool modifiers[StyleClassTypeCount];

        
    private:    // No copy.
        StyleStack(const StyleStack &);
        StyleStack & operator=(const StyleStack &);
    };
}

#endif
