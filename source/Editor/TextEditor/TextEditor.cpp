// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/TextEditor/TextEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Scripting.hpp>
#include <GTEngine/GTEngine.hpp>
#include <easy_path/easy_path.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GT
{
    TextEditor::TextEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          mainElement(nullptr), textArea(nullptr), panelElement(nullptr), errorListElement(nullptr),
          textAreaEventHandler(new TextAreaEventHandler(this)),
          compilationErrorHandler(*this),
          proxyGame(nullptr),
          compilationScript(nullptr),
          isScriptFile(false)
    {
        char* fileContent = easyvfs_open_and_read_text_file(g_EngineContext->GetVFS(), absolutePath, nullptr);
        if (fileContent != nullptr)
        {
            auto &gui    = this->GetGUI();
            auto &script = this->GetScript();

            // We need to determine whether or not we are running a script.
            const char* extension = easypath_extension(absolutePath);
            assert(extension != nullptr);
            {
                this->isScriptFile = Strings::Equal<false>(extension, "lua")    ||
                                     Strings::Equal<false>(extension, "script") ||
                                     Strings::Equal<false>(extension, "gtscript");
            }


            // The file contents have been loaded, so now we need to create the text-box element that will display it.
            this->mainElement = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='text-editor' />");
            assert(this->mainElement != nullptr);
            {
                // The element has been created, but we need to execute a script to have it turn into a proper multi-line text box.
                script.Get(String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
                assert(script.IsTable(-1));
                {
                    script.Push("TextEditor");
                    script.GetTableValue(-2);
                    assert(script.IsFunction(-1));
                    {
                        script.PushValue(-2);   // <-- 'self'.
                        script.Push(this);      // <-- '_internalPtr'
                        script.Call(2, 0);
                    }
                }

                // Now what we need to do is actually set the text. This will be much quicker if done on the C++ side so that the script parser doesn't need to
                // parse potentially very large files.
                this->textArea = gui.GetElementByID(script.GetString(String::CreateFormatted("GTGUI.Server.GetElementByID('%s').TextBox.TextArea:GetID();", this->mainElement->id).c_str()));
                assert(this->textArea != nullptr);
                {
                    this->textArea->SetText(fileContent);
                    this->textArea->SetGUIStyleAttribute("background-color", "inherit");
                    this->textArea->SetGUIStyleAttribute("text-color",       "inherit");
                     
                    this->textArea->textManager.MoveCursorToStart();
                    gui.FocusElement(this->textArea);


                    // Now we need to attach an event handler to the text area so we can detect changes to the text. We propagate this to the scripting
                    // environment so that the owner tab can be marked as modified.
                    this->textArea->AttachEventHandler(*this->textAreaEventHandler);
                }

                
                // The panel.
                this->panelElement = gui.GetElementByID(script.GetString(String::CreateFormatted("GTGUI.Server.GetElementByID('%s').Panel:GetID();", this->mainElement->id).c_str()));
                assert(this->panelElement != nullptr);
                {
                    // If we're editting a regular text file (not a script), we don't want to show the panel.
                    if (!this->IsScriptFile())
                    {
                        this->panelElement->Hide();
                    }
                }


                // Error list.
                this->errorListElement = gui.GetElementByID(script.GetString(String::CreateFormatted("GTGUI.Server.GetElementByID('%s').ErrorList:GetID();", this->panelElement->id).c_str()));
                assert(this->errorListElement != nullptr);
                {
                }
            }

            // Do an initial compilation.
            if (this->IsScriptFile())
            {
                GameStateManager nullGameStateManager;
                this->proxyGame = new Game(nullGameStateManager);
                if (this->proxyGame->GetScript().Startup())
                {
                    // We need to setup a few things with the scripting.
                    this->compilationScript = &this->proxyGame->GetScript();
                    this->compilationScript->AttachErrorHandler(this->compilationErrorHandler);

                    this->CompileAndUpdateErrorOutput();
                }
            }

            
            easyvfs_free(fileContent);
        }
    }

    TextEditor::~TextEditor()
    {
        this->GetGUI().DeleteElement(this->mainElement);
        delete this->textAreaEventHandler;

        delete this->proxyGame;
    }


    bool TextEditor::IsScriptFile() const
    {
        return this->isScriptFile;
    }

    void TextEditor::CompileAndUpdateErrorOutput()
    {
        if (this->IsScriptFile())
        {
            assert(this->proxyGame         != nullptr);
            assert(this->compilationScript != nullptr);


            // The the error list first.
            this->ClearErrorList();

            // Now we compile.
            this->compilationScript->Execute("self = {}");
            this->compilationScript->Execute(this->textArea->GetText(), this->GetAbsolutePath());
        }
    }

    void TextEditor::OnScriptSyntaxError(GT::Script &script, int lineNumber, const char* message)
    {
        (void)script;

        this->AddItemToErrorList(lineNumber, message);
    }


    ///////////////////////////////////////////////////
    // Virtual Methods.

    void TextEditor::Show()
    {
        this->mainElement->Show();
        this->GetGUI().FocusElement(this->textArea);
        this->GetGUI().PositionCaret(false, false);
    }

    void TextEditor::Hide()
    {
        this->mainElement->Hide();
    }

    bool TextEditor::Save()
    {
        bool wasSaved = false;

        auto text = this->textArea->GetText();
        if (text != nullptr)
        {
            wasSaved = easyvfs_open_and_write_text_file(g_EngineContext->GetVFS(), this->GetAbsolutePath(), text);
        }
        else
        {
            wasSaved = easyvfs_open_and_write_text_file(g_EngineContext->GetVFS(), this->GetAbsolutePath(), "");
        }

        if (wasSaved)
        {
            this->UnmarkAsModified();

            // If the file was a script file, we want to do an immediate check for modifications so that any open scene editors can see
            // the changes seemlessly.
            if (this->IsScriptFile())
            {
                this->CompileAndUpdateErrorOutput();
                this->GetOwnerEditor().GetGame().GetDataFilesWatcher().CheckForChangesAndDispatchEvents();
            }
        }

        return wasSaved;
    }


    ///////////////////////////////////////////////////
    // Private

    void TextEditor::AddItemToErrorList(int lineNumber, const char* message)
    {
        auto &script = this->GetScript();

        script.GetGlobal("GTGUI");
        assert(script.IsTable(-1));
        {
            script.Push("GUIServer");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("GetElementByID");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(this->errorListElement->id);
                    script.Call(1, 1);
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("AddItem");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);    // 'self'
                                script.Push(lineNumber);
                                script.Push(message);
                                script.Call(3, 0);
                            }
                        }
                        script.Pop(1);      // <-- return value from GetElementByID().
                    }
                }
            }
            script.Pop(1);
        }
        script.Pop(1);
    }

    void TextEditor::ClearErrorList()
    {
        auto &script = this->GetScript();

        script.GetGlobal("GTGUI");
        assert(script.IsTable(-1));
        {
            script.Push("GUIServer");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("GetElementByID");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(this->errorListElement->id);
                    script.Call(1, 1);
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("Clear");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);    // 'self'
                                script.Call(1, 0);
                            }
                        }
                        script.Pop(1);      // <-- return value from GetElementByID().
                    }
                }
            }
            script.Pop(1);
        }
        script.Pop(1);
    }




    ///////////////////////////////////////////////////
    // TextAreaEventHandler.

    TextEditor::TextAreaEventHandler::TextAreaEventHandler(TextEditor* ownerTextEditorIn)
        : ownerTextEditor(ownerTextEditorIn)
    {
    }

    void TextEditor::TextAreaEventHandler::OnTextChanged(GTGUI::GUIElement &)
    {
        assert(this->ownerTextEditor != nullptr);
        {
            if (!this->ownerTextEditor->IsMarkedAsModified())
            {
                this->ownerTextEditor->MarkAsModified();
            }
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
