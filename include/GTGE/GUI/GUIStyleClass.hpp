// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIStyleClass
#define GT_GUIStyleClass

#include <GTGE/Script.hpp>
#include <GTGE/Core/List.hpp>
#include "GUIStyleAttribute.hpp"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4480)   // nonstandard extension used: specifying underlying type for enum.
#endif

namespace GT
{
    class GUIStyleStack;
    class GUIStyleServer;

    // !!! Be careful here. These values are used as indices into arrays (except GUIStyleClassType_Normal). !!!
    enum GUIStyleClassType : int
    {
        GUIStyleClassType_None   = -2,
        GUIStyleClassType_Normal = -1,

        GUIStyleClassType_Hovered = 0,
        GUIStyleClassType_Pushed,
        GUIStyleClassType_Focused,
        GUIStyleClassType_Disabled,

        GUIStyleClassType_End,
    };

    /// The number of modifier class types.
    static const int StyleClassTypeCount = static_cast<int>(GUIStyleClassType_End);

    
    /// Converts a string to a style class type. Returns GUIStyleClassType_None if the string can not be converted.
    GUIStyleClassType ToStyleClassType(const char *name, ptrdiff_t nameSizeInBytes = -1);


    
    // Class representing a style class attached to an element.
    class GUIStyleClass
    {
    public:
    
        /**
        *   \brief  Constructor.
        */
        GUIStyleClass(GUIStyleServer &server, const char* name, ptrdiff_t nameSizeInTs = -1);
        
        /**
        *   \brief  Destructor for deallocating style attribute values.
        */
        ~GUIStyleClass();
        

        /**
        *   \brief  Retrieves the stringified value of an attribute.
        */
        GT::String GetAttribute(const char* name, ptrdiff_t nameSizeInTs = -1);
        
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
        void SetModifierClass(GUIStyleClass &modifier);

        /**
        *   \brief  Unsets a modifier style class attribute.
        */
        void UnsetModifierClass(GUIStyleClassType modifierType);

        /**
        *   \brief  Retrieves the modifier style class.
        */
        GUIStyleClass* GetModifierClass(GUIStyleClassType type);


        /**
        *   \brief  Retrieves the array of modifier classes.
        */
        GUIStyleAttribute_StyleClass* GetModifierClassAttributes() { return this->modifiers; }

        
    public:

        /// The style server that owns this style class.
        GUIStyleServer &server;
    
        // The name of the class.
        char* name;
        
        // The type of this style class.
        GUIStyleClassType type;

        // The includes. This is just a string. The classes in this string will be added/removed from an elements style stack when
        // this class is added/removed.
        char* includes;
        
        // We need to store a list of style stacks that have this class in them. This is because
        // we need to notify the stacks when a change is made to the style class.
        GT::List<GUIStyleStack*> hosts;
        
        // Size.
        GUIStyleAttribute_Number  width;
        GUIStyleAttribute_Number  height;
        GUIStyleAttribute_Number  minWidth;
        GUIStyleAttribute_Number  maxWidth;
        GUIStyleAttribute_Number  minHeight;
        GUIStyleAttribute_Number  maxHeight;
        GUIStyleAttribute_String  relativeWidthMode;
        GUIStyleAttribute_String  relativeHeightMode;
        GUIStyleAttribute_Boolean flexChildWidth;
        GUIStyleAttribute_Boolean flexChildHeight;
        
        // Background.
        GUIStyleAttribute_Colour backgroundColour;
        GUIStyleAttribute_String backgroundImage;
        GUIStyleAttribute_Colour backgroundImageColour;
        GUIStyleAttribute_String backgroundAlignX;     // left/center/right
        GUIStyleAttribute_String backgroundAlignY;     // top/center/bottom
        GUIStyleAttribute_String backgroundRepeatX;    // none/repeat/stretch
        GUIStyleAttribute_String backgroundRepeatY;    // none/repeat/stretch
        
        // Borders.
        GUIStyleAttribute_Number borderLeftWidth;
        GUIStyleAttribute_Colour borderLeftColour;
        GUIStyleAttribute_Number borderRightWidth;
        GUIStyleAttribute_Colour borderRightColour;
        GUIStyleAttribute_Number borderTopWidth;
        GUIStyleAttribute_Colour borderTopColour;
        GUIStyleAttribute_Number borderBottomWidth;
        GUIStyleAttribute_Colour borderBottomColour;
        
