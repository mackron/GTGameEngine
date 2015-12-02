// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include "GUIStyleTokenizer.hpp"
#include "GUIStyleAttributeHandler.hpp"
#include <GTGE/GUI/GUIServer.hpp>
#include <GTGE/Core/String.hpp>
#include <GTGE/Core/stdlib.hpp>
#include <GTGE/GTEngine.hpp>
#include <easy_path/easy_path.h>

// Delete the headers below later on.
#include <iostream>

namespace GT
{
    /// Event handler used by the style server for intercepting styling errors.
    class GUIStyleServerCompilerErrorHandler : public GUIStyleScriptCompilerErrorHandler
    {
    public:

        /// Constructor.
        GUIStyleServerCompilerErrorHandler(GUIStyleServer &server)
            : m_server(server)
        {
        }

        /// GUIStyleScriptCompilerErrorHandler::OnError().
        void OnError(const GUIStyleScriptError &error)
        {
            this->m_server.OnCompilerError(error);
        }


    private:

        /// A reference to the main style server.
        GUIStyleServer &m_server;


    private:    // No copying.
        GUIStyleServerCompilerErrorHandler(const GUIStyleServerCompilerErrorHandler &);
        GUIStyleServerCompilerErrorHandler & operator=(const GUIStyleServerCompilerErrorHandler &);
    };



    //////////////////////////////////////
    // GUIStyleServer

    GUIStyleServer::GUIStyleServer(GUIServer &server)
        : server(server),
          variables(),
          classes(), defaultStyleClass(nullptr), rootElementStyleClass(nullptr),
          attributeHandlers(),
          errorStack(),
          compilerStack()
    {
        // Before doing anything, we need to load our style attribute handlers.
        this->LoadGUIStyleAttributeHandlers();

        // We need to add a few default things.
        if (!this->LoadDefaults())
        {
            this->server.PostError("GUIStyleServer::GUIStyleServer() - Error loading defaults.");
        }
    }

    GUIStyleServer::~GUIStyleServer()
    {
        for (size_t i = 0; i < this->variables.count; ++i)
        {
            GT::Strings::Delete(this->variables.buffer[i]->value);
        }

        this->DeleteAllStyleClasses();
        this->UnloadGUIStyleAttributeHandlers();

        for (size_t iCompiler = 0; iCompiler < this->compilerStack.count; ++iCompiler)
        {
            delete this->compilerStack[iCompiler];
        }
    }

    bool GUIStyleServer::Load(const char* script, const char* baseURLPath, const char* identifier)
    {
        GUIStyleServerCompilerErrorHandler errorHandler(*this);

        auto compiler = new GUIStyleScriptCompiler(identifier);
        compiler->SetErrorHandler(&errorHandler);

        if (compiler->Compile(script, baseURLPath))
        {
            // We compiled successfully. We now need to add everything to the environment. We don't need or want an error handler anymore.
            compiler->SetErrorHandler(nullptr);


            // Variables.
            size_t variableCount = compiler->GetVariableCount();
            for (size_t iVariable = 0; iVariable < variableCount; ++iVariable)
            {
                auto &variable = compiler->GetVariableByIndex(iVariable);
                {
                    this->AddVariable(variable.GetName(), variable.GetValue());
                }
            }


            // Classes.
            size_t classCount = compiler->GetClassCount();
            for (size_t iClass = 0; iClass < classCount; ++iClass)
            {
                auto &compilerClass = compiler->GetClassByIndex(iClass);
                {
                    // If the class already exists, just merge it. If not, we just create a new one.
                    auto newClass = this->GetStyleClass(compilerClass.GetName());
                    if (newClass == nullptr)
                    {
                        newClass = this->CreateStyleClass(compilerClass.GetName());
                    }

                    assert(newClass != nullptr);
                    {
                        this->MergeStyleClass(*newClass, compilerClass);

                        // We now need to do create or merge the modifier classes.
                        auto &compilerSubClasses = compilerClass.GetSubClasses();
                        for (size_t iSubClass = 0; iSubClass < compilerSubClasses.count; ++iSubClass)
                        {
                            auto &compilerModifierClass = compilerSubClasses.buffer[iSubClass]->value;
                            auto  modifierClassType     = ToStyleClassType(compilerSubClasses.buffer[iSubClass]->key);
                            
                            if (modifierClassType != GUIStyleClassType_None)
                            {
                                auto modifierClass = newClass->GetModifierClass(modifierClassType);
                                if (modifierClass == nullptr)
                                {
                                    modifierClass = this->CreateModifierStyleClass(*newClass, modifierClassType);
                                }

                                assert(modifierClass != nullptr);
                                {
                                    this->MergeStyleClass(*modifierClass, compilerModifierClass);
                                }
                            }
                        }
                    }
                }
            }

            
            // A pointer to the compiler needs to be stored in our list so it can later be removed.
            this->compilerStack.PushBack(compiler);

            return true;
        }

        return false;
    }

