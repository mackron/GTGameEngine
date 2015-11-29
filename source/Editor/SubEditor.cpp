// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/SubEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <easy_path/easy_path.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GT
{
    SubEditor::SubEditor(Editor &ownerEditorIn, const char* absolutePathIn, const char* relativePathIn)
        : m_ownerEditor(ownerEditorIn), m_absolutePath(absolutePathIn), m_relativePath(relativePathIn),
          m_tabElement(nullptr), m_tabEventHandler(*this),
          m_isMarkedAsModified(false), m_isMarkingAsModifiedEnabled(true)
    {
        // We need to create a tab for this new editor.
        //
        // To do this, we're going to first push the main tab bar. Then, we're going to call AddTab() on that tab bar with the
        // name of the file (not the full path).
        auto &gui    = this->GetGUI();
        auto &script = this->GetScript();
        
        script.Get("GTGUI.Server.GetElementByID('Editor_TabBar')");
        assert(script.IsTable(-1));
        {
            script.Push("AddTab");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);
                script.Push(easypath_file_name(m_absolutePath.c_str()));
                script.Call(2, 1);

                // The top item on the stack is the tab GUI element. We now need to set the 'absolutePath' and 'relativePath' attributes.
                script.SetTableValue(-1, "absolutePath", m_absolutePath.c_str());
                script.SetTableValue(-1, "relativePath", m_relativePath.c_str());

                // We now want to get the ID of the element so we can retrieve it with gui.GetElementByID().
                script.Push("GetID");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);
                    script.Call(1, 1);

                    // At this point, the top item will be the ID of the new tab. We will use that to retrieve it from the GUI object.
                    assert(script.IsString(-1));
                    {
                        m_tabElement = gui.GetElementByID(script.ToString(-1));

                        // Now we need to attach an event handler to the tab.
                        assert(m_tabElement != nullptr);
                        {
                            m_tabElement->AttachEventHandler(m_tabEventHandler);
                        }
                    }
                    
                    script.Pop(1);      // <-- The return value from Editor_TabBar:AddTab():GetID().
                }

                script.Pop(1);          // <-- The return value from Editor_TabBar:AddTab().
            }
        }
        script.Pop(1);
    }

    SubEditor::~SubEditor()
    {
        // We need to remove the tab from the tab bar, since that is where we created it.
        if (m_tabElement != nullptr)
        {
            auto &script = this->GetScript();

            script.Get("GTGUI.Server.GetElementByID('Editor_TabBar')");
            assert(script.IsTable(-1));
            {
                script.Push("RemoveTab");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);       // 'self'

                    // We need to get the element.
                    script.GetGlobal("GTGUI");
                    assert(script.IsTable(-1));
                    {
                        script.Push("Server");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("GetElementByID");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.Push(m_tabElement->id);
                                script.Call(1, 1);

                                // The return value needs to be relocated so that it is placed as the second argument to Editor_TabBar:RemoveTab().
                                script.InsertIntoStack(-3);
                            }
                        }
                        script.Pop(1);  // GTGUI.Server
                    }
                    script.Pop(1);      // GTGUI


                    // Now we need to call the function, ignoring any return values.
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }
    }


    const char* SubEditor::GetAbsolutePath() const
    {
        return m_absolutePath.c_str();
    }

    const char* SubEditor::GetRelativePath() const
    {
        return m_relativePath.c_str();
    }

    
    void SubEditor::MarkAsModified()
    {
        if (m_isMarkingAsModifiedEnabled)
        {
            if (!m_isMarkedAsModified)
            {
                m_isMarkedAsModified = true;

                // We will modify the text of the tab to show a star to the right.
                String tabText(easypath_file_name(this->GetRelativePath())); tabText += "*";
                this->SetTabText(tabText.c_str());


                m_ownerEditor.UpdateMenuButtonEnableStates();
            }
        }
    }

    void SubEditor::UnmarkAsModified()
    {
        if (m_isMarkedAsModified)
        {
            m_isMarkedAsModified = false;

            // Now we want to remove the star.
            this->SetTabText(easypath_file_name(this->GetRelativePath()));


            m_ownerEditor.UpdateMenuButtonEnableStates();
        }
    }

    bool SubEditor::IsMarkedAsModified() const
    {
        return m_isMarkedAsModified;
    }


    void SubEditor::EnableMarkingAsModified()
    {
        m_isMarkingAsModifiedEnabled = true;
    }

    void SubEditor::DisableMarkingAsModified()
    {
        m_isMarkingAsModifiedEnabled = false;
    }

    bool SubEditor::IsMarkingAsModifiedEnabled() const
    {
        return m_isMarkingAsModifiedEnabled;
    }



    void SubEditor::Close()
    {
        m_ownerEditor.CloseFile(m_absolutePath.c_str());
    }

    void SubEditor::DeleteToolbar()
    {
        auto &script = this->GetScript();

        auto mainElement = this->GetMainElement();
        if (mainElement != nullptr)
        {
            script.Get(String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", mainElement->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("DeleteToolBar");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // 'self'.
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }
    }



    GameScript & SubEditor::GetScript()
    {
        return m_ownerEditor.GetGame().GetScript();
    }

    const GameScript & SubEditor::GetScript() const
    {
        return m_ownerEditor.GetGame().GetScript();
    }

    GTGUI::Server & SubEditor::GetGUI()
    {
        return m_ownerEditor.GetGame().GetGUI();
    }

    
    Game & SubEditor::GetGame()
    {
        return m_ownerEditor.GetGame();
    }
    
    const Game & SubEditor::GetGame() const
    {
        return m_ownerEditor.GetGame();
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void SubEditor::Show()
    {
    }

    void SubEditor::Hide()
    {
    }

    bool SubEditor::Save()
    {
        return true;
    }

    void SubEditor::OnUpdate(double)
    {
    }

    void SubEditor::OnKeyPressed(Key)
    {
    }

    void SubEditor::OnKeyReleased(Key)
    {
    }

    void SubEditor::OnMouseButtonDown(MouseButton, int, int)
    {
    }

    void SubEditor::OnMouseButtonUp(MouseButton, int, int)
    {
    }

    void SubEditor::OnMainWindowReceiveFocus()
    {
    }

    void SubEditor::OnMainWindowLoseFocus()
    {
    }



    void SubEditor::OnFileInsert(const DataFilesWatcher::Item &)
    {
    }

    void SubEditor::OnFileRemove(const DataFilesWatcher::Item &)
    {
    }

    void SubEditor::OnFileUpdate(const DataFilesWatcher::Item &)
    {
    }



    ///////////////////////////////////////////////////
    // Private Methods.

    void SubEditor::SetTabText(const char* text)
    {
        // This is slightly annoying because the text needs to be set on a child of the main tab. We do it all via the scripting interface.
        auto &script = this->GetScript();
        script.Get(String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GetTabElement()->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("text");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("SetText");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // 'self'
                    script.Push(text);
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }
        script.Pop(1);
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