        // Padding.
        GUIStyleAttribute_Number paddingLeft;
        GUIStyleAttribute_Number paddingRight;
        GUIStyleAttribute_Number paddingTop;
        GUIStyleAttribute_Number paddingBottom;
        
        // Margin.
        GUIStyleAttribute_Number marginLeft;
        GUIStyleAttribute_Number marginRight;
        GUIStyleAttribute_Number marginTop;
        GUIStyleAttribute_Number marginBottom;
        
        // Other stuff.
        GUIStyleAttribute_Plane   childPlane;
        GUIStyleAttribute_Align   horizontalAlign;
        GUIStyleAttribute_Align   verticalAlign;
        GUIStyleAttribute_String  cursor;
        GUIStyleAttribute_Boolean visible;
        GUIStyleAttribute_Number  zIndex;
        GUIStyleAttribute_Boolean transparentMouseInput;
        GUIStyleAttribute_Boolean enabled;
        GUIStyleAttribute_Colour  textCursorColour;
        GUIStyleAttribute_Boolean canReceiveFocusFromMouse;
        
        // Positioning.
        GUIStyleAttribute_Positioning    positioning;
        GUIStyleAttribute_Number         left;
        GUIStyleAttribute_Number         right;
        GUIStyleAttribute_Number         top;
        GUIStyleAttribute_Number         bottom;
        GUIStyleAttribute_PositionOrigin positionOrigin;
        GUIStyleAttribute_Number         innerOffsetX;
        GUIStyleAttribute_Number         innerOffsetY;
        
        // Fonts/Text.
        GUIStyleAttribute_String     fontFamily;
        GUIStyleAttribute_Number     fontSize;
        GUIStyleAttribute_FontWeight fontWeight;
        GUIStyleAttribute_FontSlant  fontSlant;
        GUIStyleAttribute_Colour     textColour;
        GUIStyleAttribute_Colour     textSelectionColour;
        GUIStyleAttribute_Colour     textSelectionBackgroundColour;
        GUIStyleAttribute_Colour     textSelectionBackgroundColourBlurred;

        // Text editing.
        GUIStyleAttribute_Boolean editableText;
        GUIStyleAttribute_Boolean singleLineText;

        // Opacity/Transparency.
        GUIStyleAttribute_Number  opacity;
        GUIStyleAttribute_Boolean compoundOpacity;

        // Shadows.
        GUIStyleAttribute_Boolean enableShadow;
        GUIStyleAttribute_Colour  shadowColour;
        GUIStyleAttribute_Number  shadowBlurRadius;
        GUIStyleAttribute_Number  shadowOffsetX;
        GUIStyleAttribute_Number  shadowOffsetY;
        GUIStyleAttribute_Number  shadowExtrusionX;
        GUIStyleAttribute_Number  shadowExtrusionY;
        GUIStyleAttribute_Number  shadowOpacity;

        // Dragging/Moving/Resizing.
        GUIStyleAttribute_Boolean allowMouseDrag;
        GUIStyleAttribute_Boolean constrainMouseDragX;
        GUIStyleAttribute_Boolean constrainMouseDragY;
        GUIStyleAttribute_String  mouseDragClampModeX;
        GUIStyleAttribute_String  mouseDragClampModeY;
        GUIStyleAttribute_Boolean allowMouseResize;
        GUIStyleAttribute_Number  leftGripperWidth;
        GUIStyleAttribute_Number  rightGripperWidth;
        GUIStyleAttribute_Number  topGripperWidth;
        GUIStyleAttribute_Number  bottomGripperWidth;


        // Modifier classes. These are indexed by GUIStyleClassType.
        GUIStyleAttribute_StyleClass modifiers[StyleClassTypeCount];
    
        
        // Whether or not 'right' has priority over 'left'.
        bool rightHasPriority;

        // Whether or not 'bottom' has priority over 'top'.
        bool bottomHasPriority;
        
        
        
        /// Properties in this structure are used for efficiently storing the style class in a GUIStyleClassTree object.
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
            GUIStyleClass* left;
            
            /// The right side child in the BST.
            GUIStyleClass* right;
            
            /// The parent in the BST.
            GUIStyleClass* parent;
            
        }bst;
        
    private:    // No copying.
        GUIStyleClass(const GUIStyleClass &);
        GUIStyleClass & operator=(const GUIStyleClass &);
    };
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