    void GUIStyleServer::Unload(const char* identifier, bool firstOccuranceOnly)
    {
        if (identifier != nullptr)
        {
            // We need to loop over each compiler and remove any that share the given identifier.
            size_t iCompiler = 0;
            while (iCompiler < this->compilerStack.count)
            {
                auto compiler = this->compilerStack[iCompiler];
                assert(compiler != nullptr);
                {
                    if (GT::Strings::Equal(compiler->GetIdentifier(), identifier))
                    {
                        this->UnloadCompilerByIndex(iCompiler);

                        // Optimization.
                        if (firstOccuranceOnly)
                        {
                            break;
                        }
                    }
                    else
                    {
                        ++iCompiler;
                    }
                }
            }
        }
    }

    bool GUIStyleServer::LoadFromFile(const char* filePath)
    {
        char* pFileData = easyvfs_open_and_read_text_file(GT::g_EngineContext->GetVFS(), filePath, nullptr);
        if (pFileData != nullptr)
        {
            char absolutePath[EASYVFS_MAX_PATH];
            if (!easyvfs_find_absolute_path(GT::g_EngineContext->GetVFS(), filePath, absolutePath, sizeof(absolutePath)))
            {
                // Failed to retrieve the absolute path. Just use the original path.
                strcpy_s(absolutePath, sizeof(absolutePath), filePath);
            }

            // We want to unload the file before re-loading it.
            this->UnloadFile(absolutePath);

            char absolutePathBase[EASYVFS_MAX_PATH];
            easypath_copy_base_path(absolutePath, absolutePathBase, sizeof(absolutePathBase));

            bool successful = this->Load(pFileData, absolutePathBase, absolutePath);
            

            easyvfs_free(pFileData);
            return successful;
        }
        else
        {
            char msg[256];
            GT::IO::snprintf(msg, 256, "Failed to open file: %s", filePath);

            this->server.PostError(msg);
            return false;
        }
    }

    void GUIStyleServer::UnloadFile(const char* filePath)
    {
        // The file path needs to be absolute.
        char absolutePath[EASYVFS_MAX_PATH];
        if (!easypath_is_absolute(filePath))
        {
            if (!easyvfs_find_absolute_path(GT::g_EngineContext->GetVFS(), filePath, absolutePath, sizeof(absolutePath))) {
                strcpy_s(absolutePath, sizeof(absolutePath), filePath);
            }
        }
        else
        {
            strcpy_s(absolutePath, sizeof(absolutePath), filePath);
        }
        
        this->Unload(absolutePath, false);       // 'true' means to only remove the first occurance. There should only ever be a single file in the compilation stack at a time.
    }


    void GUIStyleServer::AddVariable(const char* name, const char* valueIn, ptrdiff_t nameSize, ptrdiff_t valueSize)
    {
        // We need our own copy of the value.
        auto value = GT::Strings::Create(valueIn, valueSize);

        // If the old value exists, it must be deleted.
        auto item = this->variables.Find(name, nameSize);
        if (item != nullptr)
        {
            // Item already exists - just replace it. We need to also delete the old value.
            GT::Strings::Delete(item->value);
            item->value = value;
        }
        else
        {
            // Item doesn't exist - just add it.
            this->variables.Add(name, nameSize, value);
        }
    }

    void GUIStyleServer::RemoveVariable(const char* name, ptrdiff_t nameSizeInTs)
    {
        auto item = this->variables.Find(name, nameSizeInTs);
        if (item != nullptr)
        {
            GT::Strings::Delete(item->value);
            item->value = nullptr;

            this->variables.RemoveByIndex(item->index);
        }
    }

    void GUIStyleServer::AddStyleClass(GUIStyleClass &styleClass)
    {
        this->classes.Insert(styleClass);
    }

