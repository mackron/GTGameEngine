// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include "GUIStyleAttributeHandler.hpp"
#include <GTEngine/GUI/GUIStyleStack.hpp>
#include <GTEngine/GUI/GUIElement.hpp>
#include <GTEngine/GUI/GUIServer.hpp>
#include <cassert>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behavior: elements of array 'GTGUI::GUIStyleStack::modifiers' will be default initialized
#endif

#define REFRESH_ATTRIBUTE(attribute) \
    for (auto i = this->classes.root; i != nullptr; i = i->next) \
    { \
        auto value = i->value; \
        if (value && value->attribute.isset) \
        { \
            this->attribute = &value->attribute; \
            break; \
        } \
    }

#define ACTIVATE_MODIFIER_CLASSES(modifier) \
    bool stackChanged = false; \
    \
    this->LockRefresh(); \
    { \
        for (auto i = this->classes.last; i != nullptr; i = i->prev) \
        { \
            if (i->value->modifier.isset) \
            { \
                this->classes.Prepend(i->value->modifier.value); \
                stackChanged = true; \
            } \
        } \
    } \
    this->UnlockRefresh(); \
    \
    if (stackChanged) \
    { \
        this->Refresh(); \
    }

namespace GTGUI
{
    GUIStyleStack::GUIStyleStack(GUIElement &owner)
        : owner(owner),
          classes(),
          width(nullptr), height(nullptr), minWidth(nullptr), maxWidth(nullptr), minHeight(nullptr), maxHeight(nullptr), relativeWidthMode(nullptr), relativeHeightMode(nullptr), flexChildWidth(nullptr), flexChildHeight(nullptr),
          backgroundColour(nullptr), backgroundImage(nullptr), backgroundImageColour(nullptr), backgroundAlignX(nullptr), backgroundAlignY(nullptr), backgroundRepeatX(nullptr), backgroundRepeatY(nullptr),
          borderLeftWidth(nullptr), borderRightWidth(nullptr), borderTopWidth(nullptr), borderBottomWidth(nullptr),
          borderLeftColour(nullptr), borderRightColour(nullptr), borderTopColour(nullptr), borderBottomColour(nullptr),
          paddingLeft(nullptr), paddingRight(nullptr), paddingTop(nullptr), paddingBottom(nullptr),
          marginLeft(nullptr), marginRight(nullptr), marginTop(nullptr), marginBottom(nullptr),
          childPlane(nullptr),
          horizontalAlign(nullptr), verticalAlign(nullptr),
          cursor(nullptr),
          visible(nullptr),
          zIndex(nullptr),
          transparentMouseInput(nullptr),
          enabled(nullptr),
          textCursorColour(nullptr),
          canReceiveFocusFromMouse(nullptr),
          positioning(nullptr),
          left(nullptr), right(nullptr), top(nullptr), bottom(nullptr), positionOrigin(nullptr), innerOffsetX(nullptr), innerOffsetY(nullptr),
          fontFamily(nullptr), fontSize(nullptr), fontWeight(nullptr), fontSlant(nullptr), textColour(nullptr), textSelectionColour(nullptr), textSelectionBackgroundColour(nullptr), textSelectionBackgroundColourBlurred(nullptr),
          editableText(nullptr), singleLineText(nullptr),
          opacity(nullptr), compoundOpacity(nullptr),
          enableShadow(nullptr), shadowColour(nullptr), shadowBlurRadius(nullptr), shadowOffsetX(nullptr), shadowOffsetY(nullptr), shadowExtrusionX(nullptr), shadowExtrusionY(nullptr), shadowOpacity(nullptr),
          allowMouseDrag(nullptr), constrainMouseDragX(nullptr), constrainMouseDragY(nullptr), mouseDragClampModeX(nullptr), mouseDragClampModeY(nullptr),
          allowMouseResize(nullptr), leftGripperWidth(nullptr), rightGripperWidth(nullptr), topGripperWidth(nullptr), bottomGripperWidth(nullptr),
          rightHasPriority(false), bottomHasPriority(false),
          lockCount(0),
          modifiers()
    {
        // We default everything to the default style. We need valid pointers straight after construction.
        auto defaultStyle = owner.GetServer().GetStyleServer().GetDefaultStyleClass();
        if (defaultStyle != nullptr)
        {
            width                                = &defaultStyle->width;
            height                               = &defaultStyle->height;
            minWidth                             = &defaultStyle->minWidth;
            maxWidth                             = &defaultStyle->maxWidth;
            minHeight                            = &defaultStyle->minHeight;
            maxHeight                            = &defaultStyle->maxHeight;
            relativeWidthMode                    = &defaultStyle->relativeWidthMode;
            relativeHeightMode                   = &defaultStyle->relativeHeightMode;
            flexChildWidth                       = &defaultStyle->flexChildWidth;
            flexChildHeight                      = &defaultStyle->flexChildHeight;
            backgroundColour                     = &defaultStyle->backgroundColour;
            backgroundImage                      = &defaultStyle->backgroundImage;
            backgroundImageColour                = &defaultStyle->backgroundImageColour;
            backgroundAlignX                     = &defaultStyle->backgroundAlignX;
            backgroundAlignY                     = &defaultStyle->backgroundAlignY;
            backgroundRepeatX                    = &defaultStyle->backgroundRepeatX;
            backgroundRepeatY                    = &defaultStyle->backgroundRepeatY;
            borderLeftWidth                      = &defaultStyle->borderLeftWidth;
            borderRightWidth                     = &defaultStyle->borderRightWidth;
            borderTopWidth                       = &defaultStyle->borderTopWidth;
            borderBottomWidth                    = &defaultStyle->borderBottomWidth;
            borderLeftColour                     = &defaultStyle->borderLeftColour;
            borderRightColour                    = &defaultStyle->borderRightColour;
            borderTopColour                      = &defaultStyle->borderTopColour;
            borderBottomColour                   = &defaultStyle->borderBottomColour;
            paddingLeft                          = &defaultStyle->paddingLeft;
            paddingRight                         = &defaultStyle->paddingRight;
            paddingTop                           = &defaultStyle->paddingTop;
            paddingBottom                        = &defaultStyle->paddingBottom;
            marginLeft                           = &defaultStyle->marginLeft;
            marginRight                          = &defaultStyle->marginRight;
            marginTop                            = &defaultStyle->marginTop;
            marginBottom                         = &defaultStyle->marginBottom;
            childPlane                           = &defaultStyle->childPlane;
            horizontalAlign                      = &defaultStyle->horizontalAlign;
            verticalAlign                        = &defaultStyle->verticalAlign;
            cursor                               = &defaultStyle->cursor;
            visible                              = &defaultStyle->visible;
            zIndex                               = &defaultStyle->zIndex;
            transparentMouseInput                = &defaultStyle->transparentMouseInput;
            enabled                              = &defaultStyle->enabled;
            textCursorColour                     = &defaultStyle->textCursorColour;
            canReceiveFocusFromMouse             = &defaultStyle->canReceiveFocusFromMouse;
            positioning                          = &defaultStyle->positioning;
            left                                 = &defaultStyle->left;
            right                                = &defaultStyle->right;
            top                                  = &defaultStyle->top;
            bottom                               = &defaultStyle->bottom;
            positionOrigin                       = &defaultStyle->positionOrigin;
            innerOffsetX                         = &defaultStyle->innerOffsetX;
            innerOffsetY                         = &defaultStyle->innerOffsetY;
            fontFamily                           = &defaultStyle->fontFamily;
            fontSize                             = &defaultStyle->fontSize;
            fontWeight                           = &defaultStyle->fontWeight;
            fontSlant                            = &defaultStyle->fontSlant;
            textColour                           = &defaultStyle->textColour;
            textSelectionColour                  = &defaultStyle->textSelectionColour;
            textSelectionBackgroundColour        = &defaultStyle->textSelectionBackgroundColour;
            textSelectionBackgroundColourBlurred = &defaultStyle->textSelectionBackgroundColourBlurred;
            editableText                         = &defaultStyle->editableText;
            singleLineText                       = &defaultStyle->singleLineText;
            opacity                              = &defaultStyle->opacity;
            compoundOpacity                      = &defaultStyle->compoundOpacity;
            enableShadow                         = &defaultStyle->enableShadow;
            shadowColour                         = &defaultStyle->shadowColour;
            shadowBlurRadius                     = &defaultStyle->shadowBlurRadius;
            shadowOffsetX                        = &defaultStyle->shadowOffsetX;
            shadowOffsetY                        = &defaultStyle->shadowOffsetY;
            shadowExtrusionX                     = &defaultStyle->shadowExtrusionX;
            shadowExtrusionY                     = &defaultStyle->shadowExtrusionY;
            shadowOpacity                        = &defaultStyle->shadowOpacity;
            allowMouseDrag                       = &defaultStyle->allowMouseDrag;
            constrainMouseDragX                  = &defaultStyle->constrainMouseDragX;
            constrainMouseDragY                  = &defaultStyle->constrainMouseDragY;
            mouseDragClampModeX                  = &defaultStyle->mouseDragClampModeX;
            mouseDragClampModeY                  = &defaultStyle->mouseDragClampModeY;
            allowMouseResize                     = &defaultStyle->allowMouseResize;
            leftGripperWidth                     = &defaultStyle->leftGripperWidth;
            rightGripperWidth                    = &defaultStyle->rightGripperWidth;
            topGripperWidth                      = &defaultStyle->topGripperWidth;
            bottomGripperWidth                   = &defaultStyle->bottomGripperWidth;
        }
    }

