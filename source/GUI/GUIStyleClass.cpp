// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/GUIStyleClass.hpp>
#include <GTEngine/GUI/GUIStyleServer.hpp>
#include <GTEngine/Core/Strings/Create.hpp>
#include "GUIStyleAttributeHandler.hpp"

namespace GT
{
    GUIStyleClassType ToStyleClassType(const char *name, ptrdiff_t nameSizeInBytes)
    {
        if (GT::Strings::Equal(name, nameSizeInBytes, "hovered"))
        {
            return GUIStyleClassType_Hovered;
        }

        if (GT::Strings::Equal(name, nameSizeInBytes, "pushed"))
        {
            return GUIStyleClassType_Pushed;
        }

        if (GT::Strings::Equal(name, nameSizeInBytes, "focused"))
        {
            return GUIStyleClassType_Focused;
        }

        if (GT::Strings::Equal(name, nameSizeInBytes, "disabled"))
        {
            return GUIStyleClassType_Disabled;
        }

        return GUIStyleClassType_None;
    }
}

namespace GT
{
    GUIStyleClass::GUIStyleClass(GUIStyleServer &server, const char* name, ptrdiff_t nameSizeInBytes)
        : server(server), name(GT::Strings::Create(name, nameSizeInBytes)), type(GUIStyleClassType_Normal), includes(nullptr), hosts(),
          width(), height(), minWidth(), maxWidth(), minHeight(), maxHeight(), relativeWidthMode(), relativeHeightMode(), flexChildWidth(), flexChildHeight(),
          backgroundColour(), backgroundImage(),backgroundImageColour(), backgroundAlignX(), backgroundAlignY(), backgroundRepeatX(), backgroundRepeatY(),
          borderLeftWidth(), borderLeftColour(),
          borderRightWidth(), borderRightColour(),
          borderTopWidth(), borderTopColour(),
          borderBottomWidth(), borderBottomColour(),
          paddingLeft(), paddingRight(), paddingTop(), paddingBottom(),
          marginLeft(), marginRight(), marginTop(), marginBottom(),
          childPlane(),
          horizontalAlign(), verticalAlign(),
          cursor(),
          visible(),
          zIndex(),
          transparentMouseInput(),
          enabled(),
          textCursorColour(),
          canReceiveFocusFromMouse(),
          positioning(), left(), right(), top(), bottom(), positionOrigin(), innerOffsetX(), innerOffsetY(),
          fontFamily(), fontSize(), fontWeight(), fontSlant(), textColour(), textSelectionColour(), textSelectionBackgroundColour(), textSelectionBackgroundColourBlurred(),
          editableText(), singleLineText(),
          opacity(), compoundOpacity(),
          enableShadow(), shadowColour(), shadowBlurRadius(), shadowOffsetX(), shadowOffsetY(), shadowExtrusionX(), shadowExtrusionY(), shadowOpacity(),
          allowMouseDrag(), constrainMouseDragX(), constrainMouseDragY(), mouseDragClampModeX(), mouseDragClampModeY(),
          allowMouseResize(), leftGripperWidth(), rightGripperWidth(), topGripperWidth(), bottomGripperWidth(),
          modifiers(),
          rightHasPriority(false), bottomHasPriority(false),
          bst()
    {
        if (nameSizeInBytes == -1)
        {
            bst.hashedName = GT::Hash(name);
        }
        else
        {
            bst.hashedName = GT::Hash(name, nameSizeInBytes);
        }
    }
    
    GUIStyleClass::~GUIStyleClass()
    {
        GT::Strings::Delete(this->name);
        GT::Strings::Delete(this->includes);
    }

    GT::String GUIStyleClass::GetAttribute(const char* nameIn, ptrdiff_t nameSizeInTs)
    {
        return this->server.GetGUIStyleAttribute(*this, nameIn, nameSizeInTs);
    }
    
    bool GUIStyleClass::SetAttribute(const char* nameIn, ptrdiff_t nameSizeInTs, const char *value, ptrdiff_t valueSizeInTs)
    {
        return this->server.SetGUIStyleAttribute(*this, nameIn, nameSizeInTs, value, valueSizeInTs);
    }

    void GUIStyleClass::AppendIncludes(const char *newIncludes)
    {
        // We won't do anything if we don't have any includes.
        if (newIncludes != nullptr && !GT::Strings::Equal(newIncludes, ""))
        {
            // We need to grab the old includes so we can delete the string later.
            auto oldIncludes = this->includes;

            GT::Strings::List<char> newIncludesList;

            if (oldIncludes != nullptr)
            {
                newIncludesList.Append(oldIncludes);
                newIncludesList.Append(" ");
            }

            newIncludesList.Append(newIncludes);

            this->includes = GT::Strings::Create(newIncludesList);

            GT::Strings::Delete(oldIncludes);
        }
    }

    void GUIStyleClass::RemoveIncludes(const char* includesToRemove)
    {
        auto result = GT::Strings::FindFirst(this->includes, includesToRemove);
        if (result != nullptr)
        {
            // We need to grab the old includes so we can delete the string later.
            auto oldIncludes = this->includes;

            GT::Strings::List<char> newIncludesList;
            
            if (result != this->includes)
            {
                newIncludesList.Append(this->includes, result - this->includes);
            }

            size_t includesToRemoveSize = GT::Strings::SizeInTs(includesToRemove);
            if (*(result + includesToRemoveSize) != '\0')
            {
                newIncludesList.Append(result + includesToRemoveSize);
            }
            
            this->includes = GT::Strings::Create(newIncludesList);


            GT::Strings::Delete(oldIncludes);
        }
    }

    void GUIStyleClass::SetModifierClass(GUIStyleClass& modifier)
    {
        if (modifier.type >= 0 && modifier.type < GUIStyleClassType_End)
        {
            auto &modifierAttrib = this->modifiers[modifier.type];
        
            modifierAttrib.isset = true;
            modifierAttrib.value = &modifier;
        }
    }

    void GUIStyleClass::UnsetModifierClass(GUIStyleClassType typeIn)
    {
        if (typeIn >= 0 && typeIn < GUIStyleClassType_End)
        {
            auto &modifierAttrib = this->modifiers[typeIn];

            modifierAttrib.isset = false;
            modifierAttrib.value = nullptr;
        }
    }

    GUIStyleClass* GUIStyleClass::GetModifierClass(GUIStyleClassType typeIn)
    {
        if (typeIn >= 0 && typeIn < GUIStyleClassType_End)
        {
            auto &modifierAttrib = this->modifiers[typeIn];

            if (modifierAttrib.isset)
            {
                return modifierAttrib.value;
            }
        }

        return nullptr;
    }
}