    void GUIStyleServer::RemoveStyleClass(GUIStyleClass &styleClass)
    {
        this->classes.Remove(styleClass);
    }

    GUIStyleClass* GUIStyleServer::CreateStyleClass(const char *name, ptrdiff_t nameSizeInBytes)
    {
        auto newClass = new GUIStyleClass(*this, name, nameSizeInBytes);
        this->AddStyleClass(*newClass);

        return newClass;
    }

    void GUIStyleServer::DeleteStyleClass(GUIStyleClass &styleClass, bool refreshHostStacks)
    {
        // We first remove the class.
        this->RemoveStyleClass(styleClass);

        // All modifier classes need to be deleted before the style class.
        this->DeleteAllModifierStyleClasses(styleClass);

        // We need to remove the style class from any host stacks.
        while (styleClass.hosts.root != nullptr)
        {
            styleClass.hosts.root->value->Detach(styleClass, refreshHostStacks);
        }

        // All internal style classes are allocated with 'new'...
        delete &styleClass;
    }

    void GUIStyleServer::DeleteAllStyleClasses()
    {
        GUIStyleClass* root;
        while ((root = this->classes.GetRoot()) != nullptr)
        {
            this->DeleteStyleClass(*root);
        }
    }


    GUIStyleClass* GUIStyleServer::CreateModifierStyleClass(GUIStyleClass& baseClass, GUIStyleClassType modifierType)
    {
        auto newClass = new GUIStyleClass(*this, "", 0);
        newClass->type = modifierType;

        // The modifier must be set on the base class.
        baseClass.SetModifierClass(*newClass);

        return newClass;
    }

    void GUIStyleServer::DeleteModifierStyleClass(GUIStyleClass &baseClass, GUIStyleClassType modifierType)
    {
        auto modifierClass = baseClass.GetModifierClass(modifierType);
        if (modifierClass != nullptr)
        {
            // We first unset the modifier class on the base class.
            baseClass.UnsetModifierClass(modifierType);

            // Now we can finally delete the modifier class.
            delete modifierClass;
        }
    }

    void GUIStyleServer::DeleteAllModifierStyleClasses(GUIStyleClass &baseClass)
    {
        for (int i = 0; i < StyleClassTypeCount; ++i)
        {
            this->DeleteModifierStyleClass(baseClass, static_cast<GUIStyleClassType>(i));
        }
    }


    const char* GUIStyleServer::GetVariable(const char* name, ptrdiff_t nameSize)
    {
        auto item = this->variables.Find(name, nameSize);

        // We will recursively call this method so that we have the final value. Doing this allows us to do variable alias'. Might come in handy...
        if (item)
        {
            auto temp = this->GetVariable(item->value, -1);
            if (temp != nullptr)
            {
                return temp;
            }

            return item->value;
        }

        return nullptr;
    }

    GUIStyleClass* GUIStyleServer::GetStyleClass(const char *name, ptrdiff_t nameSize)
    {
        return this->classes.FindByName(name, nameSize); 
    }

    GUIStyleClass* GUIStyleServer::GetDefaultStyleClass()
    {
        if (this->defaultStyleClass == nullptr)
        {
            this->defaultStyleClass = this->GetStyleClass("*");
        }
        
        return this->defaultStyleClass;
    }
    
    GUIStyleClass* GUIStyleServer::GetRootElementStyleClass()
    {
        if (this->rootElementStyleClass == nullptr)
        {
            this->rootElementStyleClass = this->GetStyleClass("#_Root");
        }
        
        return this->rootElementStyleClass;
    }


    bool GUIStyleServer::SetGUIStyleAttribute(GUIStyleClass &styleClass, const char* name, ptrdiff_t nameSizeInTs, const char* value, ptrdiff_t valueSizeInTs)
    {
        // The result will be false if we don't find the attribute.
        bool result   = false;
        auto iHandler = this->attributeHandlers.Find(name, nameSizeInTs);
        if (iHandler)
        {
            auto handler = iHandler->value;
            assert(handler != nullptr);

            // We found it. If the value is set to 'inherit', we can actually
            if (value == nullptr || GT::Strings::Equal(value, valueSizeInTs, ""))
            {
                handler->Unset(styleClass);
                result = true;
            }
            else if (GT::Strings::Equal(value, valueSizeInTs, "inherit"))
            {
                handler->SetInherit(styleClass, true, true);
                result = true;
            }
            else
            {
                auto variableValue = this->GetVariable(value, valueSizeInTs);
                if (variableValue != nullptr)
                {
                    value         = variableValue;
                    valueSizeInTs = -1;
                }

                handler->SetInherit(styleClass, false, false);
                result = handler->Set(styleClass, value, valueSizeInTs);
            }
        }

        return result;
    }

