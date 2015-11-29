// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/Server.hpp>
#include <GTEngine/GUI/StandardLibrary.hpp>
#include <GTEngine/Core/Strings/Tokenizer.hpp>
#include <GTEngine/Core/Strings/Create.hpp>
#include <GTEngine/Core/Strings/Equal.hpp>
#include <GTEngine/Core/ToString.hpp>
#include <cassert>
#include <GTEngine/GTEngine.hpp>
#include <easy_path/easy_path.h>

// Server
namespace GTGUI
{
    Server::Server(GT::Script* script, ImageManager* imageManagerIn)
        : operationMode(OperationMode_Delayed),
          eventHandler(&ServerEventHandler::Default), scripting(*this, script), styling(*this),
          markupLoader(*this),
          m_imageManager(imageManagerIn), glyphMapManager(*this),
          m_renderer(nullptr),
          elements(),
          absoluteElements(),
          elementsUsingZIndex(),
          layoutManager(),
          stepTimer(),
          eventQueue(), eventLock(),
          viewportWidth(0), viewportHeight(0),
          fontServer(glyphMapManager), fontCache(fontServer),
          hoveredElements(),
          elementTooltipProperties(), tooltipDelay(0.25f),
          pushedElement(nullptr), pushedElementMousePosX(0), pushedElementMousePosY(0),
          focusedElement(nullptr),
          draggingPushedElement(false), draggedElementClickPosX(0), draggedElementClickPosY(0),
          resizingPushedElement(false), overLeftSizingGripper(false), overRightSizingGripper(false), overTopSizingGripper(false), overBottomSizingGripper(false),
          resizingUsingLeftGripper(false), resizingUsingRightGripper(false), resizingUsingTopGripper(false), resizingUsingBottomGripper(false),
          gripperClickPosX(0), gripperClickPosY(0),
          garbageCollector(),
          caret(),
          isStepping(false),
          eventHandlingCounter(0),
          isMouseValid(false), isMouseCaptured(false),
          isLeftMouseButtonDown(false), isRightMouseButtonDown(false), isMiddleMouseButtonDown(false),
          mouseMoved(false),
          mousePosX(0), mousePosY(0),
          lastMouseClickPosX(0), lastMouseClickPosY(0),
          currentCursor(GT::Cursor_Arrow),
          loadedImages(), registeredImages(),
          errorLevel(1),
          isCTRLKeyDown(false), isShiftKeyDown(false), onTearEventPosted(false), isMouseSelectingText(false),
          dragAndDropProxyElement(nullptr), dragAndDropProxyElementOffset(0, 0),
          elementsNeedingOnShow(), elementsNeedingOnHide(),
          autoElementCounter(0)
    {
        // Load the defaults after setting the event handlers.
        this->LoadDefaults();

        // TODO: Consider whether or not this should be called manually by the application. Perhaps they won't want the standard library?
        // Here we load the standard library.
        this->LoadStandardLibrary();
    }

    Server::~Server()
    {
        // We will collect garbage early on.
        this->garbageCollector.Collect(true);


        for (size_t i = 0; i < this->elementsUsingZIndex.count; ++i)
        {
            delete this->elementsUsingZIndex.buffer[i]->value;
        }


        while (this->elements.GetRoot() != nullptr)
        {
            auto element = this->elements.GetRoot();
            this->elements.Remove(element);

            delete element;
        }


        // Images need to be unloaded.
        if (m_imageManager != nullptr)
        {
            for (size_t i = 0; i < this->loadedImages.count; ++i)
            {
                m_imageManager->DeleteImage(this->loadedImages.buffer[i]->value);
            }

            for (size_t i = 0; i < this->registeredImages.count; ++i)
            {
                m_imageManager->DeleteImage(this->registeredImages.buffer[i]->value);
            }
        }
    }

    bool Server::IsInitialised() const
    {
        return true;
    }


    void Server::SetOperationMode(int mode)
    {
        assert(mode == OperationMode_Delayed || mode == OperationMode_Immediate);

        this->operationMode = mode;
    }

    int Server::GetOperationMode() const
    {
        return this->operationMode;
    }


    void Server::SetEventHandler(ServerEventHandler &eventHandlerIn)
    {
        this->eventHandler = &eventHandlerIn;
    }

    ServerEventHandler & Server::GetEventHandler()
    {
        return *this->eventHandler;
    }

    ScriptServer & Server::GetScriptServer()
    {
        return this->scripting;
    }

    StyleServer & Server::GetStyleServer()
    {
        return this->styling;
    }

    bool Server::Load(const char* xml, const char* absDirectory, GT::String &topID)
    {
        return this->markupLoader.Load(xml, absDirectory, topID);
    }

    bool Server::Load(const char* xml, const char* absDirectory)
    {
        GT::String devnull;
        return this->Load(xml, absDirectory, devnull);
    }

    bool Server::Load(const char* xml, GT::String &topID)
    {
        return this->Load(xml, nullptr, topID);
    }


    bool Server::LoadFromFile(const char* filename)
    {
        return this->markupLoader.LoadFile(filename);
    }

    bool Server::IsFileLoaded(const char* filename)
    {
        // We need to check with the absolute path.
        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(GT::g_EngineContext->GetVFS(), filename, absolutePath, sizeof(absolutePath)))
        {
            return this->markupLoader.IsFileLoaded(absolutePath);
        }