    GUIStyleStack::~GUIStyleStack()
    {
        // We need to detach ourselves from every style class that is attached to this stack.
        while (this->classes.root != nullptr)
        {
            auto styleClass = this->classes.root->value;
            assert(styleClass != nullptr);

            styleClass->hosts.Remove(styleClass->hosts.Find(this));

            this->classes.RemoveRoot();
        }
    }

    void GUIStyleStack::Attach(GUIStyleClass &style, bool refresh)
    {
        // We need to check if the style class already exists. If it does, we just move it to the front.
        auto iClass = this->classes.Find(&style);
        if (iClass != nullptr)
        {
            // We need to detach before re-attaching.
            this->classes.Remove(iClass);
            style.hosts.Remove(style.hosts.Find(this));
        }

        // This stack is now a host for the given style.
        style.hosts.Append(this);

        this->classes.Prepend(&style);


        // When we attach this style class we need to apply modifier classes that are already applied.
        for (int i = 0; i < StyleClassTypeCount; ++i)
        {
            if (this->modifiers[i])
            {
                this->ActivateModifierClasses(static_cast<GUIStyleClassType>(i));
            }
        }


        if (refresh)
        {
            this->Refresh();
        }
    }

    void GUIStyleStack::Detach(GUIStyleClass &style, bool refresh)
    {
        // We're no longer a host for the given style.
        style.hosts.Remove(style.hosts.Find(this));

        this->classes.Remove(this->classes.Find(&style));

        if (refresh)
        {
            this->Refresh();
        }
    }