    bool GUIStyleServer::UnsetGUIStyleAttribute(GUIStyleClass &styleClass, const char* name, ptrdiff_t nameSizeInTs)
    {
        return this->SetGUIStyleAttribute(styleClass, name, nameSizeInTs, nullptr, -1);
    }

    GT::String GUIStyleServer::GetGUIStyleAttribute(GUIStyleClass &styleClass, const char* name, ptrdiff_t nameSizeInTs)
    {
        auto handler = this->attributeHandlers.Find(name, nameSizeInTs);
        if (handler != nullptr)
        {
            return handler->value->Get(styleClass);
        }

        return "";
    }


    void GUIStyleServer::ClearErrors()
    {
        this->errorStack.Clear();
    }

    bool GUIStyleServer::GetLastError(GUIStyleScriptError &errorOut)
    {
        if (this->errorStack.count > 0)
        {
            errorOut = this->errorStack.GetBack();
            this->errorStack.PopBack();

            return true;
        }

        return false;
    }

    void GUIStyleServer::OnCompilerError(const GUIStyleScriptError &error)
    {
        this->PostError(error);
    }


    bool GUIStyleServer::LoadDefaults()
    {
        bool success = true;

        // Some variables. It's faster to do this manually through the AddVariable() API then it is to load it as a script.
        this->AddVariable("white", "#ffffff");
        this->AddVariable("black", "#000000");
        this->AddVariable("red",   "#ff0000");
        this->AddVariable("green", "#00ff00");
        this->AddVariable("blue",  "#0000ff");

        // We need a default style class.
        success = success && this->Load
        (
            "*"
            "{"
            "    width:100%;"
            "    height:auto;"
            "    min-width:auto;"
            "    max-width:auto;"
            "    min-height:auto;"
            "    max-height:auto;"
            "    relative-width-mode:inner;"
            "    relative-height-mode:inner;"
            "    flex-child-width:false;"
            "    flex-child-height:false;"
            "    background-color:none;"
            "    background-image:none;"
            "    background-image-color:inherit;"
            "    background-align:center center;"
            "    background-repeat:none;"
            "    border:none;"
            "    padding:0px;"
            "    margin:0px;"
            "    font-family:inherit;"
            "    font-size:inherit;"
            "    font-weight:inherit;"
            "    font-slant:inherit;"
            "    text-color:inherit;"
            "    text-selection-color:inherit;"
            "    text-selection-background-color:inherit;"
            "    text-selection-background-color-blurred:inherit;"
            "    cursor:arrow;"
            "    child-plane:vertical;"
            "    horizontal-align:left;"
            "    vertical-align:top;"
            "    visible:true;"
            "    z-index:auto;"
            "    transparent-mouse-input:false;"
            "    enabled:true;"
            "    text-cursor-color:inherit;"
            "    can-receive-focus-from-mouse:false;"
            "    positioning:auto;"
            "    left:0px;"
            "    top:0px;"
            "    position-origin:inner;"
            "    inner-offset-x:0px"
            "    inner-offset-y:0px"
            "    editable-text:false;"
            "    single-line-text:false;"
            "    opacity:1;"
            "    compound-opacity:true;"
            "    enable-shadow:false;"
            "    shadow-color:#000;"
            "    shadow-blur-radius:4px;"
            "    shadow-offset-x:0px;"
            "    shadow-offset-y:0px;"
            "    shadow-extrusion-x:8px;"
            "    shadow-extrusion-y:8px;"
            "    shadow-opacity:50%;"
            "    allow-mouse-drag:false;"
            "    constrain-mouse-drag-x:false;"
            "    constrain-mouse-drag-y:false;"
            "    mouse-drag-clamp-mode-x:clickpos;"
            "    mouse-drag-clamp-mode-y:clickpos;"
            "    allow-mouse-resize:false;"
            "    left-gripper-width:0px;"
            "    right-gripper-width:0px;"
            "    top-gripper-width:0px;"
            "    bottom-gripper-width:0px;"
            "}"

            // Do not ever set any attribute in #_Root to 'inherit'.
            "#_Root"
            "{"
            "    width:0px;"
            "    height:0px;"
            "    min-width:auto;"
            "    max-width:auto;"
            "    min-height:auto;"
            "    max-height:auto;"
            "    relative-width-mode:inner;"
            "    relative-height-mode:inner;"
            "    flex-child-width:false;"
            "    flex-child-height:false;"
            "    background-color:none;"
            "    background-image:none;"
            "    background-image-color:#fff;"
            "    background-align:center center;"
            "    background-repeat:none;"
            "    border:none;"
            "    padding:0px;"
            "    margin:0px;"
            "    font-family:liberation sans;"
            "    font-size:9pt;"
            "    font-weight:default;"
            "    font-slant:default;"
            "    text-color:#000;"
            "    text-selection-color:#000;"
            "    text-selection-background-color:#55a;"
            "    text-selection-background-color-blurred:#338;"
            "    cursor:arrow;"
            "    child-plane:vertical;"
            "    horizontal-align:left;"
            "    vertical-align:top;"
            "    visible:true;"
            "    z-index:auto;"
            "    transparent-mouse-input:false;"
            "    enabled:true;"
            "    text-cursor-color:#000;"
            "    can-receive-focus-from-mouse:false;"
            "    positioning:absolute;"                 // <-- important to set this to 'absolute'.
            "    left:0px;"
            "    top:0px;"
            "    position-origin:inner;"
            "    inner-offset-x:0px"
            "    inner-offset-y:0px"
            "    editable-text:false;"
            "    single-line-text:false;"
            "    opacity:1;"
            "    compound-opacity:true;"
            "    enable-shadow:false;"
            "    shadow-color:#000;"
            "    shadow-blur-radius:4px;"
            "    shadow-offset-x:0px;"
            "    shadow-offset-y:0px;"
            "    shadow-extrusion-x:8px;"
            "    shadow-extrusion-y:8px;"
            "    shadow-opacity:50%;"
            "    allow-mouse-drag:false;"
            "    constrain-mouse-drag-x:false;"
            "    constrain-mouse-drag-y:false;"
            "    mouse-drag-clamp-mode-x:clickpos;"
            "    mouse-drag-clamp-mode-y:clickpos;"
            "    allow-mouse-resize:false;"
            "    left-gripper-width:0px;"
            "    right-gripper-width:0px;"
            "    top-gripper-width:0px;"
            "    bottom-gripper-width:0px;"
            "}"
        );

        return success;
    }



