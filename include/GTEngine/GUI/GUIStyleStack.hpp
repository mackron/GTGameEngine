// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIStyleStack
#define GT_GUIStyleStack

#include "GUIStyleClass.hpp"
#include "GUIStyleAttribute.hpp"
#include <GTEngine/Core/List.hpp>

namespace GTGUI
{
    // GUIElement.hpp includes this file.
    class GUIElement;
    
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
    class GUIStyleStack
    {
    public:
    
        /**
        *   \brief  Constructor.
        */
        GUIStyleStack(GUIElement &owner);
        
        /**
        *   \brief  Destructor.
        */
        ~GUIStyleStack();
        
        /**
        *   \brief  Attaches a new style class to the start of the list, giving it the highest priority.
        *
        *   \remarks
        *       If the class is already in the list, it will be moved to the front.
        */
        void Attach(GUIStyleClass &newClass, bool refresh = true);
        
        /**
        *   \brief  Removes a style class from the list.
        */
        void Detach(GUIStyleClass &oldClass, bool refresh = true);
        
        /**
        *   \brief  Retrieves a reference to the root style. This is the first style in the stack.
        *   \return A pointer to the first style class on the stack.
        *
        *   \remarks
        *       If there are no classes in the stack, null is returned.
        */
        GUIStyleClass* FirstStyleClass();


        /// Activates the modifier classes of the given type.
        void ActivateModifierClasses(GUIStyleClassType type);

        /// Deactivates the modifier classes of the given type.
        void DeactivateModifierClasses(GUIStyleClassType type);

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
        GUIElement &owner;
        
        /// The list of classes in the stack.
        GT::List<GUIStyleClass*> classes;

        GUIStyleAttribute_Number*         width;
        GUIStyleAttribute_Number*         height;
        GUIStyleAttribute_Number*         minWidth;
        GUIStyleAttribute_Number*         maxWidth;
        GUIStyleAttribute_Number*         minHeight;
        GUIStyleAttribute_Number*         maxHeight;
        GUIStyleAttribute_String*         relativeWidthMode;
        GUIStyleAttribute_String*         relativeHeightMode;
        GUIStyleAttribute_Boolean*        flexChildWidth;
        GUIStyleAttribute_Boolean*        flexChildHeight;
        GUIStyleAttribute_Colour*         backgroundColour;
        GUIStyleAttribute_String*         backgroundImage;
        GUIStyleAttribute_Colour*         backgroundImageColour;
        GUIStyleAttribute_String*         backgroundAlignX;
        GUIStyleAttribute_String*         backgroundAlignY;
        GUIStyleAttribute_String*         backgroundRepeatX;
        GUIStyleAttribute_String*         backgroundRepeatY;
        GUIStyleAttribute_Number*         borderLeftWidth;
        GUIStyleAttribute_Number*         borderRightWidth;
        GUIStyleAttribute_Number*         borderTopWidth;
        GUIStyleAttribute_Number*         borderBottomWidth;
        GUIStyleAttribute_Colour*         borderLeftColour;
        GUIStyleAttribute_Colour*         borderRightColour;
        GUIStyleAttribute_Colour*         borderTopColour;
        GUIStyleAttribute_Colour*         borderBottomColour;
        GUIStyleAttribute_Number*         paddingLeft;
        GUIStyleAttribute_Number*         paddingRight;
        GUIStyleAttribute_Number*         paddingTop;
        GUIStyleAttribute_Number*         paddingBottom;
        GUIStyleAttribute_Number*         marginLeft;
        GUIStyleAttribute_Number*         marginRight;
        GUIStyleAttribute_Number*         marginTop;
        GUIStyleAttribute_Number*         marginBottom;
        GUIStyleAttribute_Plane*          childPlane;
        GUIStyleAttribute_Align*          horizontalAlign;
        GUIStyleAttribute_Align*          verticalAlign;
        GUIStyleAttribute_String*         cursor;
        GUIStyleAttribute_Boolean*        visible;
        GUIStyleAttribute_Number*         zIndex;
        GUIStyleAttribute_Boolean*        transparentMouseInput;
        GUIStyleAttribute_Boolean*        enabled;
        GUIStyleAttribute_Colour*         textCursorColour;
        GUIStyleAttribute_Boolean*        canReceiveFocusFromMouse;
        GUIStyleAttribute_Positioning*    positioning;
        GUIStyleAttribute_Number*         left;
        GUIStyleAttribute_Number*         right;
        GUIStyleAttribute_Number*         top;
        GUIStyleAttribute_Number*         bottom;
        GUIStyleAttribute_PositionOrigin* positionOrigin;
        GUIStyleAttribute_Number*         innerOffsetX;
        GUIStyleAttribute_Number*         innerOffsetY;
        GUIStyleAttribute_String*         fontFamily;
        GUIStyleAttribute_Number*         fontSize;
        GUIStyleAttribute_FontWeight*     fontWeight;
        GUIStyleAttribute_FontSlant*      fontSlant;
        GUIStyleAttribute_Colour*         textColour;
        GUIStyleAttribute_Colour*         textSelectionColour;
        GUIStyleAttribute_Colour*         textSelectionBackgroundColour;
        GUIStyleAttribute_Colour*         textSelectionBackgroundColourBlurred;
        GUIStyleAttribute_Boolean*        editableText;
        GUIStyleAttribute_Boolean*        singleLineText;
        GUIStyleAttribute_Number*         opacity;
        GUIStyleAttribute_Boolean*        compoundOpacity;
        GUIStyleAttribute_Boolean*        enableShadow;
        GUIStyleAttribute_Colour*         shadowColour;
        GUIStyleAttribute_Number*         shadowBlurRadius;
        GUIStyleAttribute_Number*         shadowOffsetX;
        GUIStyleAttribute_Number*         shadowOffsetY;
        GUIStyleAttribute_Number*         shadowExtrusionX;
        GUIStyleAttribute_Number*         shadowExtrusionY;
        GUIStyleAttribute_Number*         shadowOpacity;
        GUIStyleAttribute_Boolean*        allowMouseDrag;
        GUIStyleAttribute_Boolean*        constrainMouseDragX;
        GUIStyleAttribute_Boolean*        constrainMouseDragY;
        GUIStyleAttribute_String*         mouseDragClampModeX;
        GUIStyleAttribute_String*         mouseDragClampModeY;
        GUIStyleAttribute_Boolean*        allowMouseResize;
        GUIStyleAttribute_Number*         leftGripperWidth;
        GUIStyleAttribute_Number*         rightGripperWidth;
        GUIStyleAttribute_Number*         topGripperWidth;
        GUIStyleAttribute_Number*         bottomGripperWidth;

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
        GUIStyleStack(const GUIStyleStack &);
        GUIStyleStack & operator=(const GUIStyleStack &);
    };
}

#endif
