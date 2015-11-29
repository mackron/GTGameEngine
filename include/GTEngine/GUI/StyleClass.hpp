// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_StyleClass_hpp_
#define __GTLib_GUI_StyleClass_hpp_

#include <GTEngine/Script.hpp>
#include <GTEngine/Core/List.hpp>
#include "StyleAttribute.hpp"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4480)   // nonstandard extension used: specifying underlying type for enum.
#endif

namespace GTGUI
{
    class StyleStack;
    class StyleServer;

    // !!! Be careful here. These values are used as indices into arrays (except StyleClassType_Normal). !!!
    enum StyleClassType : int
    {
        StyleClassType_None   = -2,
        StyleClassType_Normal = -1,

        StyleClassType_Hovered = 0,
        StyleClassType_Pushed,
        StyleClassType_Focused,
        StyleClassType_Disabled,

        StyleClassType_End,
    };

    /// The number of modifier class types.
    static const int StyleClassTypeCount = static_cast<int>(StyleClassType_End);

    
    /// Converts a string to a style class type. Returns StyleClassType_None if the string can not be converted.
    StyleClassType ToStyleClassType(const char *name, ptrdiff_t nameSizeInBytes = -1);


    
    // Class representing a style class attached to an element.
    class StyleClass
    {
    public:
    
        /**
        *   \brief  Constructor.
        */
        StyleClass(StyleServer &server, const char* name, ptrdiff_t nameSizeInTs = -1);
        
        /**
        *   \brief  Destructor for deallocating style attribute values.
        */
        ~StyleClass();
        

        /**
        *   \brief  Retrieves the stringified value of an attribute.
        */
        GTLib::String GetAttribute(const char* name, ptrdiff_t nameSizeInTs = -1);
        
        /**
        *   \brief                     Generically sets the value of an attribute.
        *   \param  name          [in] The name of the attribute being set.
        *   \param  nameSizeInTs  [in] The size in chars of the name. Use if \c name can not be null terminated.
        *   \param  value         [in] The value of the attribute, as a string.
        *   \param  valueSizeInTs [in] The size in chars of the value. Use if \c name can not be null terminated.
        *
        *   \remarks
        *       This method is fairly slow. Use a direct assignment if possible. This is mainly used for setting the attribute from a string.
        */
        bool SetAttribute(const char* nameIn, ptrdiff_t nameSizeInTs, const char* value, ptrdiff_t valueSizeInTs = -1);
        bool SetAttribute(const char* nameIn, const char* value) { return this->SetAttribute(nameIn, -1, value, -1); }
        
        /**
        *   \brief  Appends the given includes.
        */
        void AppendIncludes(const char* newIncludes);

        /// Removes the given includes.
        ///
        /// @param includesToRemove [in] The includes string to remove.
        void RemoveIncludes(const char* includesToRemove);


        /**
        *   \brief  Sets the value of a modifier style class.
        */
        void SetModifierClass(StyleClass &modifier);

        /**
        *   \brief  Unsets a modifier style class attribute.
        */
        void UnsetModifierClass(StyleClassType modifierType);

        /**
        *   \brief  Retrieves the modifier style class.
        */
        StyleClass* GetModifierClass(StyleClassType type);


        /**
        *   \brief  Retrieves the array of modifier classes.
        */
        StyleAttribute_StyleClass* GetModifierClassAttributes() { return this->modifiers; }

        
    public:

        /// The style server that owns this style class.
        StyleServer &server;
    
        // The name of the class.
        char* name;
        
        // The type of this style class.
        StyleClassType type;

        // The includes. This is just a string. The classes in this string will be added/removed from an elements style stack when
        // this class is added/removed.
        char* includes;
        
        // We need to store a list of style stacks that have this class in them. This is because
        // we need to notify the stacks when a change is made to the style class.
        GTLib::List<StyleStack*> hosts;
        
        // Size.
        StyleAttribute_Number  width;
        StyleAttribute_Number  height;
        StyleAttribute_Number  minWidth;
        StyleAttribute_Number  maxWidth;
        StyleAttribute_Number  minHeight;
        StyleAttribute_Number  maxHeight;
        StyleAttribute_String  relativeWidthMode;
        StyleAttribute_String  relativeHeightMode;
        StyleAttribute_Boolean flexChildWidth;
        StyleAttribute_Boolean flexChildHeight;
        
        // Background.
        StyleAttribute_Colour backgroundColour;
        StyleAttribute_String backgroundImage;
        StyleAttribute_Colour backgroundImageColour;
        StyleAttribute_String backgroundAlignX;     // left/center/right
        StyleAttribute_String backgroundAlignY;     // top/center/bottom
        StyleAttribute_String backgroundRepeatX;    // none/repeat/stretch
        StyleAttribute_String backgroundRepeatY;    // none/repeat/stretch
        