    void GUIStyleServer::PrintDetails()
    {
        /*
        std::cout << "--- GUIStyleServer Details ---" << std::endl;
        std::cout << "Variables:" << std::endl;
        for (size_t i = 0; i < this->variables.count; ++i)
        {
            std::cout << "    " << this->variables.buffer[i]->key << " = " << this->variables.buffer[i]->value << std::endl;
        }

        std::cout << "Classes:" << std::endl;
        for (size_t i = 0; i < this->classes.count; ++i)
        {
            std::cout << "    " << this->classes.buffer[i]->key << std::endl;
        }
        */
    }
}


// This section here implements the LoadGUIStyleAttributeHandlers() method. We keep this separate so that it's easy to find as new style attributes are added.
#define ADD_HANDLER(ctor) this->AddGUIStyleAttributeHandler(*(new ctor))

namespace GT
{
    void GUIStyleServer::LoadGUIStyleAttributeHandlers()
    {
        // Primitive attributes.
        ADD_HANDLER(AttributeHandlers::width);
        ADD_HANDLER(AttributeHandlers::height);
        ADD_HANDLER(AttributeHandlers::minWidth);
        ADD_HANDLER(AttributeHandlers::maxWidth);
        ADD_HANDLER(AttributeHandlers::minHeight);
        ADD_HANDLER(AttributeHandlers::maxHeight);
        ADD_HANDLER(AttributeHandlers::relativeWidthMode);
        ADD_HANDLER(AttributeHandlers::relativeHeightMode);
        ADD_HANDLER(AttributeHandlers::flexChildWidth);
        ADD_HANDLER(AttributeHandlers::flexChildHeight);
        ADD_HANDLER(AttributeHandlers::backgroundColour);
        ADD_HANDLER(AttributeHandlers::backgroundImage);
        ADD_HANDLER(AttributeHandlers::backgroundImageColour);
        ADD_HANDLER(AttributeHandlers::backgroundAlignX);
        ADD_HANDLER(AttributeHandlers::backgroundAlignY);
        ADD_HANDLER(AttributeHandlers::backgroundRepeatX);
        ADD_HANDLER(AttributeHandlers::backgroundRepeatY);
        ADD_HANDLER(AttributeHandlers::borderLeftWidth);
        ADD_HANDLER(AttributeHandlers::borderRightWidth);
        ADD_HANDLER(AttributeHandlers::borderTopWidth);
        ADD_HANDLER(AttributeHandlers::borderBottomWidth);
        ADD_HANDLER(AttributeHandlers::borderLeftColour);
        ADD_HANDLER(AttributeHandlers::borderRightColour);
        ADD_HANDLER(AttributeHandlers::borderTopColour);
        ADD_HANDLER(AttributeHandlers::borderBottomColour);
        ADD_HANDLER(AttributeHandlers::paddingLeft);
        ADD_HANDLER(AttributeHandlers::paddingRight);
        ADD_HANDLER(AttributeHandlers::paddingTop);
        ADD_HANDLER(AttributeHandlers::paddingBottom);
        ADD_HANDLER(AttributeHandlers::marginLeft);
        ADD_HANDLER(AttributeHandlers::marginRight);
        ADD_HANDLER(AttributeHandlers::marginTop);
        ADD_HANDLER(AttributeHandlers::marginBottom);
        ADD_HANDLER(AttributeHandlers::childPlane);
        ADD_HANDLER(AttributeHandlers::horizontalAlign);
        ADD_HANDLER(AttributeHandlers::verticalAlign);
        ADD_HANDLER(AttributeHandlers::cursor);
        ADD_HANDLER(AttributeHandlers::visible);
        ADD_HANDLER(AttributeHandlers::zIndex);
        ADD_HANDLER(AttributeHandlers::transparentMouseInput);
        ADD_HANDLER(AttributeHandlers::enabled);
        ADD_HANDLER(AttributeHandlers::textCursorColour);
        ADD_HANDLER(AttributeHandlers::canReceiveFocusFromMouse);
        ADD_HANDLER(AttributeHandlers::positioning);
        ADD_HANDLER(AttributeHandlers::left);
        ADD_HANDLER(AttributeHandlers::right);
        ADD_HANDLER(AttributeHandlers::top);
        ADD_HANDLER(AttributeHandlers::bottom);
        ADD_HANDLER(AttributeHandlers::positionOrigin);
        ADD_HANDLER(AttributeHandlers::innerOffsetX);
        ADD_HANDLER(AttributeHandlers::innerOffsetY);
        ADD_HANDLER(AttributeHandlers::fontFamily);
        ADD_HANDLER(AttributeHandlers::fontSize);
        ADD_HANDLER(AttributeHandlers::fontWeight);
        ADD_HANDLER(AttributeHandlers::fontSlant);
        ADD_HANDLER(AttributeHandlers::textColour);
        ADD_HANDLER(AttributeHandlers::textSelectionColour);
        ADD_HANDLER(AttributeHandlers::textSelectionBackgroundColour);
        ADD_HANDLER(AttributeHandlers::textSelectionBackgroundColourBlurred);
        ADD_HANDLER(AttributeHandlers::editableText);
        ADD_HANDLER(AttributeHandlers::singleLineText);
        ADD_HANDLER(AttributeHandlers::opacity);
        ADD_HANDLER(AttributeHandlers::compoundOpacity);
        ADD_HANDLER(AttributeHandlers::enableShadow);
        ADD_HANDLER(AttributeHandlers::shadowColour);
        ADD_HANDLER(AttributeHandlers::shadowBlurRadius);
        ADD_HANDLER(AttributeHandlers::shadowOffsetX);
        ADD_HANDLER(AttributeHandlers::shadowOffsetY);
        ADD_HANDLER(AttributeHandlers::shadowExtrusionX);
        ADD_HANDLER(AttributeHandlers::shadowExtrusionY);
        ADD_HANDLER(AttributeHandlers::shadowOpacity);
        ADD_HANDLER(AttributeHandlers::allowMouseDrag);
        ADD_HANDLER(AttributeHandlers::constrainMouseDragX);
        ADD_HANDLER(AttributeHandlers::constrainMouseDragY);
        ADD_HANDLER(AttributeHandlers::mouseDragClampModeX);
        ADD_HANDLER(AttributeHandlers::mouseDragClampModeY);
        ADD_HANDLER(AttributeHandlers::allowMouseResize);
        ADD_HANDLER(AttributeHandlers::leftGripperWidth);
        ADD_HANDLER(AttributeHandlers::rightGripperWidth);
        ADD_HANDLER(AttributeHandlers::topGripperWidth);
        ADD_HANDLER(AttributeHandlers::bottomGripperWidth);

        // Shorthand attributes.
        ADD_HANDLER(AttributeHandlers::backgroundAlign);
        ADD_HANDLER(AttributeHandlers::backgroundRepeat);
        ADD_HANDLER(AttributeHandlers::background);
        ADD_HANDLER(AttributeHandlers::borderWidth);
        ADD_HANDLER(AttributeHandlers::borderColour);
        ADD_HANDLER(AttributeHandlers::borderLeft);
        ADD_HANDLER(AttributeHandlers::borderRight);
        ADD_HANDLER(AttributeHandlers::borderTop);
        ADD_HANDLER(AttributeHandlers::borderBottom);
        ADD_HANDLER(AttributeHandlers::border);
        ADD_HANDLER(AttributeHandlers::padding);
        ADD_HANDLER(AttributeHandlers::margin);
    }