    GUIStyleClass * GUIStyleStack::FirstStyleClass()
    {
        if (this->classes.root)
        {
            return this->classes.root->value;
        }

        return nullptr;
    }


    void GUIStyleStack::ActivateModifierClasses(GUIStyleClassType type)
    {
        bool stackChanged = false;

        this->LockRefresh();
        {
            for (auto i = this->classes.last; i != nullptr; i = i->prev)
            {
                auto &modifier = i->value->modifiers[type];
                if (modifier.isset)
                {
                    this->classes.Prepend(modifier.value);
                    stackChanged = true;
                }
            }

            this->modifiers[type] = true;
        }
        this->UnlockRefresh();

        if (stackChanged)
        {
            this->Refresh();
        }
    }

    void GUIStyleStack::DeactivateModifierClasses(GUIStyleClassType type)
    {
        bool stackChanged = false;

        // To 'deactivate' the modifier classes, we just iterate forwards and remove classes until we find the first one that is not what we want.
        for (auto i = this->classes.root; i != nullptr; )
        {
            if (i->value->type == type)
            {
                auto classToRemove = i;
                i = i->next;

                this->classes.Remove(classToRemove);
                stackChanged = true;
            }
            else
            {
                i = i->next;
            }
        }

        this->modifiers[type] = false;

        if (stackChanged)
        {
            this->Refresh();
        }
    }

