// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/StyleClass.hpp>
#include <GTEngine/GUI/StyleServer.hpp>
#include <GTEngine/Core/Strings/Create.hpp>
//#include <GTEngine/Core/Math.hpp>

#include "StyleAttributeHandler.hpp"

namespace GTGUI
{
    StyleClassType ToStyleClassType(const char *name, ptrdiff_t nameSizeInBytes)
    {
        if (GTLib::Strings::Equal(name, nameSizeInBytes, "hovered"))
        {
            return StyleClassType_Hovered;
        }

        if (GTLib::Strings::Equal(name, nameSizeInBytes, "pushed"))
        {
            return StyleClassType_Pushed;
        }

        if (GTLib::Strings::Equal(name, nameSizeInBytes, "focused"))
        {
            return StyleClassType_Focused;
        }

        if (GTLib::Strings::Equal(name, nameSizeInBytes, "disabled"))
        {
            return StyleClassType_Disabled;
        }

        return StyleClassType_None;
    }
}

namespace GTGUI
{
    StyleClass::StyleClass(StyleServer &server, const char* name, ptrdiff_t nameSizeInBytes)
        : server(server), name(GTLib::Strings::Create(name, nameSizeInBytes)), type(StyleClassType_Normal), includes(nullptr), hosts(),
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
            bst.hashedName = GTLib::Hash(name);
        }
        else
        {
            bst.hashedName = GTLib::Hash(name, nameSizeInBytes);
        }
    }
    
    StyleClass::~StyleClass()
    {
        GTLib::Strings::Delete(this->name);
        GTLib::Strings::Delete(this->includes);
    }

    GTLib::String StyleClass::GetAttribute(const char* nameIn, ptrdiff_t nameSizeInTs)
    {
        return this->server.GetStyleAttribute(*this, nameIn, nameSizeInTs);
    }
    
    bool StyleClass::SetAttribute(const char* nameIn, ptrdiff_t nameSizeInTs, const char *value, ptrdiff_t valueSizeInTs)
    {
        return this->server.SetStyleAttribute(*this, nameIn, nameSizeInTs, value, valueSizeInTs);
    }

    void StyleClass::AppendIncludes(const char *newIncludes)
    {
        // We won't do anything if we don't have any includes.
        if (newIncludes != nullptr && !GTLib::Strings::Equal(newIncludes, ""))
        {
            // We need to grab the old includes so we can delete the string later.
            auto oldIncludes = this->includes;

            GTLib::Strings::List<char> newIncludesList;

            if (oldIncludes != nullptr)
            {
                newIncludesList.Append(oldIncludes);
                newIncludesList.Append(" ");
            }

            newIncludesList.Append(newIncludes);

            this->includes = GTLib::Strings::Create(newIncludesList);

            GTLib::Strings::Delete(oldIncludes);
        }
    }

    void StyleClass::RemoveIncludes(const char* includesToRemove)
    {
        auto result = GTLib::Strings::FindFirst(this->includes, includesToRemove);
        if (result != nullptr)
        {
            // We need to grab the old includes so we can delete the string later.
            auto oldIncludes = this->includes;

            GTLib::Strings::List<char> newIncludesList;
            
            if (result != this->includes)
            {
                newIncludesList.Append(this->includes, result - this->includes);
            }

            size_t includesToRemoveSize = GTLib::Strings::SizeInTs(includesToRemove);
            if (*(result + includesToRemoveSize) != '\0')
            {
                newIncludesList.Append(result + includesToRemoveSize);
            }
            
            this->includes = GTLib::Strings::Create(newIncludesList);


            GTLib::Strings::Delete(oldIncludes);
        }
    }

    void StyleClass::SetModifierClass(StyleClass& modifier)
    {
        if (modifier.type >= 0 && modifier.type < StyleClassType_End)
        {
            auto &modifierAttrib = this->modifiers[modifier.type];
        
            modifierAttrib.isset = true;
            modifierAttrib.value = &modifier;
        }
    }

    void StyleClass::UnsetModifierClass(StyleClassType typeIn)
    {
        if (typeIn >= 0 && typeIn < StyleClassType_End)
        {
            auto &modifierAttrib = this->modifiers[typeIn];

            modifierAttrib.isset = false;
            modifierAttrib.value = nullptr;
        }
    }

    StyleClass* StyleClass::GetModifierClass(StyleClassType typeIn)
    {
        if (typeIn >= 0 && typeIn < StyleClassType_End)
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