        // Borders.
        StyleAttribute_Number borderLeftWidth;
        StyleAttribute_Colour borderLeftColour;
        StyleAttribute_Number borderRightWidth;
        StyleAttribute_Colour borderRightColour;
        StyleAttribute_Number borderTopWidth;
        StyleAttribute_Colour borderTopColour;
        StyleAttribute_Number borderBottomWidth;
        StyleAttribute_Colour borderBottomColour;
        
        // Padding.
        StyleAttribute_Number paddingLeft;
        StyleAttribute_Number paddingRight;
        StyleAttribute_Number paddingTop;
        StyleAttribute_Number paddingBottom;
        
        // Margin.
        StyleAttribute_Number marginLeft;
        StyleAttribute_Number marginRight;
        StyleAttribute_Number marginTop;
        StyleAttribute_Number marginBottom;
        
        // Other stuff.
        StyleAttribute_Plane   childPlane;
        StyleAttribute_Align   horizontalAlign;
        StyleAttribute_Align   verticalAlign;
        StyleAttribute_String  cursor;
        StyleAttribute_Boolean visible;
        StyleAttribute_Number  zIndex;
        StyleAttribute_Boolean transparentMouseInput;
        StyleAttribute_Boolean enabled;
        StyleAttribute_Colour  textCursorColour;
        StyleAttribute_Boolean canReceiveFocusFromMouse;
        
        // Positioning.
        StyleAttribute_Positioning    positioning;
        StyleAttribute_Number         left;
        StyleAttribute_Number         right;
        StyleAttribute_Number         top;
        StyleAttribute_Number         bottom;
        StyleAttribute_PositionOrigin positionOrigin;
        StyleAttribute_Number         innerOffsetX;
        StyleAttribute_Number         innerOffsetY;
        
        // Fonts/Text.
        StyleAttribute_String     fontFamily;
        StyleAttribute_Number     fontSize;
        StyleAttribute_FontWeight fontWeight;
        StyleAttribute_FontSlant  fontSlant;
        StyleAttribute_Colour     textColour;
        StyleAttribute_Colour     textSelectionColour;
        StyleAttribute_Colour     textSelectionBackgroundColour;
        StyleAttribute_Colour     textSelectionBackgroundColourBlurred;

        // Text editing.
        StyleAttribute_Boolean editableText;
        StyleAttribute_Boolean singleLineText;

        // Opacity/Transparency.
        StyleAttribute_Number  opacity;
        StyleAttribute_Boolean compoundOpacity;

        // Shadows.
        StyleAttribute_Boolean enableShadow;
        StyleAttribute_Colour  shadowColour;
        StyleAttribute_Number  shadowBlurRadius;
        StyleAttribute_Number  shadowOffsetX;
        StyleAttribute_Number  shadowOffsetY;
        StyleAttribute_Number  shadowExtrusionX;
        StyleAttribute_Number  shadowExtrusionY;
        StyleAttribute_Number  shadowOpacity;

        // Dragging/Moving/Resizing.
        StyleAttribute_Boolean allowMouseDrag;
        StyleAttribute_Boolean constrainMouseDragX;
        StyleAttribute_Boolean constrainMouseDragY;
        StyleAttribute_String  mouseDragClampModeX;
        StyleAttribute_String  mouseDragClampModeY;
        StyleAttribute_Boolean allowMouseResize;
        StyleAttribute_Number  leftGripperWidth;
        StyleAttribute_Number  rightGripperWidth;
        StyleAttribute_Number  topGripperWidth;
        StyleAttribute_Number  bottomGripperWidth;


        // Modifier classes. These are indexed by StyleClassType.
        StyleAttribute_StyleClass modifiers[StyleClassTypeCount];
    
        
        // Whether or not 'right' has priority over 'left'.
        bool rightHasPriority;

        // Whether or not 'bottom' has priority over 'top'.
        bool bottomHasPriority;
        
        
        
        /// Properties in this structure are used for efficiently storing the style class in a StyleClassTree object.
        struct BST
        {
            /// Default constructor.
            BST()
                : hashedName(0), left(nullptr), right(nullptr), parent(nullptr)
            {
            }
            
            
            /// The hashed name. This will be set when the name is set.
            uint32_t hashedName;
            
            /// The left side child in the BST.
            StyleClass* left;
            
            /// The right side child in the BST.
            StyleClass* right;
            
            /// The parent in the BST.
            StyleClass* parent;
            
        }bst;
        
    private:    // No copying.
        StyleClass(const StyleClass &);
        StyleClass & operator=(const StyleClass &);
    };
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