    void GUIStyleStack::DeactivateAllModifierClasses()
    {
        bool stackChanged = false;

        // To 'deactivate' the modifier classes, we just iterate forwards and remove classes until we find the first one that is not what we want.
        for (auto i = this->classes.root; i != nullptr; )
        {
            if (i->value->type != GUIStyleClassType_None && i->value->type != GUIStyleClassType_Normal)
            {
                auto classToRemove = i;
                i = i->next;

                this->classes.Remove(classToRemove);
                stackChanged = true;
            }
            else
            {
                i = i->next;
            }
        }

        if (stackChanged)
        {
            this->Refresh();
        }
    }



    void GUIStyleStack::LockRefresh()
    {
        ++this->lockCount;
    }

    void GUIStyleStack::UnlockRefresh()
    {
        if (this->lockCount > 0)
        {
            --this->lockCount;
        }
    }

    void GUIStyleStack::Refresh()
    {
        // Don't refresh anything if refreshing is locked.
        if (!this->IsRefreshLocked())
        {
            // The 'enabled' attribute is a special case because it requires us to activate the 'disabled' modifier class, which
            // will also need to refresh the style.
            this->LockRefresh();
            {
                AttributeHandlers::enabled::RefreshStack(*this);
            }
            this->UnlockRefresh();

            // First we refresh every attribute. Note that we don't want to call Refresh*() here since that
            // causes a lot of redundant calculations.
            AttributeHandlers::width::RefreshStack(*this);
            AttributeHandlers::height::RefreshStack(*this);
            AttributeHandlers::minWidth::RefreshStack(*this);
            AttributeHandlers::maxWidth::RefreshStack(*this);
            AttributeHandlers::minHeight::RefreshStack(*this);
            AttributeHandlers::maxHeight::RefreshStack(*this);
            AttributeHandlers::relativeWidthMode::RefreshStack(*this);
            AttributeHandlers::relativeHeightMode::RefreshStack(*this);
            AttributeHandlers::flexChildWidth::RefreshStack(*this);
            AttributeHandlers::flexChildHeight::RefreshStack(*this);
            AttributeHandlers::backgroundColour::RefreshStack(*this);
            AttributeHandlers::backgroundImage::RefreshStack(*this);
            AttributeHandlers::backgroundImageColour::RefreshStack(*this);
            AttributeHandlers::backgroundAlignX::RefreshStack(*this);
            AttributeHandlers::backgroundAlignY::RefreshStack(*this);
            AttributeHandlers::backgroundRepeatX::RefreshStack(*this);
            AttributeHandlers::backgroundRepeatY::RefreshStack(*this);
            AttributeHandlers::borderLeftWidth::RefreshStack(*this);
            AttributeHandlers::borderRightWidth::RefreshStack(*this);
            AttributeHandlers::borderTopWidth::RefreshStack(*this);
            AttributeHandlers::borderBottomWidth::RefreshStack(*this);
            AttributeHandlers::borderLeftColour::RefreshStack(*this);
            AttributeHandlers::borderRightColour::RefreshStack(*this);
            AttributeHandlers::borderTopColour::RefreshStack(*this);
            AttributeHandlers::borderBottomColour::RefreshStack(*this);
            AttributeHandlers::paddingLeft::RefreshStack(*this);
            AttributeHandlers::paddingRight::RefreshStack(*this);
            AttributeHandlers::paddingTop::RefreshStack(*this);
            AttributeHandlers::paddingBottom::RefreshStack(*this);
            AttributeHandlers::marginLeft::RefreshStack(*this);
            AttributeHandlers::marginRight::RefreshStack(*this);
            AttributeHandlers::marginTop::RefreshStack(*this);
            AttributeHandlers::marginBottom::RefreshStack(*this);
            AttributeHandlers::childPlane::RefreshStack(*this);
            AttributeHandlers::horizontalAlign::RefreshStack(*this);
            AttributeHandlers::verticalAlign::RefreshStack(*this);
            AttributeHandlers::cursor::RefreshStack(*this);
            AttributeHandlers::visible::RefreshStack(*this);
            AttributeHandlers::zIndex::RefreshStack(*this);
            AttributeHandlers::transparentMouseInput::RefreshStack(*this);
            AttributeHandlers::textCursorColour::RefreshStack(*this);
            AttributeHandlers::canReceiveFocusFromMouse::RefreshStack(*this);
            AttributeHandlers::positioning::RefreshStack(*this);
            AttributeHandlers::left::RefreshStack(*this);
            AttributeHandlers::right::RefreshStack(*this);
            AttributeHandlers::top::RefreshStack(*this);
            AttributeHandlers::bottom::RefreshStack(*this);
            AttributeHandlers::positionOrigin::RefreshStack(*this);
            AttributeHandlers::innerOffsetX::RefreshStack(*this);
            AttributeHandlers::innerOffsetY::RefreshStack(*this);
            AttributeHandlers::fontFamily::RefreshStack(*this);
            AttributeHandlers::fontSize::RefreshStack(*this);
            AttributeHandlers::fontWeight::RefreshStack(*this);
            AttributeHandlers::fontSlant::RefreshStack(*this);
            AttributeHandlers::textColour::RefreshStack(*this);
            AttributeHandlers::textSelectionColour::RefreshStack(*this);
            AttributeHandlers::textSelectionBackgroundColour::RefreshStack(*this);
            AttributeHandlers::textSelectionBackgroundColourBlurred::RefreshStack(*this);
            AttributeHandlers::editableText::RefreshStack(*this);
            AttributeHandlers::singleLineText::RefreshStack(*this);
            AttributeHandlers::opacity::RefreshStack(*this);
            AttributeHandlers::compoundOpacity::RefreshStack(*this);
            AttributeHandlers::enableShadow::RefreshStack(*this);
            AttributeHandlers::shadowColour::RefreshStack(*this);
            AttributeHandlers::shadowBlurRadius::RefreshStack(*this);
            AttributeHandlers::shadowOffsetX::RefreshStack(*this);
            AttributeHandlers::shadowOffsetY::RefreshStack(*this);
            AttributeHandlers::shadowExtrusionX::RefreshStack(*this);
            AttributeHandlers::shadowExtrusionY::RefreshStack(*this);
            AttributeHandlers::shadowOpacity::RefreshStack(*this);
            AttributeHandlers::allowMouseDrag::RefreshStack(*this);
            AttributeHandlers::constrainMouseDragX::RefreshStack(*this);
            AttributeHandlers::constrainMouseDragY::RefreshStack(*this);
            AttributeHandlers::mouseDragClampModeX::RefreshStack(*this);
            AttributeHandlers::mouseDragClampModeY::RefreshStack(*this);
            AttributeHandlers::allowMouseResize::RefreshStack(*this);
            AttributeHandlers::leftGripperWidth::RefreshStack(*this);
            AttributeHandlers::rightGripperWidth::RefreshStack(*this);
            AttributeHandlers::topGripperWidth::RefreshStack(*this);
            AttributeHandlers::bottomGripperWidth::RefreshStack(*this);


            // Any applicable properties need to be prioritised.
            this->UpdatePositioningPriorities();
        }
    }



    void GUIStyleStack::UpdatePositioningPriorities()
    {
        // We need to traverse through the style classes and check whether left, right, top and bottom should have priority over each other.
        bool rightPrioritySet  = false;
        bool bottomPrioritySet = false;

        for (auto i = this->classes.root; i != nullptr; i = i->next)
        {
            auto value = i->value;
            if (value != nullptr)
            {
                // Only check left/right priority if it hasn't yet been found.
                if (!rightPrioritySet)
                {
                    if (value->right.isset || value->left.isset)
                    {
                        this->rightHasPriority = value->rightHasPriority;
                        rightPrioritySet = true;
                    }
                }

                // Only check top/bottom priority is it hasn't yet been found.
                if (!bottomPrioritySet)
                {
                    if (value->bottom.isset || value->top.isset)
                    {
                        this->bottomHasPriority = value->bottomHasPriority;
                        bottomPrioritySet = true;
                    }
                }
            }

            // We can terminate early...
            if (rightPrioritySet && bottomPrioritySet)
            {
                break;
            }
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