    void GUIStyleServer::UnloadGUIStyleAttributeHandlers()
    {
        for (size_t i = 0; i < this->attributeHandlers.count; ++i)
        {
            delete this->attributeHandlers.buffer[i]->value;
        }
    }

    void GUIStyleServer::AddGUIStyleAttributeHandler(GUIStyleAttributeHandler &handler)
    {
        this->attributeHandlers.Add(handler.GetAttributeName(), &handler);
    }

    void GUIStyleServer::PostError(const GUIStyleScriptError &error)
    {
        this->errorStack.PushBack(error);
    }

    void GUIStyleServer::MergeStyleClass(GUIStyleClass &dest, const GUIStyleScriptCompilerClass &source)
    {
        // Includes.
        dest.AppendIncludes(source.GetIncludes());

        // Attributes.
        size_t attributeCount = source.GetAttributeCount();
        for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
        {
            auto &sourceAttribute = source.GetAttributeByIndex(iAttribute);
            {
                auto variableValue = this->GetVariable(sourceAttribute.GetValue());
                if (variableValue != nullptr)
                {
                    dest.SetAttribute(sourceAttribute.GetName(), variableValue);   // Value will be null terminated.
                }
                else
                {
                    dest.SetAttribute(sourceAttribute.GetName(), sourceAttribute.GetValue());
                }
            }
        }
    }