        return false;
    }


    bool Server::ExecuteScript(const char* script, int returnValueCount)
    {
        return this->scripting.Execute(script, returnValueCount);
    }

    bool Server::ExecuteStyleScript(const char* script, const char* baseURLPath)
    {
        return this->styling.Load(script, baseURLPath);
    }


    void Server::SetImageManager(ImageManager* newImageManager)
    {
        // TODO: Need to unload all currently loaded images and reload with the new image manager.
        m_imageManager = newImageManager;
    }

    void Server::SetRenderer(Renderer* newRenderer)
    {
        m_renderer = newRenderer;
    }



    Element* Server::CreateElement(const char* xml, const char* absDirectory)
    {
        // We just load like normal, and then retrieve the element based on it's ID.
        GT::String topID;
        if (this->Load(xml, absDirectory, topID))
        {
            return this->GetElementByID(topID.c_str());
        }

        return nullptr;
    }

    Element* Server::CreateElement(const char* id, Element* parentElement)
    {
        // We're going to need an ID.
        char autoID[64];
        if (id == nullptr)
        {
            this->NextAnonymousID(autoID, 64);
            id = autoID;
        }


        auto &script = this->scripting.GetScript();
        {
            // local newelement = GTGUI.Element.Create('id');
            script.GetGlobal("GTGUI");
            assert(script.IsTable(-1));
            {
                script.Push("Element");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push("Create");
                    script.GetTableValue(-2);
                    assert(script.IsFunction(-1));
                    {
                        script.Push(id);
                        script.Call(1, 1);

                        script.InsertIntoStack(-3);
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);


            // The new element will be at the top of the stack (-1).

            // GTGUI.Server._Elems[element->parentid]:AppendChild(newelement);
            if (parentElement != nullptr)
            {
                script.GetGlobal("GTGUI");
                assert(script.IsTable(-1));
                {
                    script.Push("Server");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.Push("_Elems");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push(parentElement->id);
                            script.GetTableValue(-2);
                            assert(script.IsTable(-1));
                            {
                                script.Push("AppendChild");
                                script.GetTableValue(-2);
                                assert(script.IsFunction(-1));
                                {
                                    script.PushValue(-2);       // <-- 'self'
                                    script.PushValue(-7);       // <-- 'newelement'
                                    script.Call(2, 0);
                                }
                            }
                            script.Pop(1);
                        }
                        script.Pop(1);
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }


            // Need to pop the new element.
            script.Pop(1);
        }


        return this->GetElementByID(id);
    }

    Element* Server::CreateElement(Element* parentElement)
    {
        return this->CreateElement(nullptr, parentElement);
    }



    void Server::DeleteElement(Element* element, bool deleteChildren, bool invalidateParentLayout)
    {
        // When deleting an element, we don't delete the object straight away. Instead we mark for collection and perform the cleanup
        // during the next step. The reason we do this is because a rendering thread may still need the element.

        if (element != nullptr)
        {
            // If the element is the focused element, it needs to be blurred.
            if (this->focusedElement == element)
            {
                this->BlurFocusedElement();
            }

            // If the element is the pushed element, it needs to be released.
            if (this->pushedElement == element)
            {
                this->ReleasePushedElement(true);       // 'true' means to block posting events. We don't want to post events on an element that's getting deleted.
            }



            // If we're deleting children, we will recursively do them first.
            if (deleteChildren)
            {
                while (element->firstChild != nullptr)
                {
                    this->DeleteElement(element->firstChild, true, false);      // Second argument: delete children. Third argument: do not invalidate the parent's layout.
                }
            }

            // The element needs to be orphaned before deleting.
            if (element->parent != nullptr)
            {
                element->parent->RemoveChild(*element, invalidateParentLayout);
            }


            // We need to detach all event handlers here.
            element->DetachAllEventHandlers();

            // Any existing events need to be nullified.
            this->eventQueue.RemoveEventsOfElement(element);


            // If the element is hovered, it needs to be unhovered.
            this->hoveredElements.Remove(this->hoveredElements.Find(element));
            this->elementTooltipProperties.Remove(element);




            // If the element is the drag-and-drop proxy, it needs to be unset. We don't post an event.
            if (element == this->GetDragAndDropProxyElement())
            {
                this->RemoveCurrentDragAndDropProxyElement(false);
            }



            // The element needs to be removed from any lists...
            this->elements.Remove(element);
            this->RemoveFromZIndexList(*element);
            this->absoluteElements.Remove(this->absoluteElements.Find(element));
            this->elementsNeedingOnShow.RemoveFirstOccuranceOf(element);
            this->elementsNeedingOnHide.RemoveFirstOccuranceOf(element);

            // Any references to this element needs to be removed from the layout manager.
            this->layoutManager.RemoveElement(*element);

            // Here is where we are going to remove the element from the scripting environment.
            this->scripting.RemoveElement(*element);


            // If we're in the middle of stepping we will want to garbage collect. Otherwise we can just delete immediately. The reason for this is
            // that if an element is in the middle of it's event handler iterations, we'll need to delay deleting until the remaining events have
            // been handled.
            if (this->isStepping || this->IsHandlingEvent())
            {
                this->garbageCollector.MarkForCollection(*element);
            }
            else
            {
                delete element;
            }
        }
    }


    void Server::NextAnonymousID(char* destBuffer, size_t destBufferSizeInBytes)
    {
        // TODO: Handle this a little better. We're currently willy-nilly subtracting 3 from destBufferSizeInBytes. Might go below zero, in which
        //       case we'll up with with an overflow.

        // Unused at the moment.
        (void)destBufferSizeInBytes;

        destBuffer[0] = '_';
        destBuffer[1] = '_';
        destBuffer[2] = 'E';

    #if defined(_MSC_VER)
        _itoa_s(this->autoElementCounter++, destBuffer + 3, destBufferSizeInBytes - 3, 10);
    #else
        IO::snprintf(destBuffer + 3, destBufferSizeInBytes - 3, "%d", this->autoElementCounter++);
    #endif

        //IO::snprintf(destBuffer, destBufferSizeInBytes, "__E%d", this->autoElementCounter++);
    }



    void Server::SetViewportSize(unsigned int width, unsigned int height)
    {
        this->viewportWidth  = width;
        this->viewportHeight = height;

        auto root = this->GetRootElement();
        if (root != nullptr)
        {
            auto rootStyle = this->styling.GetRootElementStyleClass();
            if (rootStyle != nullptr)
            {
                rootStyle->SetAttribute("width",  GT::ToString(width).c_str());
                rootStyle->SetAttribute("height", GT::ToString(height).c_str());
            }
        }

        // Absolute elements will now have invalid positions.
        for (auto i = this->absoluteElements.root; i != nullptr; i = i->next)
        {
            auto element = i->value;
            assert(element != nullptr);

            if (element->style.rightHasPriority)
            {
                this->OnRightChanged(*element);
            }
            else
            {
                if (element->style.left->InPercent())
                {
                    this->OnLeftChanged(*element);
                }
            }

            if (element->style.bottomHasPriority)
            {
                this->OnBottomChanged(*element);
            }
            else
            {
                if (element->style.top->InPercent())
                {
                    this->OnTopChanged(*element);
                }
            }
        }
    }

    void Server::GetViewportSize(unsigned int &width, unsigned int &height) const
    {
        width  = this->viewportWidth;
        height = this->viewportHeight;
    }

    Element* Server::GetElementByID(const char *id, ptrdiff_t idSizeInBytes) const
    {
        if (id != nullptr)
        {
            return this->elements.FindByID(id, idSizeInBytes);
        }

        return nullptr;
    }


    void Server::Step()
    {
        this->Step(this->stepTimer.GetTimeSinceLastUpdate());
    }

    void Server::Step(double deltaInSeconds)
    {
        this->isStepping = true;
        {
            // We update the step time in case the host application is using automatic deltas.
            this->stepTimer.Update();

            // Here we are going to update the caret.
            this->caret.Update(deltaInSeconds);


            // TODO: Timers need to be updated.


            // Tooltips need to be checked.
            this->PostShowTooltipEvents(deltaInSeconds);


            // Garbage elements need to be deleted.
            this->CollectGarbage();


            // We want to handle any pending events at the beginning. This will allow for things like colour changes, size adjustments, etc.
            //this->HandleEvents();


            // Now that the events have been handled we can start updating the layout information. We do this twice. Once before validating
            // the mouse, and again after. The reason is, is that validating the mouse can change the layout of elements such as when they
            // are being dragged or resized with the mouse. In order to avoid a one-step lag, we simply do another layout update.
            this->layoutManager.Validate();


            // After events have been handled, we need to validate the mouse. We do this after because one of the events will be the
            // OnMouseMove event. This function itself may post events, but they will be handled during the next step.
            this->ValidateMouse();

            // After laying out, we want to post OnShow or OnHide events.
            this->PostVisibilityEvents();


            // ValidateMouse() may move elements due to things such as dragging. Thus, we'll quickly validate any changes from there.
            this->layoutManager.Validate();
        }
        this->isStepping = false;
    }


    void Server::Paint(int left, int top, int right, int bottom)
    {
        assert(left <= right);
        assert(top  <= bottom);

        this->Render();
    }

    void Server::Paint()
    {
        this->Paint(0, 0, this->viewportWidth, this->viewportHeight);
    }


    void Server::UpdateLayout()
    {
        this->layoutManager.Validate();
    }


    void Server::GetFontInfoFromElement(const Element &element, GT::FontInfo &fi) const
    {
        fi.family = element.style.fontFamily->value.c_str();
        //fi.family.ToLower_ASCII();        // <-- Important. If not specified, strings such as "arial" and "Arial" will be considered separate.


        // Weights.
        switch (element.style.fontWeight->value)
        {
        case FontWeight_Thin:
            {
                fi.styleFlags |= GT::FontStyle_Thin;
                break;
            }

        case FontWeight_ExtraLight:
            {
                fi.styleFlags |= GT::FontStyle_ExtraLight;
                break;
            }

        case FontWeight_Light:
            {
                fi.styleFlags |= GT::FontStyle_Light;
                break;
            }

        case FontWeight_Medium:
            {
                fi.styleFlags |= GT::FontStyle_Medium;
                break;
            }

        case FontWeight_SemiBold:
            {
                fi.styleFlags |= GT::FontStyle_SemiBold;
                break;
            }

        case FontWeight_Bold:
            {
                fi.styleFlags |= GT::FontStyle_Bold;
                break;
            }

        case FontWeight_ExtraBold:
            {
                fi.styleFlags |= GT::FontStyle_ExtraBold;
                break;
            }

        case FontWeight_Heavy:
            {
                fi.styleFlags |= GT::FontStyle_Heavy;
                break;
            }

        case FontWeight_Default:
        default: break;
        }


        // Slants.
        switch (element.style.fontSlant->value)
        {
        case FontSlant_Italic:
            {
                fi.styleFlags |= GT::FontStyle_Italic;
                break;
            }

        case FontSlant_Default:
        default: break;
        }


        // Size.
        if (element.style.fontSize->InPoints())
        {
            fi.sizeInPoints = element.style.fontSize->value;
            fi.sizeInPixels = 0.0f;
        }
        else if (element.style.fontSize->InPixels())
        {
            fi.sizeInPoints = 0.0f;
            fi.sizeInPixels = element.style.fontSize->value;
        }
        else if (element.style.fontSize->InPercent())
        {
            fi.sizeInPoints = 0.0f;
            fi.sizeInPixels = static_cast<float>(element.GetHeight()) * (element.style.fontSize->value / 100.0f);
        }
    }


    GT::Font* Server::AcquireFont(const GT::FontInfo &fi)
    {
        return this->fontCache.AcquireFont(fi);
    }

    void Server::UnacquireFont(const GT::Font* font)
    {
        this->fontCache.UnacquireFont(font);
    }


    ImageHandle Server::AcquireImage(const char* absURLOrID)
    {
        if (m_imageManager != nullptr)
        {
            if (easypath_is_absolute(absURLOrID))
            {
                // We will first check that the image isn't already loaded. If it is, we just return the existing image.
                auto iImage = this->loadedImages.Find(absURLOrID);
                if (iImage != nullptr)
                {
                    return iImage->value;
                }
                else
                {
                    // The image does not exist. We simply load it, add it to our loaded list and then return it.
                    auto newImage = m_imageManager->LoadImage(absURLOrID);
                    if (newImage != 0)
                    {
                        this->loadedImages.Add(absURLOrID, newImage);
                    }
                    else
                    {
                        this->PostWarning(GT::String::CreateFormatted("GUI: Error loading \"%s\".", absURLOrID).c_str());
                    }

                    return newImage;
                }
            }
            else
            {
                auto iImage = this->registeredImages.Find(absURLOrID);
                if (iImage != nullptr)
                {
                    return iImage->value;
                }

                return 0;
            }
        }

        return 0;
    }


    EventContext Server::BeginPostingEvents()
    {
        EventContext context;
        context.keyEventsTarget = this->focusedElement;

        return context;
    }

    void Server::EndPostingEvents(EventContext context)
    {
        (void)context;
    }


    void Server::OnMouseMove(EventContext eventContext, int x, int y)
    {
        (void)eventContext;

        Event e;
        e.code        = EventCode_OnMouseMove;
        e.element     = nullptr;
        e.mousemove.x = x;
        e.mousemove.y = y;

        this->HandleEvent_OnMouseMove(e);

#if 0
        // We intentionally dont have a receiver element here. Instead we post mouse-move events to elements when the mouse is validated.
        Event e;
        e.code        = EventCode_OnMouseMove;
        e.element     = nullptr;
        e.mousemove.x = x;
        e.mousemove.y = y;

        if (this->IsInImmediateMode())
        {
            this->HandleEvent_OnMouseMove(e);
        }
        else
        {
            this->QueueEvent(e);
        }
#endif
    }

    void Server::OnMouseWheel(EventContext eventContext, int delta, int x, int y)
    {
        (void)eventContext;

        Event e;
        e.code             = EventCode_OnMouseWheel;
        e.element          = nullptr;
        e.mousewheel.delta = delta;
        e.mousewheel.x     = x;
        e.mousewheel.y     = y;

        this->HandleEvent_OnMouseWheel(e);

#if 0
        if (this->IsInImmediateMode())
        {
            this->HandleEvent_OnMouseWheel(e);
        }
        else
        {
            this->QueueEvent(e);
        }
#endif
    }


    void Server::OnMouseButtonDown(EventContext eventContext, int button)
    {
        (void)eventContext;

        Event e;
        e.code             = EventCode_OnMouseButtonDown;
        e.element          = nullptr;
        e.mousedown.button = static_cast<GT::MouseButton>(button);
        e.mousedown.x      = this->mousePosX;
        e.mousedown.y      = this->mousePosY;

        this->HandleEvent_OnMouseButtonDown(e);

#if 0
        if (this->IsInImmediateMode())
        {
            this->HandleEvent_OnMouseButtonDown(e);
        }
        else
        {
            this->QueueEvent(e);
        }
#endif
    }

    void Server::OnMouseButtonUp(EventContext eventContext, int button)
    {
        (void)eventContext;

        Event e;
        e.code           = EventCode_OnMouseButtonUp;
        e.element        = nullptr;
        e.mouseup.button = static_cast<GT::MouseButton>(button);
        e.mouseup.x      = this->mousePosX;
        e.mouseup.y      = this->mousePosY;

        this->HandleEvent_OnMouseButtonUp(e);

#if 0
        if (this->IsInImmediateMode())
        {
            this->HandleEvent_OnMouseButtonUp(e);
        }
        else
        {
            this->QueueEvent(e);
        }
#endif
    }

    void Server::OnMouseButtonDoubleClick(EventContext eventContext, int button)
    {
        (void)eventContext;

        Event e;
        e.code             = EventCode_OnMouseButtonDoubleClick;
        e.element          = nullptr;
        e.mousedown.button = static_cast<GT::MouseButton>(button);
        e.mousedown.x      = this->mousePosX;
        e.mousedown.y      = this->mousePosY;

        this->HandleEvent_OnMouseButtonDoubleClick(e);

#if 0
        if (this->IsInImmediateMode())
        {
            this->HandleEvent_OnMouseButtonDoubleClick(e);
        }
        else
        {
            this->QueueEvent(e);
        }
#endif
    }


    void Server::OnKeyPressed(EventContext eventContext, GT::Key key)
    {
        Event e;
        e.code        = EventCode_OnKeyPressed;
        e.element     = eventContext.keyEventsTarget;
        e.keydown.key = key;

        this->HandleEvent_OnKeyPressed(e);

#if 0
        if (this->IsInImmediateMode())
        {
            this->HandleEvent_OnKeyPressed(e);
        }
        else
        {
            this->QueueEvent(e);
        }
#endif
    }

    void Server::OnKeyDown(EventContext eventContext, GT::Key key)
    {
        Event e;
        e.code        = EventCode_OnKeyDown;
        e.element     = eventContext.keyEventsTarget;
        e.keydown.key = key;

        this->HandleEvent_OnKeyDown(e);

#if 0
        if (this->IsInImmediateMode())
        {
            this->HandleEvent_OnKeyDown(e);
        }
        else
        {
            this->QueueEvent(e);
        }
#endif
    }

    void Server::OnKeyUp(EventContext eventContext, GT::Key key)
    {
        Event e;
        e.code      = EventCode_OnKeyUp;
        e.element   = eventContext.keyEventsTarget;
        e.keyup.key = key;

        this->HandleEvent_OnKeyUp(e);

#if 0
        if (this->IsInImmediateMode())
        {
            this->HandleEvent_OnKeyUp(e);
        }
        else
        {
            this->QueueEvent(e);
        }
#endif
    }

    void Server::OnSize(Element *element)
    {
        // We don't neccessarilly want to use QueueEvent() here. If we're currently inside HandleEvents(), we just want to call the event handler
        // directly.
        Event e;
        e.code        = EventCode_OnSize;
        e.element     = element;
        e.size.width  = element->GetWidth();
        e.size.height = element->GetHeight();

        this->HandleEvent_OnSize(e);

#if 0
        if (!this->handlingEvents && this->IsInDelayedMode())
        {
            this->QueueEvent(e);
        }
        else
        {
            this->HandleEvent_OnSize(e);
        }
#endif
    }

    void Server::OnMove(Element *element)
    {
        // We don't neccessarilly want to use QueueEvent() here. If we're currently inside HandleEvents(), we just want to call the event handler
        // directly.
        Event e;
        e.code    = EventCode_OnMove;
        e.element = element;
        e.move.x  = element->x;
        e.move.y  = element->y;

        this->HandleEvent_OnMove(e);

#if 0
        if (!this->handlingEvents && this->IsInDelayedMode())
        {
            this->QueueEvent(e);
        }
        else
        {
            this->HandleEvent_OnMove(e);
        }
#endif
    }


    void Server::OnShow(Element* element)
    {
        this->BeginEventHandling();
        {
            element->OnShow();
            this->scripting.PostEvent_OnShow(*element);
        }
        this->EndEventHandling();
    }

    void Server::OnHide(Element* element)
    {
        this->BeginEventHandling();
        {
            element->OnHide();
            this->scripting.PostEvent_OnHide(*element);
        }
        this->EndEventHandling();
    }



    void Server::QueueEvent(Event &e)
    {
        this->eventLock.Lock();
        {
            this->eventQueue.Push(e);
        }
        this->eventLock.Unlock();
    }

    void Server::FocusElement(Element *element)
    {
        this->BlurFocusedElement();

        if (element != nullptr)
        {
            this->focusedElement = element;
            this->focusedElement->style.ActivateModifierClasses(StyleClassType_Focused);


            // If the element has editable text, we need to show the text cursor. The height of the cursor will be the height of a line with it's current font.
            if (this->focusedElement->style.editableText->value && element->GetFont() != nullptr)
            {
                this->caret.Show(this->focusedElement);
                this->caret.SetSize(1, element->GetFont()->GetLineHeight());

                // If we position the text cursor here, the focused element will scroll before the LMB events are posted. This will cause the
                // element to move the cursor to the incorrect spot. Don't position the text cursor here.
                //this->PositionCaret(false, true);

                // We need to invalidate the text rendering data so the GUI can show the focused selection colour.
                this->focusedElement->InvalidateTextRenderingData();
            }
            else
            {
                this->caret.Hide();
            }


            // We need to let the script server know about this and post the OnFocus() event.
            this->scripting.PostEvent_OnFocus(*element);
            element->OnFocus();
        }
    }

    void Server::BlurFocusedElement()
    {
        if (this->focusedElement != nullptr)
        {
            // Alway hide the caret when focus is lost. It will be reshown in FocusElement().
            this->caret.Hide();

            // We deactivate the styles before posting events.
            this->focusedElement->style.DeactivateModifierClasses(StyleClassType_Focused);

            // Now we post events.
            this->scripting.PostEvent_OnBlur(*this->focusedElement);
            this->focusedElement->OnBlur();


            // We need to invalidate the text rendering data so the GUI can show the blurred selection colour.
            this->focusedElement->InvalidateTextRenderingData();


            // We set the focused element only after we've posted events.
            this->focusedElement = nullptr;
        }
    }

    Element* Server::GetFocusedElement()
    {
        return this->focusedElement;
    }

    bool Server::CanElementReceiveFocusFromMouse(const Element &element) const
    {
        return element.IsEnabled() && (element.style.canReceiveFocusFromMouse->value || element.style.editableText->value);
    }


    void Server::BringToTop(Element &element)
    {
        // All we do is change the z-index to 1 higher than the highest that's currently showing.
        if (this->elementsUsingZIndex.count > 0)    // <-- should actually be able to assert this.
        {
            auto highZIndex = this->elementsUsingZIndex.buffer[this->elementsUsingZIndex.count - 1]->key;

            element.SetStyleAttribute("z-index", GT::ToString(highZIndex + 1).c_str());
        }
    }

    void Server::PushToBack(Element &element)
    {
        // All we do is change the z-index to 1 lower than the highest that's currently showing.
        if (this->elementsUsingZIndex.count > 0)    // <-- should actually be able to assert this.
        {
            auto highZIndex = this->elementsUsingZIndex.buffer[0]->key;

            element.SetStyleAttribute("z-index", GT::ToString(highZIndex - 1).c_str());
        }
    }


    void Server::PositionCaret(int clickPosX, int clickPosY, bool extendSelection, bool autoScroll)
    {
        (void)clickPosY;        // <-- this will be used when we support multi-line text.

        // The position of the caret depends on the text, the font and the alignment. The click position is relative to
        // the caret's owner, so we'll get the text rect relative to the element, too.
        auto owner = this->caret.GetOwner();
        if (owner != nullptr)
        {
            int leftPadding = owner->GetLeftPadding();
            int topPadding  = owner->GetTopPadding();

            // We position the caret using the element's text manager. The text manager requires that the input coordinates be relative
            // to the top left of the container area. The container area is the size of the element, minus the padding.
            owner->textManager.MoveCursor(clickPosX - leftPadding, clickPosY - topPadding);

            int caretPosX;
            int caretPosY;
            owner->textManager.GetCursorPosition(caretPosX, caretPosY);

            this->caret.SetPosition(caretPosX + leftPadding, caretPosY + topPadding);


            // The caret should always be visible.
            if (autoScroll)
            {
                this->ScrollTextCursorIntoVisibility();
            }

            this->HandleTextSelection(extendSelection);
        }
    }

    void Server::PositionCaret(bool extendSelection, bool autoScroll)
    {
        auto owner = this->caret.GetOwner();
        if (owner != nullptr)
        {
            int leftPadding = owner->GetLeftPadding();
            int topPadding  = owner->GetTopPadding();

            int caretPosX;
            int caretPosY;
            owner->textManager.GetCursorPosition(caretPosX, caretPosY);

            this->caret.SetPosition(caretPosX + leftPadding, caretPosY + topPadding);


            /// The text cursor should always be visible.
            if (autoScroll)
            {
                this->ScrollTextCursorIntoVisibility();
            }


            this->HandleTextSelection(extendSelection);
        }
    }


    void Server::ScrollTextCursorIntoVisibility()
    {
        auto owner = this->caret.GetOwner();
        if (owner != nullptr)
        {
            int caretPosX;
            int caretPosY;
            this->caret.GetPosition(caretPosX, caretPosY);

            int currentXOffset = owner->GetInnerXOffset();
            int currentYOffset = owner->GetInnerYOffset();
            int newXOffset     = currentXOffset;
            int newYOffset     = currentYOffset;
            int textWidth      = static_cast<int>(owner->textManager.GetTextWidth());



            // How do we do this for center alignment? Ignore that case for now.
            if (owner->style.horizontalAlign->value != Align_Center)
            {
                if (caretPosX > owner->GetInnerRightEdge())
                {
                    if (owner->style.horizontalAlign->value == Align_Left)
                    {
                        newXOffset = currentXOffset - (caretPosX - owner->GetInnerRightEdge());
                        if (!owner->style.singleLineText->value)
                        {
                            newXOffset -= 32;
                        }
                    }
                    else if (owner->style.horizontalAlign->value == Align_Right)
                    {
                        newXOffset = GT::Max(0, currentXOffset - (caretPosX - owner->GetInnerRightEdge()) - 16);
                    }
                }
                else if (caretPosX < owner->GetInnerLeftEdge())
                {
                    if (owner->style.horizontalAlign->value == Align_Left)
                    {
                        newXOffset = GT::Min(0, currentXOffset - caretPosX + 16 + owner->GetInnerLeftEdge());
                    }
                    else if (owner->style.horizontalAlign->value == Align_Right)
                    {
                        newXOffset = currentXOffset - caretPosX + owner->GetInnerLeftEdge();
                    }
                }
                else
                {
                    if (owner->style.singleLineText->value)
                    {
                        if (owner->style.horizontalAlign->value == Align_Left)
                        {
                            int textRightEdge = owner->GetInnerLeftEdge() + textWidth + currentXOffset;

                            if (currentXOffset < 0 && textRightEdge < owner->GetInnerRightEdge())
                            {
                                newXOffset = GT::Min(0, owner->GetInnerWidth() - textWidth);
                            }
                        }
                        else if (owner->style.horizontalAlign->value == Align_Right)
                        {
                            int textLeftEdge = owner->GetInnerRightEdge() - owner->textManager.GetTextWidth() + currentXOffset;

                            if (currentXOffset > 0 && textLeftEdge > owner->GetInnerLeftEdge())
                            {
                                newXOffset = textWidth - owner->GetInnerWidth();
                            }
                        }
                    }
                }
            }

            // Vertical. As with horizontal, we're going to ignore center alignment for now.
            if (owner->style.verticalAlign->value != Align_Center)
            {
                int cursorPosX;
                int cursorPosY;
                owner->textManager.GetCursorPosition(cursorPosX, cursorPosY);

                int lineHeight    = static_cast<int>(owner->textManager.GetCursorLine()->GetHeight());
                int pageLineCount = static_cast<int>(owner->textManager.GetPageLineCount());

                if (caretPosY + static_cast<int>(lineHeight) > owner->GetInnerBottomEdge())
                {
                    int lineIndex = static_cast<int>(owner->textManager.GetCursorLineIndex());

                    if (owner->style.verticalAlign->value == Align_Top)
                    {
                        newYOffset = (pageLineCount - lineIndex - 1) * lineHeight;
                    }
                    else if (owner->style.verticalAlign->value == Align_Bottom)
                    {
                    }
                }
                else if (caretPosY < owner->GetInnerTopEdge())
                {
                    int lineIndex = static_cast<int>(owner->textManager.GetCursorLineIndex());

                    if (owner->style.verticalAlign->value == Align_Top)
                    {
                        newYOffset = -lineIndex * lineHeight;
                    }
                    else if (owner->style.verticalAlign->value == Align_Bottom)
                    {
                    }
                }
            }



            // Now we need to do the actual scrolling.
            if (newXOffset != currentXOffset || newYOffset != currentYOffset)
            {
                if (newXOffset != currentXOffset)
                {
                    owner->SetStyleAttribute("inner-offset-x", GT::ToString(newXOffset).c_str());
                }

                if (newYOffset != currentYOffset)
                {
                    owner->SetStyleAttribute("inner-offset-y", GT::ToString(newYOffset).c_str());
                }


                // We need to mark the text as changed so that layout and rendering information can be updated.
                this->OnTextChanged(*owner, true);

                // Now we need to reposition the caret so that it's also visible, making sure we pass 'false' as the argument which will
                // ensure it doesn't try to do another auto-scroll, which could cause a recursion issue.
                this->PositionCaret(true, false);
            }
        }
    }

    void Server::HandleTextSelection(bool extendSelection)
    {
        auto owner = this->caret.GetOwner();
        if (owner != nullptr)
        {
            if (extendSelection)
            {
                owner->textManager.MoveSelectionEndToCursor();
                owner->InvalidateTextRenderingData();
            }
            else
            {
                bool wasAnythingSelected = owner->textManager.IsAnythingSelected();

                owner->textManager.DeselectAll();

                if (wasAnythingSelected)
                {
                    owner->InvalidateTextRenderingData();
                }
            }
        }
    }


    Element * Server::GetElementUnderPoint(int x, int y, bool fromMouseInput, bool ignoreDragAndDropProxy)
    {
        Element* result = nullptr;

        // Elements with a higher z-index have priority...
        for (size_t i = this->elementsUsingZIndex.count; i > 0; --i)
        {
            auto list = this->elementsUsingZIndex.buffer[i - 1]->value;
            assert(list != nullptr);

            // What we do here is run through the z-indexed elements and find the element that the mouse is over, if any. We
            // actually need to run through this list backwards so that it's consistent with rendering.
            for (auto iElement = list->last; iElement != nullptr; iElement = iElement->prev)
            {
                assert(iElement->value != nullptr);

                if (ignoreDragAndDropProxy)
                {
                    if (iElement->value == this->dragAndDropProxyElement)
                    {
                        continue;
                    }
                }

                // Now we need to iteratively run through the chain of elements and find the last element that contains the point.
                // This function expects the rectangle of the parent, so we need to grab that, too.
                GT::Rect<int> rect;
                if (iElement->value->parent != nullptr)
                {
                    iElement->value->parent->GetAbsoluteRect(rect);
                }

                this->GetElementUnderPoint(x, y, fromMouseInput, *iElement->value, rect, rect, result);

                if (result != nullptr)
                {
                    break;
                }
            }

            // If we found an element under the pointer, we need to break.
            if (result != nullptr)
            {
                break;
            }
        }

        return result;
    }

/*
    const GT::Font & Server::GetDefaultFont() const
    {
        assert(this->defaultFont != nullptr);

        return *this->defaultFont;
    }
*/


    void Server::OnZIndexChanged(Element &element)
    {
        // We need to find the list that the element is currently sitting in. It will need to be removed from here before adding it
        // to the new list.
        auto oldList = this->FindListContainingElement(element);
        if (oldList != nullptr)
        {
            oldList->Remove(oldList->Find(&element));
        }

        // Now we add the element to the z-index list.
        this->AddToZIndexList(element);


        this->InvalidateMouse();
    }

    void Server::OnWidthChanged(Element &element)
    {
        this->layoutManager.InvalidateWidth(element);

        this->InvalidateMouse();
    }

    void Server::OnHeightChanged(Element &element)
    {
        this->layoutManager.InvalidateHeight(element);

        this->InvalidateMouse();
    }

    void Server::OnPaddingXChanged(Element &element)
    {
        this->layoutManager.InvalidateWidth(element);

        this->InvalidateMouse();
    }
    void Server::OnPaddingYChanged(Element &element)
    {
        this->layoutManager.InvalidateHeight(element);

        this->InvalidateMouse();
    }

    void Server::OnMarginsXChanged(Element &element)
    {
        this->layoutManager.InvalidateWidth(element);

        this->InvalidateMouse();
    }
    void Server::OnMarginsYChanged(Element &element)
    {
        this->layoutManager.InvalidateHeight(element);

        this->InvalidateMouse();
    }

    void Server::OnVisibilityChanged(Element &element)
    {
        // If the element has been made invisible, and it was also the focused element, it needs to lose focus.
        if (!element.IsVisible() && this->focusedElement == &element)
        {
            this->BlurFocusedElement();
        }

        // We update everything here because it may be that the element has gone from a visible to an invisible state, in which case it
        // will not have previously been updated properly.

        // Siblings will have also changed. This is where <element> will be invalidated.
        if (element.parent != nullptr)
        {
            for (auto iSibling = element.parent->firstChild; iSibling != nullptr; iSibling = iSibling->nextSibling)
            {
                this->layoutManager.InvalidateWidth(*iSibling);
                this->layoutManager.InvalidateHeight(*iSibling);
                this->layoutManager.InvalidatePosition(*iSibling);
            }

            if (element.parent->style.width->Automatic())
            {
                this->layoutManager.InvalidateWidth(*element.parent);
            }

            if (element.parent->style.height->Automatic())
            {
                this->layoutManager.InvalidateHeight(*element.parent);
            }
        }

        // The visibility of children will have also changed. These all need to be updated, recursively. We do not want to invalidate the size and position of these ones, though.
        for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
        {
            this->OnVisibilityChanged(*iChild);
        }


        // Events will need to be posted, but after validating layouts.
        if (element.IsVisible())
        {
            this->MarkElementAsNeedingOnShow(element);
        }
        else
        {
            this->MarkElementAsNeedingOnHide(element);
        }


        // With a change in visibility, there may also be a change in the hovered elements. We need to update.
        this->InvalidateMouse();
    }

    void Server::OnChildPlaneChanged(Element &element)
    {
        // We update all 'auto' children here.
        auto firstChild = element.GetFirstVisibleAutoChild();
        if (firstChild != nullptr)
        {
            this->layoutManager.InvalidatePosition(*firstChild);
        }

        this->InvalidateMouse();
    }

    void Server::OnHorizontalAlignChanged(Element &element)
    {
        // Update all 'auto' children.
        auto firstChild = element.GetFirstVisibleAutoChild();
        if (firstChild != nullptr)
        {
            this->layoutManager.InvalidatePosition(*firstChild);
        }

        this->InvalidateMouse();
    }

    void Server::OnVerticalAlignChanged(Element &element)
    {
        // Update all 'auto' children.
        auto firstChild = element.GetFirstVisibleAutoChild();
        if (firstChild != nullptr)
        {
            this->layoutManager.InvalidatePosition(*firstChild);
        }

        this->InvalidateMouse();
    }

    void Server::OnPositioningChanged(Element &element)
    {
        // When the positioning mode of an element has changed, it's possible that it has gone from 'auto' to 'relative' or 'absolute'. In this case,
        // we need to ensure it's auto-positioned siblings have their positions updated.
        if (element.style.positioning->value != Positioning_Auto)
        {
            if (element.parent != nullptr)
            {
                auto firstChild = element.parent->GetFirstVisibleAutoChild();
                if (firstChild != nullptr)
                {
                    this->layoutManager.InvalidatePosition(*firstChild);
                }
            }
        }

        this->layoutManager.InvalidatePosition(element);


        this->InvalidateMouse();

        // We need to ensure the element is in the z-index list (or not).
        if (element.style.positioning->value == Positioning_Auto)
        {
            this->RemoveFromZIndexList(element);
        }
        else
        {
            this->AddToZIndexList(element);
        }


        // We need to keep track of absolute elements so that when the main viewport is resized, we can handle absolute's correctly.
        if (element.style.positioning->value == Positioning_Absolute)
        {
            // With everything factored correctly, it should be impossible for <element> to be in this->absoluteElements. Thus, we're going
            // to use an assert here instead of a run-time check.
            assert(this->absoluteElements.Exists(&element) == false);
            this->absoluteElements.Append(&element);
        }
        else
        {
            // Here we ensure the element is no longer in our list of absolute elements.
            this->absoluteElements.Remove(this->absoluteElements.Find(&element));
        }
    }

    void Server::OnLeftChanged(Element &element)
    {
        this->layoutManager.InvalidatePosition(element);

        this->InvalidateMouse();
    }

    void Server::OnRightChanged(Element &element)
    {
        this->layoutManager.InvalidatePosition(element);

        this->InvalidateMouse();
    }

    void Server::OnTopChanged(Element &element)
    {
        this->layoutManager.InvalidatePosition(element);

        this->InvalidateMouse();
    }

    void Server::OnBottomChanged(Element &element)
    {
        this->layoutManager.InvalidatePosition(element);

        this->InvalidateMouse();
    }

    void Server::OnInnerOffsetXChanged(Element &element)
    {
        // The positions of all 'auto' children have just changed.
        for (auto child = element.firstChild; child != nullptr; child = child->nextSibling)
        {
            this->layoutManager.InvalidatePosition(*child);
        }

        this->layoutManager.InvalidateText(element);


        // We want to let the scripting environment know about a change to the inner offset. This is mostly important for scrollbars.
        this->scripting.PostEvent_OnInnerXOffsetChanged(element);

        this->InvalidateMouse();
    }

    void Server::OnInnerOffsetYChanged(Element &element)
    {
        // The positions of all 'auto' children have just changed.
        for (auto child = element.firstChild; child != nullptr; child = child->nextSibling)
        {
            this->layoutManager.InvalidatePosition(*child);
        }

        this->layoutManager.InvalidateText(element);


        // We want to let the scripting environment know about a change to the inner offset. This is mostly important for scrollbars.
        this->scripting.PostEvent_OnInnerYOffsetChanged(element);

        this->InvalidateMouse();
    }

    void Server::OnTextChanged(Element &element, bool blockEvent)
    {
        element.UpdateTextManagerLayout();

        this->layoutManager.InvalidateText(element);


        // We want to let the scripting environment know about the change.
        if (!blockEvent)
        {
            this->scripting.PostEvent_OnTextChanged(element);
        }
    }


    void Server::OnChildAttached(Element &element)
    {
        for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
        {
            this->layoutManager.InvalidateWidth(*iChild);
            this->layoutManager.InvalidateHeight(*iChild);
            this->layoutManager.InvalidatePosition(*iChild);
        }

        if (element.style.width->Automatic())
        {
            this->layoutManager.InvalidateWidth(element);
        }

        if (element.style.height->Automatic())
        {
            this->layoutManager.InvalidateHeight(element);
        }


        // We need to keep track of absolute elements so that when the main viewport is resized, we can handle absolute's correctly.
        if (element.style.positioning->value == Positioning_Absolute)
        {
            if (!this->absoluteElements.Exists(&element))
            {
                this->absoluteElements.Append(&element);
            }
        }
        else
        {
            // Here we ensure the element is no longer in our list of absolute elements.
            this->absoluteElements.Remove(this->absoluteElements.Find(&element));
        }

        this->InvalidateMouse();
    }

    void Server::OnChildDetached(Element &element)
    {
        for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
        {
            this->layoutManager.InvalidateWidth(*iChild);
            this->layoutManager.InvalidateHeight(*iChild);
            this->layoutManager.InvalidatePosition(*iChild);
        }

        if (element.style.width->Automatic())
        {
            this->layoutManager.InvalidateWidth(element);
        }

        if (element.style.height->Automatic())
        {
            this->layoutManager.InvalidateHeight(element);
        }

        this->InvalidateMouse();
    }

    void Server::OnFlexWidthChanged(Element &element)
    {
        // Only affects children with % widths.
        for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
        {
            if (iChild->style.width->InPercent())
            {
                this->layoutManager.InvalidateWidth(*iChild);
            }
        }

        this->InvalidateMouse();
    }

    void Server::OnFlexHeightChanged(Element &element)
    {
        // Only affects children with % height.
        for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
        {
            if (iChild->style.height->InPercent())
            {
                this->layoutManager.InvalidateHeight(*iChild);
            }
        }

        this->InvalidateMouse();
    }


#if 0
    void Server::HandleEvents()
    {
        this->eventLock.Lock();
        this->handlingEvents = true;

        Event e;
        while (this->eventQueue.Next(e))
        {
            switch (e.code)
            {
            case EventCode_OnSize:                   this->HandleEvent_OnSize(e);                   break;
            case EventCode_OnMove:                   this->HandleEvent_OnMove(e);                   break;
            case EventCode_OnKeyPressed:             this->HandleEvent_OnKeyPressed(e);             break;
            case EventCode_OnKeyDown:                this->HandleEvent_OnKeyDown(e);                break;
            case EventCode_OnKeyUp:                  this->HandleEvent_OnKeyUp(e);                  break;
            case EventCode_OnMouseMove:              this->HandleEvent_OnMouseMove(e);              break;
            case EventCode_OnMouseWheel:             this->HandleEvent_OnMouseWheel(e);             break;
            case EventCode_OnMouseButtonDown:        this->HandleEvent_OnMouseButtonDown(e);        break;
            case EventCode_OnMouseButtonUp:          this->HandleEvent_OnMouseButtonUp(e);          break;
            case EventCode_OnMouseButtonDoubleClick: this->HandleEvent_OnMouseButtonDoubleClick(e); break;

            case EventCode_Unknown:
            default: break;
            }
        }

        this->handlingEvents = false;
        this->eventLock.Unlock();
    }
#endif

    void Server::HandleEvent_OnSize(Event &e)
    {
        assert(e.element != nullptr);

        e.element->UpdateTextManagerLayout();

        this->BeginEventHandling();
        {
            e.element->OnSize();
            this->scripting.PostEvent_OnSize(*e.element);
        }
        this->EndEventHandling();
    }

    void Server::HandleEvent_OnMove(Event &e)
    {
        assert(e.element != nullptr);

        this->BeginEventHandling();
        {
            e.element->OnMove();
            this->scripting.PostEvent_OnMove(*e.element);
        }
        this->EndEventHandling();
    }

    void Server::HandleEvent_OnMouseMove(Event &e)
    {
        this->BeginEventHandling();
        {
            this->mousePosX = e.mousemove.x;
            this->mousePosY = e.mousemove.y;

            this->mouseMoved = true;

            this->InvalidateMouse();
        }
        this->EndEventHandling();
    }

    void Server::HandleEvent_OnMouseWheel(Event &e)
    {
        this->BeginEventHandling();
        {
            auto receiver = this->GetRootElement();
            if (this->hoveredElements.root != nullptr && this->hoveredElements.root->value != nullptr && this->hoveredElements.root->value->IsEnabled())
            {
                receiver = this->hoveredElements.root->value;
            }


            int relativeX = e.mousewheel.x;
            int relativeY = e.mousewheel.y;
            receiver->AbsoluteToRelative(relativeX, relativeY);

            receiver->OnMouseWheel(e.mousewheel.delta, relativeX, relativeY);
            this->scripting.PostEvent_OnMouseWheel(*receiver, e.mousewheel.delta, relativeX, relativeY);
        }
        this->EndEventHandling();
    }


    void Server::HandleEvent_OnMouseButtonDown(Event &e)
    {
        this->BeginEventHandling();
        {
            // Left mouse buttons are treated differently to other mouse buttons because they need to handle things like pushing and dragging. Thus,
            // we'll do it in a separate branch.
            if (e.mousedown.button == GT::MouseButton_Left)
            {
                this->HandleEvent_OnLMBDown(e);
            }
            else
            {
                auto receiver = this->GetRootElement();
                if (this->hoveredElements.root != nullptr && this->hoveredElements.root->value != nullptr && this->hoveredElements.root->value->IsEnabled())
                {
                    receiver = this->hoveredElements.root->value;
                }

                receiver->OnMouseButtonDown(e.mousedown.button, e.mousedown.x, e.mousedown.y);
                this->scripting.PostEvent_OnMouseButtonDown(*receiver, e.mousedown.button);
            }
        }
        this->EndEventHandling();
    }

    void Server::HandleEvent_OnMouseButtonUp(Event &e)
    {
        this->BeginEventHandling();
        {
            // Just like OnMouseButtonDown, the left mouse button needs to be handled differently.
            if (e.mousedown.button == GT::MouseButton_Left)
            {
                this->HandleEvent_OnLMBUp(e);
            }
            else
            {
                auto receiver = this->GetRootElement();
                if (this->hoveredElements.root != nullptr && this->hoveredElements.root->value != nullptr && this->hoveredElements.root->value->IsEnabled())
                {
                    receiver = this->hoveredElements.root->value;
                }

                receiver->OnMouseButtonUp(e.mouseup.button, e.mouseup.x, e.mouseup.y);
                this->scripting.PostEvent_OnMouseButtonUp(*receiver, e.mouseup.button);
            }
        }
        this->EndEventHandling();
    }

    void Server::HandleEvent_OnMouseButtonDoubleClick(Event &e)
    {
        this->BeginEventHandling();
        {
            auto receiver = this->GetRootElement();
            if (this->hoveredElements.root != nullptr && this->hoveredElements.root->value != nullptr && this->hoveredElements.root->value->IsEnabled())
            {
                receiver = this->hoveredElements.root->value;
            }

            receiver->OnMouseButtonDoubleClick(e.mousedoubleclick.button, e.mousedoubleclick.x, e.mousedoubleclick.y);
            this->scripting.PostEvent_OnMouseButtonDoubleClick(*receiver, e.mousedoubleclick.button);
        }
        this->EndEventHandling();
    }


    void Server::HandleEvent_OnLMBDown(Event &e)
    {
        // NOTE: Don't need a BeginEventHandling()/EndEventHandling() pair here. Done at a higher level.

        this->isLeftMouseButtonDown = true;

        this->lastMouseClickPosX = e.mousedown.x;
        this->lastMouseClickPosY = e.mousedown.y;


        auto receiver = this->GetRootElement();
        if (this->hoveredElements.root != nullptr && this->hoveredElements.root->value != nullptr && this->hoveredElements.root->value->IsEnabled())
        {
            receiver = this->hoveredElements.root->value;
        }


        // If a control has focus, it needs to lose focus, but only if the receiver will get focus later.
        bool giveReceiverFocus = this->CanElementReceiveFocusFromMouse(receiver);
        if (giveReceiverFocus)
        {
            this->BlurFocusedElement();
        }


        receiver->OnMouseButtonDown(e.mousedown.button, e.mousedown.x, e.mousedown.y);
        this->scripting.PostEvent_OnMouseButtonDown(*receiver, e.mousedown.button);


        // The receiver will become the pushed element.
        this->pushedElement = receiver;

        // The pushed element needs to have messages posted.
        this->scripting.PostEvent_OnPush(*this->pushedElement);
        this->pushedElement->OnPush();


        // We'll focus the element after pusing mouse-down and push events.
        if (giveReceiverFocus)
        {
            this->FocusElement(receiver);


            // A few things need to be done if the receiver is a text box.
            if (receiver->style.editableText->value)
            {
                this->isMouseSelectingText = true;


                // If the focused element has anything selected, it needs to be deselected now.
                if (!this->isShiftKeyDown)
                {
                    receiver->textManager.DeselectAll();
                    receiver->InvalidateTextRenderingData();
                }

                int clickPosX = this->lastMouseClickPosX;
                int clickPosY = this->lastMouseClickPosY;
                receiver->AbsoluteToRelative(clickPosX, clickPosY);


                this->PositionCaret(clickPosX, clickPosY, this->isShiftKeyDown, false);
            }
        }



        // Now we'll check if the element is draggable.
        if (receiver->style.allowMouseDrag->value && receiver->style.positioning->value != Positioning_Auto)
        {
            this->draggingPushedElement = true;

            this->draggedElementClickPosX = e.mousedown.x;
            this->draggedElementClickPosY = e.mousedown.y;
            this->pushedElement->AbsoluteToRelative(this->draggedElementClickPosX, this->draggedElementClickPosY);
        }
        else
        {
            this->draggingPushedElement = false;
        }

        // Here we'll check if the element wants to be resized. For now we'll only do fixed sized elements. Will add % sized resizing later.
        if (((this->overLeftSizingGripper || this->overRightSizingGripper)  && receiver->style.width->InPixels()) ||
            ((this->overTopSizingGripper  || this->overBottomSizingGripper) && receiver->style.height->InPixels()))
        {
            this->resizingPushedElement = true;
            this->resizingUsingLeftGripper   = this->overLeftSizingGripper;
            this->resizingUsingRightGripper  = this->overRightSizingGripper;
            this->resizingUsingTopGripper    = this->overTopSizingGripper;
            this->resizingUsingBottomGripper = this->overBottomSizingGripper;

            this->gripperClickPosX = e.mousedown.x;
            this->gripperClickPosY = e.mousedown.y;
            this->pushedElement->AbsoluteToRelative(this->gripperClickPosX, this->gripperClickPosY);

            if (this->resizingUsingRightGripper)
            {
                this->gripperClickPosX -= (this->pushedElement->width - static_cast<int>(this->pushedElement->style.rightGripperWidth->value));
            }

            if (this->resizingUsingBottomGripper)
            {
                this->gripperClickPosY -= (this->pushedElement->height - static_cast<int>(this->pushedElement->style.bottomGripperWidth->value));
            }

            // When we're resizing, we're not dragging.
            this->draggingPushedElement = false;
        }
        else
        {
            this->resizingPushedElement = false;
        }


        this->pushedElementMousePosX = this->mousePosX;
        this->pushedElementMousePosY = this->mousePosY;
        this->pushedElement->AbsoluteToRelative(this->pushedElementMousePosX, this->pushedElementMousePosY);
    }

    void Server::HandleEvent_OnLMBUp(Event &e)
    {
        // NOTE: Don't need a BeginEventHandling()/EndEventHandling() pair here. Done at a higher level.

        this->isLeftMouseButtonDown = false;


        auto receiver = this->GetRootElement();
        if (this->hoveredElements.root != nullptr && this->hoveredElements.root->value != nullptr && this->hoveredElements.root->value->IsEnabled())
        {
            receiver = this->hoveredElements.root->value;
        }


        // If we're dragging an element, we need to post an OnDrop() event for each dragged element.
        if (this->dragAndDropProxyElement != nullptr)
        {
            auto elementUnderDragAndDropProxy = this->GetElementUnderPoint(this->mousePosX, this->mousePosY, true, true);
            if (elementUnderDragAndDropProxy != nullptr && elementUnderDragAndDropProxy->IsEnabled())
            {
                elementUnderDragAndDropProxy->OnDrop(*this->dragAndDropProxyElement);
                this->scripting.PostEvent_OnDrop(*elementUnderDragAndDropProxy, *this->dragAndDropProxyElement);
            }

            // With the drag-and-drop proxy released, we need to remove it.
            this->RemoveCurrentDragAndDropProxyElement();
        }



        receiver->OnMouseButtonUp(e.mouseup.button, e.mouseup.x, e.mouseup.y);
        this->scripting.PostEvent_OnMouseButtonUp(*receiver, e.mouseup.button);


        // We need to release the pushed element.
        this->ReleasePushedElement();

        // We are no longer using the mouse to select text.
        this->isMouseSelectingText = false;
    }


    void Server::HandleEvent_OnKeyPressed(Event &e)
    {
        this->BeginEventHandling();
        {
            // There is an interesting case here, where OnKeyPressed can possibly change the focused element. We don't want to be modifying the text of the newly
            // focused element if we have changed it mid-way through event handling. What we're going to do, is keep track of the focused element before calling
            // the events. If the focused element has changed, we're going to check if the next event is an OnKeyDown with the same character, and if so, invalidate
            // that event so it isn't posted.
            if (e.element != nullptr && e.element->IsEnabled())
            {
                e.element->OnKeyPressed(e.keypressed.key);
                this->scripting.PostEvent_OnKeyPressed(*e.element, e.keypressed.key);
            }
            else
            {
                // The scripting environment must always be aware of the event regardless of whether or not an element has focus. In this case, we just post
                // it to the root element.
                this->scripting.PostEvent_OnKeyPressed(*this->GetRootElement(), e.keypressed.key);
            }


            // If the focused element has changed, we need to invalidate the proceeding OnKeyDown event, if it's there.
            if (e.element != this->focusedElement)
            {
                auto nextEvent = this->eventQueue.PeekNext();
                if (nextEvent != nullptr)
                {
                    if (nextEvent->code == EventCode_OnKeyDown)
                    {
                        nextEvent->code = EventCode_Null;
                    }
                }
            }
        }
        this->EndEventHandling();
    }

    void Server::HandleEvent_OnKeyDown(Event &e)
    {
        this->BeginEventHandling();
        {
            if (e.keydown.key == GT::Keys::Ctrl)
            {
                this->isCTRLKeyDown = true;
            }
            else if (e.keydown.key == GT::Keys::Shift)
            {
                this->isShiftKeyDown = true;
            }

            // If we have a focused element, we need to let it know about our character.
            if (e.element != nullptr && e.element->style.editableText->value && e.element->IsEnabled())
            {
                if (this->isCTRLKeyDown && (e.keydown.key == GT::Keys::X || e.keydown.key == GT::Keys::x))
                {
                    e.element->textManager.CopySelectionToClipboard();
                    e.element->textManager.DeleteSelectedCharacters();
                }
                else if (this->isCTRLKeyDown && (e.keydown.key == GT::Keys::C || e.keydown.key == GT::Keys::c))
                {
                    e.element->textManager.CopySelectionToClipboard();
                }
                else if (this->isCTRLKeyDown && (e.keydown.key == GT::Keys::V || e.keydown.key == GT::Keys::v))
                {
                    e.element->textManager.DeleteSelectedCharacters(false);
                    e.element->textManager.InsertClipboardTextAtCursor();
                }
                else
                {
                    // Here we only want to modify the text if the event handlers have returned true.
                    //
                    // There is another interesting case here... If an OnKeyDown event handler changes the focused element, it will cause that newly focused element
                    // to receive the key input, which is definately not what we want.
                    if (e.element->OnKeyDown(e.keydown.key) &&
                        this->scripting.PostEvent_OnKeyDown(*e.element, e.keydown.key))
                    {
                        if (e.element == this->focusedElement)                  // <-- This makes sure we only modify the text of the focused element if it wasn't changed by the event handlers.
                        {
                            bool positionCaret   = true;
                            bool extendSelection = false;

                            if (this->isCTRLKeyDown && (e.keydown.key == GT::Keys::A || e.keydown.key == GT::Keys::a))
                            {
                                e.element->SelectAllText();
                                extendSelection = true;
                            }
                            else if (this->isCTRLKeyDown && (e.keydown.key == GT::Keys::Z || e.keydown.key == GT::Keys::z))
                            {
                                e.element->textManager.Undo();
                            }
                            else if (this->isCTRLKeyDown && (e.keydown.key == GT::Keys::Y || e.keydown.key == GT::Keys::y))
                            {
                                e.element->textManager.Redo();
                            }
                            else if (e.keydown.key == GT::Keys::Backspace)
                            {
                                if (e.element->textManager.IsAnythingSelected())
                                {
                                    e.element->textManager.DeleteSelectedCharacters();
                                }
                                else
                                {
                                    e.element->textManager.DeleteCharacterAtLeftOfCursor();
                                }
                            }
                            else if (e.keydown.key == GT::Keys::Delete)
                            {
                                if (e.element->textManager.IsAnythingSelected())
                                {
                                    e.element->textManager.DeleteSelectedCharacters();
                                }
                                else
                                {
                                    e.element->textManager.DeleteCharacterAtRightOfCursor();
                                }
                            }
                            else if (e.keydown.key == GT::Keys::ArrowLeft)
                            {
                                if (e.element->textManager.IsAnythingSelected() && !this->isShiftKeyDown)
                                {
                                    e.element->textManager.MoveCursorToFirstSelectionMarker();
                                    e.element->textManager.DeselectAll();

                                    e.element->InvalidateTextRenderingData();
                                }
                                else
                                {
                                    e.element->textManager.MoveCursorLeft();
                                }

                                if (this->isShiftKeyDown)
                                {
                                    extendSelection = true;
                                }
                            }
                            else if (e.keydown.key == GT::Keys::ArrowRight)
                            {
                                if (e.element->textManager.IsAnythingSelected() && !this->isShiftKeyDown)
                                {
                                    e.element->textManager.MoveCursorToLastSelectionMarker();
                                    e.element->textManager.DeselectAll();

                                    e.element->InvalidateTextRenderingData();
                                }
                                else
                                {
                                    e.element->textManager.MoveCursorRight();
                                }

                                if (this->isShiftKeyDown)
                                {
                                    extendSelection = true;
                                }
                            }
                            else if (e.keydown.key == GT::Keys::ArrowUp)
                            {
                                e.element->textManager.MoveCursorUp();

                                if (!this->isShiftKeyDown)
                                {
                                    e.element->textManager.DeselectAll();
                                    e.element->InvalidateTextRenderingData();
                                }
                                else
                                {
                                    extendSelection = true;
                                }
                            }
                            else if (e.keydown.key == GT::Keys::ArrowDown)
                            {
                                e.element->textManager.MoveCursorDown();

                                if (!this->isShiftKeyDown)
                                {
                                    e.element->textManager.DeselectAll();
                                    e.element->InvalidateTextRenderingData();
                                }
                                else
                                {
                                    extendSelection = true;
                                }
                            }
                            else if (e.keydown.key == GT::Keys::Home)
                            {
                                e.element->textManager.MoveCursorToStartOfLine();

                                if (!this->isShiftKeyDown)
                                {
                                    e.element->textManager.DeselectAll();
                                    e.element->InvalidateTextRenderingData();
                                }
                                else
                                {
                                    extendSelection = true;
                                }
                            }
                            else if (e.keydown.key == GT::Keys::End)
                            {
                                e.element->textManager.MoveCursorToEndOfLine();

                                if (!this->isShiftKeyDown)
                                {
                                    e.element->textManager.DeselectAll();
                                    e.element->InvalidateTextRenderingData();
                                }
                                else
                                {
                                    extendSelection = true;
                                }
                            }
                            else if ((e.keydown.key == GT::Keys::Enter || e.keydown.key == GT::Keys::Return) && !e.element->style.singleLineText->value)
                            {
                                e.element->textManager.DeleteSelectedCharacters(false);
                                e.element->textManager.InsertNewLineAtCursor();
                            }
                            else if (e.keydown.key == GT::Keys::Tab)
                            {
                                e.element->textManager.InsertTabAtCursor(true);
                            }
                            else if (GT::IsKeyPrintable(e.keydown.key) && !this->isCTRLKeyDown)
                            {
                                if (e.element->textManager.IsAnythingSelected())
                                {
                                    e.element->textManager.DeleteSelectedCharacters(false);
                                }

                                e.element->textManager.InsertCharacterAtCursor(e.keydown.key);
                            }
                            else
                            {
                                positionCaret = false;
                            }



                            if (positionCaret &&
                                e.keydown.key != GT::Keys::Shift &&
                                e.keydown.key != GT::Keys::Ctrl  &&
                                e.keydown.key != GT::Keys::Alt)
                            {
                                this->PositionCaret(extendSelection, true);
                            }
                        }
                    }
                }
            }
            else
            {
                if (e.element != nullptr && this->focusedElement->IsEnabled())
                {
                    e.element->OnKeyDown(e.keydown.key);
                    this->scripting.PostEvent_OnKeyDown(*e.element, e.keydown.key);
                }
                else
                {
                    // The scripting environment must always be aware of the event regardless of whether or not an element has focus. In this case, we just post
                    // it to the root element.
                    this->scripting.PostEvent_OnKeyDown(*this->GetRootElement(), e.keydown.key);
                }
            }
        }
        this->EndEventHandling();
    }

    void Server::HandleEvent_OnKeyUp(Event &e)
    {
        this->BeginEventHandling();
        {
            if (e.keyup.key == GT::Keys::Ctrl)
            {
                this->isCTRLKeyDown = false;
            }
            else if (e.keyup.key == GT::Keys::Shift)
            {
                this->isShiftKeyDown = false;
            }

            if (e.element != nullptr && e.element->IsEnabled())
            {
                e.element->OnKeyUp(e.keyup.key);
                this->scripting.PostEvent_OnKeyUp(*e.element, e.keyup.key);
            }
            else
            {
                // The scripting environment must always be aware of the event regardless of whether or not an element has focus. In this case, we just post
                // it to the root element.
                this->scripting.PostEvent_OnKeyUp(*this->GetRootElement(), e.keyup.key);
            }
        }
    }


    void Server::BeginEventHandling()
    {
        this->eventHandlingCounter += 1;
    }

    void Server::EndEventHandling()
    {
        assert(this->eventHandlingCounter > 0);
        {
            this->eventHandlingCounter -= 1;
        }
    }

    bool Server::IsHandlingEvent() const
    {
        return this->eventHandlingCounter > 0;
    }



    void Server::Render()
    {
        // Don't do anything if we don't have a renderer attached.
        if (m_renderer != nullptr)
        {
            // First thing is to let the renderer know that we're starting.
            m_renderer->Begin(*this);


            // Elements with a higher z-index are rendered last. They need to be shown on top of everything below it.
            for (size_t i = 0; i < this->elementsUsingZIndex.count; ++i)
            {
                auto list = this->elementsUsingZIndex.buffer[i]->value;
                assert(list != nullptr);
                {
                    for (auto iElement = list->root; iElement != nullptr; iElement = iElement->next)
                    {
                        auto element = iElement->value;
                        assert(element != nullptr);
                        {
                            // We need to recursively run through the chain of elements and render them in order. This won't render
                            // children that use a z-index since they will be drawn separately.
                            if (element->IsVisible())
                            {
                                m_renderer->RenderElement(*this, *element);
                            }
                        }
                    }
                }
            }


            // Finally, the renderer needs to know that we're finished.
            m_renderer->End();
        }
    }



    bool Server::LoadDefaults()
    {
        bool success = true;

        success = success && this->Load("<div id='_Root' />");

        return success;
    }

    bool Server::LoadStandardLibrary()
    {
        return StandardLibrary::Load(*this);
    }


    Element* Server::CreateNewElement(const char* id)
    {
        auto element = new Element(id, *this);
        this->elements.Insert(element);

        // Default to needing an OnShow event posted.
        this->MarkElementAsNeedingOnShow(*element);

        return element;
    }


    void Server::CollectGarbage()
    {
        // All we need to do is call our garbage collector's Collect() method...
        this->garbageCollector.Collect();
    }


    void Server::InvalidateMouse()
    {
        this->isMouseValid = false;
    }


    void Server::RegisterImage(const char* id, unsigned int width, unsigned int height, unsigned int bpp, const void* data)
    {
        if (m_imageManager != nullptr)
        {
            // Delete existing image.
            auto iImage = this->registeredImages.Find(id);
            if (iImage != nullptr)
            {
                m_imageManager->DeleteImage(iImage->value);
                this->registeredImages.Remove(id);
            }

            this->registeredImages.Add(id, m_imageManager->CreateImage(width, height, (bpp == 3) ? GTGUI::ImageFormat_RGB8 : GTGUI::ImageFormat_RGBA8, data));
        }
    }

    ImageHandle Server::GetRegisteredImage(const char* id)
    {
        auto iImage = this->registeredImages.Find(id);
        if (iImage != nullptr)
        {
            return iImage->value;
        }

        return 0;
    }


    void Server::PostError(const char* message)
    {
        if (this->eventHandler != nullptr)
        {
            this->eventHandler->OnError(message);
        }
    }

    void Server::PostWarning(const char* message)
    {
        if (this->eventHandler != nullptr)
        {
            this->eventHandler->OnWarning(message);
        }
    }

    void Server::PostLogMessage(const char* message)
    {
        if (this->eventHandler != nullptr)
        {
            this->eventHandler->OnLog(message);
        }
    }


    void Server::SetDragAndDropProxyElement(Element &newDragAndDropProxyElement)
    {
        if (this->dragAndDropProxyElement != nullptr)
        {
            this->RemoveCurrentDragAndDropProxyElement();
        }

        this->dragAndDropProxyElement = &newDragAndDropProxyElement;
    }

    Element* Server::GetDragAndDropProxyElement()
    {
        return this->dragAndDropProxyElement;
    }

    void Server::RemoveCurrentDragAndDropProxyElement(bool postRemoveEvent)
    {
        if (this->dragAndDropProxyElement != nullptr)
        {
            if (postRemoveEvent)
            {
                this->dragAndDropProxyElement->OnDragAndDropProxyRemoved();
                this->scripting.PostEvent_OnDragAndDropProxyRemoved(*this->dragAndDropProxyElement);
            }

            this->dragAndDropProxyElement       = nullptr;
            this->dragAndDropProxyElementOffset = GT::Point<int>(0, 0);
        }
    }

    void Server::SetDragAndDropProxyElementOffset(int offsetX, int offsetY)
    {
        this->dragAndDropProxyElementOffset = GT::Point<int>(offsetX, offsetY);
    }


    void Server::GetMousePosition(int &xOut, int &yOut) const
    {
        xOut = this->mousePosX;
        yOut = this->mousePosY;
    }

    void Server::SetMousePosition(int x, int y)
    {
        this->mousePosX = x;
        this->mousePosY = y;
    }


    void Server::MarkMouseAsCaptured()
    {
        this->isMouseCaptured = true;
        this->InvalidateMouse();
    }

    void Server::UnmarkMouseAsCaptured()
    {
        this->isMouseCaptured = false;
        this->InvalidateMouse();
    }

    bool Server::IsMouseCaptured() const
    {
        return this->isMouseCaptured;
    }


    bool Server::IsLMBDown() const
    {
        return this->isLeftMouseButtonDown;
    }

    bool Server::IsRMBDown() const
    {
        return this->isRightMouseButtonDown;
    }

    bool Server::IsMMBDown() const
    {
        return this->isMiddleMouseButtonDown;
    }



    void Server::ValidateMouse()
    {
        // We don't need to do anything if the mouse is already valid.
        if (!this->isMouseValid)
        {
            // The first step is to the top-most element that is under the mouse.
            auto hoveredElement = this->GetElementUnderPoint(this->mousePosX, this->mousePosY, true, true);

            // We pass all mouse events over to the root element if the mouse is captured.
            if (this->IsMouseCaptured())
            {
                hoveredElement = this->GetRootElement();
            }
            else
            {
                // If we have a pushed element, we don't want to highlight stuff.
                if (this->pushedElement != nullptr && this->dragAndDropProxyElement == nullptr)
                {
                    hoveredElement = this->pushedElement;
                }
            }

            // The new cursor to use. It will be set at the end.
            GT::Cursor newCursor = this->currentCursor;


            // This list will contain pointers to the elements that have the mouse over them. The first item in this list will be hoveredElement if it was
            // non-null. The next will be ancestors of hoveredElement.
            GT::List<Element*>  newHoveredElements;
            GT::List<Element*> &oldHoveredElements = this->hoveredElements;

            if (hoveredElement != nullptr)
            {
                // First element needs to be 'hoveredElement'.
                newHoveredElements.Append(hoveredElement);

                // Ancestores need to receive mouse events, too.
                hoveredElement->GetAncestors(newHoveredElements);

                // The cursor needs to change. We post an event to the event handler since GTGUI doesn't know about the window system.
                if (!this->resizingPushedElement)
                {
                    if (hoveredElement->IsEnabled())
                    {
                        newCursor = GT::StringToCursor(hoveredElement->style.cursor->value.c_str());
                    }
                    else
                    {
                        newCursor = GT::Cursor_Arrow;
                    }
                }
            }

            // 'newHoveredElements' now contains all of the elements that the mouse is currently hovering over. Any element here that is not in
            // this->hoveredElements will receive MouseEnter messages. Converseley, any element in this->hoveredElements that is not in 'newHoveredElements'
            // will receive MouseLeave messages.
            for (auto iElement = newHoveredElements.root; iElement != nullptr; iElement = iElement->next)
            {
                auto element = iElement->value;
                assert(element != nullptr);
                {
                    bool wasPreviouslyHovered = oldHoveredElements.Find(element) != nullptr;
                    if (!wasPreviouslyHovered)
                    {
                        if (element->IsEnabled())
                        {
                            this->scripting.PostEvent_OnMouseEnter(*element);
                            element->OnMouseEnter();
                        }

                        this->elementTooltipProperties.Add(element, ElementTooltipProperties());
                    }
                }
            }

            for (auto iElement = oldHoveredElements.root; iElement != nullptr; iElement = iElement->next)
            {
                auto element = iElement->value;
                assert(element != nullptr);
                {
                    bool isCurrentlyHovered = newHoveredElements.Find(element) != nullptr;
                    if (!isCurrentlyHovered)
                    {
                        if (element->IsEnabled())
                        {
                            this->scripting.PostEvent_OnMouseLeave(*element);
                            element->OnMouseLeave();
                        }

                        // We may need to post a tooltip hide event.
                        auto iTooltipProperties = this->elementTooltipProperties.Find(element);
                        assert(iTooltipProperties != nullptr);
                        {
                            auto &tooltipProperties = iTooltipProperties->value;
                            if (tooltipProperties.hasShowEventBeenPosted)
                            {
                                element->OnHideTooltip();
                                this->scripting.PostEvent_OnHideTooltip(*element);
                            }

                            this->elementTooltipProperties.Remove(element);
                        }
                    }
                }
            }



            // We want to now post the OnDragAndDropEnter/Leave events. We don't post these if the receiver is disabled. We only post these to
            // top-level elements.
            if (this->dragAndDropProxyElement != nullptr)
            {
                if (hoveredElement != nullptr)
                {
                    if (hoveredElement->IsEnabled())
                    {
                        this->scripting.PostEvent_OnDragAndDropEnter(*hoveredElement, *this->dragAndDropProxyElement);
                        hoveredElement->OnDragAndDropEnter(*this->dragAndDropProxyElement);
                    }

                    if (oldHoveredElements.root != nullptr && oldHoveredElements.root->value != nullptr && oldHoveredElements.root->value != hoveredElement)
                    {
                        if (oldHoveredElements.root->value->IsEnabled())
                        {
                            this->scripting.PostEvent_OnDragAndDropLeave(*hoveredElement, *this->dragAndDropProxyElement);
                            oldHoveredElements.root->value->OnDragAndDropLeave(*this->dragAndDropProxyElement);
                        }
                    }
                }
            }




            // Now the internal list of elements need to be set to our new one. The most efficient way to do this is to clear the old list and
            // move each item from newHoveredElements into this->hoveredElements.
            this->hoveredElements.Clear();

            while (newHoveredElements.root != nullptr)
            {
                auto temp = newHoveredElements.root;

                newHoveredElements.Detach(temp);
                this->hoveredElements.AttachToEnd(temp);
            }







            // If we have a pushed element, but the hovered element is not equal to it, it means that the mouse has moved off the pushed element. In this
            // situation we change the elements style from the pushed state, but we don't want to fire any release events. Release events will be posted
            // when the mouse have been released through OnLMBUp().
            if (this->pushedElement != nullptr)
            {
                if (this->pushedElement != hoveredElement)
                {
                    this->pushedElement->style.DeactivateModifierClasses(StyleClassType_Pushed);
                }

                this->pushedElementMousePosX = this->mousePosX;
                this->pushedElementMousePosY = this->mousePosY;
                this->pushedElement->AbsoluteToRelative(this->pushedElementMousePosX, this->pushedElementMousePosY);
            }


            // If the mouse was moved, we may need to do some events and dragging.
            if (this->mouseMoved)
            {
                // If we have a hovered element, we need to post events.
                if (hoveredElement != nullptr)
                {
                    int relativePosX = this->mousePosX;
                    int relativePosY = this->mousePosY;
                    hoveredElement->AbsoluteToRelative(relativePosX, relativePosY);

                    hoveredElement->OnMouseMove(relativePosX, relativePosY);
                    this->scripting.PostEvent_OnMouseMove(*hoveredElement, relativePosX, relativePosY);
                }
            }


            if (this->pushedElement == nullptr)
            {
                this->overLeftSizingGripper   = false;
                this->overRightSizingGripper  = false;
                this->overTopSizingGripper    = false;
                this->overBottomSizingGripper = false;

                // Here we're going to check if we're hovered over a sizing gripper. If we are, we're going to change the cursor.
                if (hoveredElement != nullptr && hoveredElement->IsEnabled() && hoveredElement->parent != nullptr && hoveredElement->style.allowMouseResize->value)
                {
                    int relativePosX = this->mousePosX;
                    int relativePosY = this->mousePosY;
                    hoveredElement->AbsoluteToRelative(relativePosX, relativePosY);

                    // Now we're going to see which grippers the mouse is over.
                    if ((hoveredElement->parent->style.horizontalAlign->value == Align_Left || hoveredElement->style.positioning->value != Positioning_Auto) &&
                        (relativePosX >= hoveredElement->width - hoveredElement->style.rightGripperWidth->value))
                    {
                        this->overRightSizingGripper = true;
                    }
                    else if ((hoveredElement->parent->style.horizontalAlign->value == Align_Right || hoveredElement->style.positioning->value != Positioning_Auto) &&
                             (relativePosX <= hoveredElement->style.leftGripperWidth->value))
                    {
                        this->overLeftSizingGripper = true;
                    }

                    if ((hoveredElement->parent->style.verticalAlign->value == Align_Top || hoveredElement->style.positioning->value != Positioning_Auto) &&
                        (relativePosY >= hoveredElement->height - hoveredElement->style.bottomGripperWidth->value))
                    {
                        this->overBottomSizingGripper = true;
                    }

                    else if ((hoveredElement->parent->style.verticalAlign->value == Align_Bottom || hoveredElement->style.positioning->value != Positioning_Auto) &&
                             (relativePosY <= hoveredElement->style.topGripperWidth->value))
                    {
                        this->overTopSizingGripper = true;
                    }


                    if (overRightSizingGripper)
                    {
                        if (overBottomSizingGripper)
                        {
                            newCursor = GT::Cursor_SizeArrowBottomRight;
                        }
                        else if (overTopSizingGripper)
                        {
                            newCursor = GT::Cursor_SizeArrowTopRight;
                        }
                        else
                        {
                            newCursor = GT::Cursor_SizeArrowRight;
                        }
                    }
                    else if (overLeftSizingGripper)
                    {
                        if (overBottomSizingGripper)
                        {
                            newCursor = GT::Cursor_SizeArrowBottomLeft;
                        }
                        else if (overTopSizingGripper)
                        {
                            newCursor = GT::Cursor_SizeArrowTopLeft;
                        }
                        else
                        {
                            newCursor = GT::Cursor_SizeArrowLeft;
                        }
                    }
                    else
                    {
                        // If we get here it means the cursor is over the top or bottom grippers, but never left or right.
                        if (overBottomSizingGripper)
                        {
                            newCursor = GT::Cursor_SizeArrowBottom;
                        }
                        else if (overTopSizingGripper)
                        {
                            newCursor = GT::Cursor_SizeArrowTop;
                        }
                    }
                }
            }


            // If we're dragging, lets do it.
            if (this->pushedElement != nullptr)
            {
                if (this->draggingPushedElement)
                {
                    int relativeMousePosX = this->mousePosX;
                    int relativeMousePosY = this->mousePosY;
                    this->pushedElement->AbsoluteToRelative(relativeMousePosX, relativeMousePosY);

                    int mouseOffsetX = (!this->pushedElement->style.constrainMouseDragX->value) ? relativeMousePosX - this->draggedElementClickPosX : 0;
                    int mouseOffsetY = (!this->pushedElement->style.constrainMouseDragY->value) ? relativeMousePosY - this->draggedElementClickPosY : 0;

                    int newLeftPos = this->pushedElement->x + mouseOffsetX;
                    int newTopPos  = this->pushedElement->y + mouseOffsetY;

                    // We may need to do some clamping here.
                    if (this->pushedElement->style.positioning->value == Positioning_Relative && this->pushedElement->parent != nullptr)
                    {
                        newLeftPos -= this->pushedElement->parent->GetLeftPadding();
                        newTopPos  -= this->pushedElement->parent->GetTopPadding();

                        if (this->pushedElement->style.mouseDragClampModeX->value == "clickpos")
                        {
                            newLeftPos = GT::Clamp(newLeftPos, -this->draggedElementClickPosX, this->pushedElement->parent->GetInnerWidth() - this->draggedElementClickPosX);
                        }
                        else if (this->pushedElement->style.mouseDragClampModeX->value == "border")
                        {
                            newLeftPos = GT::Clamp(newLeftPos, 0, this->pushedElement->parent->GetInnerWidth() - this->pushedElement->width);
                        }

                        if (this->pushedElement->style.mouseDragClampModeY->value == "clickpos")
                        {
                            newTopPos = GT::Clamp(newTopPos, -this->draggedElementClickPosY, this->pushedElement->parent->GetInnerHeight() - this->draggedElementClickPosY);
                        }
                        else if (this->pushedElement->style.mouseDragClampModeY->value == "border")
                        {
                            newTopPos = GT::Clamp(newTopPos, 0, this->pushedElement->parent->GetInnerHeight() - this->pushedElement->height);
                        }
                    }
                    else
                    {
                        if (this->pushedElement->style.mouseDragClampModeX->value == "clickpos")
                        {
                            newLeftPos = GT::Clamp(newLeftPos, -this->draggedElementClickPosX, static_cast<int>(this->viewportWidth) - this->draggedElementClickPosX);
                        }
                        else if (this->pushedElement->style.mouseDragClampModeX->value == "border")
                        {
                            newLeftPos = GT::Clamp(newLeftPos, 0, static_cast<int>(this->viewportWidth) - this->pushedElement->width);
                        }

                        if (this->pushedElement->style.mouseDragClampModeY->value == "clickpos")
                        {
                            newTopPos = GT::Clamp(newTopPos, -this->draggedElementClickPosY, static_cast<int>(this->viewportHeight) - this->draggedElementClickPosY);
                        }
                        else if (this->pushedElement->style.mouseDragClampModeY->value == "border")
                        {
                            newTopPos = GT::Clamp(newTopPos, 0, static_cast<int>(this->viewportHeight) - this->pushedElement->height);
                        }
                    }

                    if (newLeftPos != this->pushedElement->x || newTopPos != this->pushedElement->y)
                    {
                        this->pushedElement->SetPosition(newLeftPos, newTopPos);
                    }
                }
                else
                {
                    // We need to check for whether or not an OnTear() event should be called. The OnTear() event is called for drag-and-drop operations. We don't do
                    // a tear operation unless the mouse has been dragged far enough.
                    if (!this->onTearEventPosted)
                    {
                        if (GT::Abs(this->lastMouseClickPosX - this->mousePosX) >= 4 ||
                            GT::Abs(this->lastMouseClickPosY - this->mousePosY) >= 4)
                        {
                            this->onTearEventPosted = true;

                            this->pushedElement->OnTear();
                            this->scripting.PostEvent_OnTear(*hoveredElement);
                        }
                    }
                }
            }



            // If we're resizing, we need to do that too.
            if (this->pushedElement != nullptr && this->resizingPushedElement)
            {
                int gripperMousePosX = this->mousePosX;
                int gripperMousePosY = this->mousePosY;
                this->pushedElement->AbsoluteToRelative(gripperMousePosX, gripperMousePosY);

                auto sc = this->pushedElement->GetPrimaryStyleClass();

                if (this->resizingUsingRightGripper)
                {
                    // Lets make everything relative to the gripper.
                    gripperMousePosX -= (this->pushedElement->width - static_cast<int>(this->pushedElement->style.rightGripperWidth->value));

                    int mouseOffsetX = gripperMousePosX - this->gripperClickPosX;
                    int newWidth     = this->pushedElement->width + mouseOffsetX;

                    sc->SetAttribute("width", GT::ToString(newWidth).c_str());
                }
                else if (this->resizingUsingLeftGripper)
                {
                    int mouseOffsetX = gripperMousePosX - this->gripperClickPosX;
                    int newWidth     = this->pushedElement->width - mouseOffsetX;

                    sc->SetAttribute("width", GT::ToString(newWidth).c_str());
                }

                if (this->resizingUsingBottomGripper)
                {
                    gripperMousePosY -= (this->pushedElement->height - static_cast<int>(this->pushedElement->style.bottomGripperWidth->value));

                    int mouseOffsetY = gripperMousePosY - this->gripperClickPosY;
                    int newHeight    = this->pushedElement->height + mouseOffsetY;

                    sc->SetAttribute("height", GT::ToString(newHeight).c_str());
                }
                else if (this->resizingUsingTopGripper)
                {
                    int mouseOffsetY = gripperMousePosY - this->gripperClickPosY;
                    int newHeight    = this->pushedElement->height - mouseOffsetY;

                    sc->SetAttribute("height", GT::ToString(newHeight).c_str());
                }
            }


            // The drag-and-drop proxy element needs to have it's position updated.
            if (this->dragAndDropProxyElement != nullptr)
            {
                this->dragAndDropProxyElement->SetPosition(this->mousePosX + this->dragAndDropProxyElementOffset.x, this->mousePosY + this->dragAndDropProxyElementOffset.y);
            }



            // If we are currently using the mouse to select text, we need to move the text cursor on the focused element. If we're doing
            // this, we want to keep the cursor regardless of the element that it's hovered over.
            if (this->focusedElement != nullptr && this->isMouseSelectingText)
            {
                // We need to position the caret. The input coordinates need to be relative to the element.
                int clickPosX = this->mousePosX;
                int clickPosY = this->mousePosY;
                this->focusedElement->AbsoluteToRelative(clickPosX, clickPosY);

                this->PositionCaret(clickPosX, clickPosY);
            }
            else
            {
                // If only want to change the cursor if we're not selecting text.
                this->SetCursor(newCursor);
            }





            // The mouse is now valid...
            this->isMouseValid = true;

            // From here on, we can make ourselves think that the mouse hasn't moved...
            this->mouseMoved = false;
        }
    }


    void Server::AddToZIndexList(Element &element)
    {
        // Only if the element actually uses the z-index do we need to store it.
        if (element.UsesZIndex())
        {
            int  zIndex      = element.GetZIndex();
            auto elementList = this->GetElementListByZIndex(zIndex);

            // If we don't have a list, we need to create it and add it to our map.
            if (elementList == nullptr)
            {
                elementList = new GT::List<Element*>;
                this->elementsUsingZIndex.Add(zIndex, elementList);
            }


            // Here is where we append the item. Note how we remove the item if it already exists - we only want one instance of the element
            // in the list.
            auto i = elementList->Find(&element);
            if (i != nullptr)
            {
                elementList->Remove(i);
            }

            elementList->Append(&element);
        }

        // Here is where we check the children.
        for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
        {
            this->AddToZIndexList(*iChild);
        }
    }

    void Server::RemoveFromZIndexList(Element &element)
    {
        auto elementList = this->FindListContainingElement(element);
        if (elementList != nullptr)
        {
            elementList->Remove(elementList->Find(&element));
        }
    }

    GT::List<Element*>* Server::GetElementListByZIndex(int zIndex)
    {
        auto item = this->elementsUsingZIndex.Find(zIndex);
        if (item != nullptr)
        {
            return item->value;
        }

        return nullptr;
    }

    GT::List<Element*>* Server::FindListContainingElement(Element &element)
    {
        for (size_t i = 0; i < this->elementsUsingZIndex.count; ++i)
        {
            auto list = this->elementsUsingZIndex.buffer[i]->value;

            if (list->Find(&element) != nullptr)
            {
                return list;
            }
        }

        return nullptr;
    }

    void Server::GetElementUnderPoint(int x, int y, bool fromMouseInput, Element &base, const GT::Rect<int> &parentRect, const GT::Rect<int> &parentScissorRect, Element* &result)
    {
        if (base.IsVisible())
        {
            // We need some rectangles
            GT::Rect<int> rect;
            GT::Rect<int> scissorRect;
            GT::Rect<int> childrenScissorRect;
            base.GetRects(parentRect, parentScissorRect, rect, scissorRect, childrenScissorRect);

            bool isOverBaseElement = false;
            if (scissorRect.Contains(x, y))
            {
                isOverBaseElement = true;
            }

            // If the point is over the root element, we need to check the children. If it's not over the base element, it's impossible that
            // it'd be over any children.
            if (isOverBaseElement)
            {
                Element* hoveredChild = nullptr;

                // What we need to do now is check if the point is in the resizing segment. If so, we don't actually want to check the
                // children. This is only done if we are doing this check from mouse input.
                if (fromMouseInput && base.style.allowMouseResize->value)
                {
                    if ((x >= scissorRect.left   && x <= scissorRect.left   + static_cast<int>(base.style.leftGripperWidth->value))  ||
                        (x <= scissorRect.right  && x >= scissorRect.right  - static_cast<int>(base.style.rightGripperWidth->value)) ||
                        (y >= scissorRect.top    && y <= scissorRect.top    + static_cast<int>(base.style.topGripperWidth->value))   ||
                        (y <= scissorRect.bottom && y >= scissorRect.bottom - static_cast<int>(base.style.bottomGripperWidth->value)))
                    {
                        result = &base;
                        return;
                    }
                }

                // Remember that we're only checking children who are NOT using a z-index. The children that ARE using the z-index will be
                // done separately.
                for (auto iChild = base.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
                {
                    if (!iChild->UsesZIndex())
                    {
                        this->GetElementUnderPoint(x, y, fromMouseInput, *iChild, rect, childrenScissorRect, hoveredChild);
                    }
                }

                if (hoveredChild != nullptr)
                {
                    result = hoveredChild;
                }
                else
                {
                    // If we get here it means the base element (the input element) has the point over it. We need to ensure we're not ignoring mouse input.
                    if (!fromMouseInput || !base.style.transparentMouseInput->value)
                    {
                        result = &base;
                    }
                }
            }
        }
    }

    void Server::SetCursor(GT::Cursor cursor)
    {
        if (cursor != this->currentCursor)
        {
            this->currentCursor = cursor;
            this->eventHandler->OnChangeCursor(cursor);
        }
    }


    void Server::ReleasePushedElement(bool blockEvent)
    {
        // If we have a pushed element, that will need to be released. It will still have the hovered state applied to it, though.
        if (this->pushedElement != nullptr)
        {
            // There is a chance the event handlers will clear this->pushedElement, such as if the element is made invisible in the
            // event handler. To resolve, we just keep track of the element in a variable.
            auto element = this->pushedElement;

            if (blockEvent)
            {
                this->scripting.PostEvent_OnRelease(*element);
                element->OnRelease();
            }

            // If the pushed element is also the hovered element, then it has also been pressed. When an element is hidden while pushed down, it makes sense
            // to release it. However, we don't want to post an OnPressed event in this situation. Consider a button that hides the parent window when it's
            // clicked - that will actually cause two OnPressed events to be posted, which is not what we want.
            if (this->hoveredElements.root != nullptr && this->hoveredElements.root->value == this->pushedElement && element->IsVisible() && !blockEvent)
            {
                this->scripting.PostEvent_OnPressed(*element);
                element->OnPressed();
            }

            // Just make sure the pushed element is now null...
            this->pushedElement = nullptr;

            // We invalidate the mouse to ensure the correct cursor is showing.
            this->InvalidateMouse();
        }

        // We'll no longer be dragging the element...
        this->draggingPushedElement = false;

        /// ... or resizing.
        this->resizingPushedElement      = false;
        this->resizingUsingLeftGripper   = false;
        this->resizingUsingRightGripper  = false;
        this->resizingUsingTopGripper    = false;
        this->resizingUsingBottomGripper = false;

        // We need to reset the variable the controls whether or not we posted the OnTear() event.
        this->onTearEventPosted = false;
    }


    void Server::PostVisibilityEvents()
    {
        // OnShow
        for (size_t iElement = 0; iElement < this->elementsNeedingOnShow.count; ++iElement)
        {
            auto element = this->elementsNeedingOnShow[iElement];
            assert(element != nullptr);
            {
                element->OnShow();
                this->scripting.PostEvent_OnShow(*element);
            }
        }

        this->elementsNeedingOnShow.Clear();



        // OnHide
        for (size_t iElement = 0; iElement < this->elementsNeedingOnHide.count; ++iElement)
        {
            auto element = this->elementsNeedingOnHide[iElement];
            assert(element != nullptr);
            {
                element->OnHide();
                this->scripting.PostEvent_OnHide(*element);
            }
        }

        this->elementsNeedingOnHide.Clear();
    }

    void Server::MarkElementAsNeedingOnShow(Element &element)
    {
        this->elementsNeedingOnHide.RemoveFirstOccuranceOf(&element);

        if (!this->elementsNeedingOnShow.Exists(&element))
        {
            this->elementsNeedingOnShow.Insert(&element);
        }
    }

    void Server::MarkElementAsNeedingOnHide(Element &element)
    {
        this->elementsNeedingOnShow.RemoveFirstOccuranceOf(&element);

        if (!this->elementsNeedingOnHide.Exists(&element))
        {
            this->elementsNeedingOnHide.Insert(&element);
        }
    }

    void Server::PostShowTooltipEvents(double deltaTimeInSeconds)
    {
        float deltaTimeInSecondsF = static_cast<float>(deltaTimeInSeconds);

        for (size_t iTooltipProperties = 0; iTooltipProperties < this->elementTooltipProperties.count; ++iTooltipProperties)
        {
            auto element = this->elementTooltipProperties.buffer[iTooltipProperties]->key;
            assert(element != nullptr);
            {
                auto &tooltipProperties = this->elementTooltipProperties.buffer[iTooltipProperties]->value;

                if (!tooltipProperties.hasShowEventBeenPosted)
                {
                    tooltipProperties.hoverTime += deltaTimeInSecondsF;

                    if (tooltipProperties.hoverTime > this->tooltipDelay)
                    {
                        element->OnShowTooltip();
                        this->scripting.PostEvent_OnShowTooltip(*element);

                        tooltipProperties.hasShowEventBeenPosted = true;
                    }
                }
            }
        }
    }
}