    void GUIStyleServer::UnloadCompilerByIndex(size_t compilerIndex)
    {
        auto compiler = this->compilerStack[compilerIndex];
        assert(compiler != nullptr);
        {
            // Classes.
            for (size_t iClass = 0; iClass < compiler->GetClassCount(); ++iClass)
            {
                auto &compilerClass = compiler->GetClassByIndex(iClass);
                {
                    this->UnloadCompilerClass(compilerClass, nullptr, compilerIndex);

                    auto &subClasses = compilerClass.GetSubClasses();
                    for (size_t iSubClass = 0; iSubClass < subClasses.count; ++iSubClass)
                    {
                        auto  modifierName  = subClasses.buffer[iSubClass]->key;
                        auto &modifierClass = subClasses.buffer[iSubClass]->value;

                        this->UnloadCompilerClass(modifierClass, modifierName, compilerIndex);
                    }
                }
            }


            // Variables.
            for (size_t iVariable = 0; iVariable < compiler->GetVariableCount(); ++iVariable)
            {
                this->UnloadCompilerVariable(compiler->GetVariableByIndex(iVariable), compilerIndex);
            }
        }

        // Don't forget to remove the compiler.
        delete compiler;
        this->compilerStack.Remove(compilerIndex);
    }

    void GUIStyleServer::UnloadCompilerVariable(const GUIStyleScriptCompilerVariable &compilerVariable, size_t compilerIndex)
    {
        if (!this->HasMoreRecentlyDefinedCompilerVariable(compilerVariable.GetName(), compilerIndex))
        {
            auto previousVariable = this->FindPreviouslyDefinedCompilerVariable(compilerVariable.GetName(), compilerIndex);
            if (previousVariable != nullptr)
            {
                // The same variable was previous defined, so we set the current value to that.
                this->AddVariable(previousVariable->GetName(), previousVariable->GetValue());
            }
            else
            {
                // The same variable was not previously defined. We need to unset it.
                this->RemoveVariable(compilerVariable.GetName());
            }
        }
    }

    bool GUIStyleServer::HasMoreRecentlyDefinedCompilerVariable(const char* variableName, size_t compilerIndex) const
    {
        for (size_t iCompiler = compilerIndex + 1; iCompiler < this->compilerStack.count; ++iCompiler)
        {
            auto compiler = this->compilerStack[iCompiler];
            assert(compiler != nullptr);
            {
                if (compiler->FindVariableByName(variableName))
                {
                    return true;
                }
            }
        }

        return false;
    }

    const GUIStyleScriptCompilerVariable* GUIStyleServer::FindPreviouslyDefinedCompilerVariable(const char* variableName, size_t compilerIndex) const
    {
        for (size_t iCompiler = compilerIndex; iCompiler > 0; --iCompiler)
        {
            auto compiler = this->compilerStack[iCompiler - 1];
            assert(compiler != nullptr);
            {
                auto variable = compiler->FindVariableByName(variableName);
                if (variable != nullptr)
                {
                    return variable;
                }
            }
        }

        return nullptr;
    }


    void GUIStyleServer::UnloadCompilerClass(const GUIStyleScriptCompilerClass &compilerClass, const char* modifierName, size_t compilerIndex)
    {
        // The way this works is we see if any properties have a more recent definition. If so, we don't do anything. Otherwise, we need
        // to scan backwards and find the next most recent definition.
        auto styleClass = this->GetStyleClass(compilerClass.GetName());
        if (styleClass != nullptr)
        {
            // Attributes.
            for (size_t iAttribute = 0; iAttribute < compilerClass.GetAttributeCount(); ++iAttribute)
            {
                auto &attribute = compilerClass.GetAttributeByIndex(iAttribute);
                {
                    if (!this->HasMoreRecentlyDefinedCompilerAttribute(compilerClass.GetName(), modifierName, attribute.GetName(), compilerIndex))
                    {
                        // This compiler defined the most recent definition of the attribute. We need to find the next more recent definition and change it. If
                        // we don't fine one, we need to undefine it.
                        auto previousAttribute = this->FindPreviouslyDefinedCompilerAttribute(compilerClass.GetName(), modifierName, attribute.GetName(), compilerIndex);
                        if (previousAttribute != nullptr)
                        {
                            // The same attribute was previously defined, so we set the current value to that.
                            if (modifierName != nullptr)
                            {
                                auto modifierClass = styleClass->GetModifierClass(ToStyleClassType(modifierName));
                                if (modifierClass != nullptr)
                                {
                                    modifierClass->SetAttribute(previousAttribute->GetName(), previousAttribute->GetValue());
                                }
                            }
                            else
                            {
                                styleClass->SetAttribute(previousAttribute->GetName(), previousAttribute->GetValue());
                            }
                        }
                        else
                        {
                            // The same attribute was not previously defined. We need to unset it.
                            styleClass->SetAttribute(attribute.GetName(), "");
                        }
                    }
                }
            }

            // Includes.
            styleClass->RemoveIncludes(compilerClass.GetIncludes());
        }
    }

    bool GUIStyleServer::HasMoreRecentlyDefinedCompilerAttribute(const char* className, const char* modifierName, const char* attributeName, size_t compilerIndex) const
    {
        for (size_t iCompiler = compilerIndex + 1; iCompiler < this->compilerStack.count; ++iCompiler)
        {
            auto compiler = this->compilerStack[iCompiler];
            assert(compiler != nullptr);
            {
                if (compiler->GetClassAttribute(className, modifierName, attributeName) != nullptr)
                {
                    return true;
                }
            }
        }

        return false;
    }

    const GUIStyleScriptCompilerClassAttribute* GUIStyleServer::FindPreviouslyDefinedCompilerAttribute(const char* className, const char* modifierName, const char* attributeName, size_t compilerIndex) const
    {
        for (size_t iCompiler = compilerIndex; iCompiler > 0; --iCompiler)
        {
            auto compiler = this->compilerStack[iCompiler - 1];
            assert(compiler != nullptr);
            {
                auto attribute = compiler->GetClassAttribute(className, modifierName, attributeName);
                if (attribute != nullptr)
                {
                    return attribute;
                }
            }
        }

        return nullptr;
    }
}
