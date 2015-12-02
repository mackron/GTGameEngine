// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/GUI/GUIServer.hpp>
#include <cassert>
#include <GTGE/GTEngine.hpp>

namespace GT
{
    GUIScriptServer::GUIScriptServer(GUIServer &server, GT::Script* script)
        : server(server), script(script), deleteScript(false)
    {
        // If no script was specified, we need to create one.
        if (this->script == nullptr)
        {
            this->script       = new GT::Script;
            this->deleteScript = true;
        }

        // Here we load the defaults. This will be constructed during the main server's constructor, so we don't really want to post
        // an error if this fails. This is OK, though, since it's a GTGUI issue if this fails, not a client application issue.
        this->LoadDefaults();
    }
    
    GUIScriptServer::~GUIScriptServer()
    {
        if (this->deleteScript)
        {
            delete this->script;
        }
    }
    
    bool GUIScriptServer::Execute(const char* scriptText, int returnValueCount)
    {
        return this->script->LoadAndExecute(scriptText, returnValueCount);
    }

    bool GUIScriptServer::ExecuteFromFile(const char *filename, int returnValueCount)
    {
        char* pFileData = easyvfs_open_and_read_text_file(GT::g_EngineContext->GetVFS(), filename, nullptr);
        if (pFileData != nullptr)
        {
            bool successful = this->Execute(pFileData, returnValueCount);

            easyvfs_free(pFileData);
            return successful;
        }
        else
        {
            char msg[256];
            GT::IO::snprintf(msg, 256, "Could not find file: %s", filename);
            
            this->server.PostError(msg);
            return false;
        }
    }


    GT::Script & GUIScriptServer::GetScript()
    {
        assert(this->script != nullptr);
        return *this->script;
    }

    void GUIScriptServer::RemoveElement(GUIElement &element)
    {
        // GTGUI.Server.RemoveElementByID(element.id);

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("RemoveElementByID");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(element.id);
                    this->script->Call(1, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::RemoveChild(GUIElement &parent, GUIElement &child)
    {
        // GTGUI.Server.RemoveChildByID(parent.id, child.id);

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("RemoveChildByID");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(parent.id);
                    this->script->Push(child.id);
                    this->script->Call(2, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::RemoveAllChildren(GUIElement &parent)
    {
        // GTGUI.Server.RemoveAllChilrenByID(parent.id);

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("RemoveAllChildrenByID");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(parent.id);
                    this->script->Call(1, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }


    void GUIScriptServer::PostEvent_OnMouseEnter(GUIElement &element)
    {
        // GTGUI.Server._Elems[element->id]:OnMouseEnter()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(element.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnMouseEnter");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnMouseLeave(GUIElement &element)
    {
        // GTGUI.Server._Elems[element->id]:OnMouseLeave()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(element.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnMouseLeave");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnPush(GUIElement &element)
    {
        // GTGUI.Server._Elems[element->id]:OnPush()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(element.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnPush");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnRelease(GUIElement &element)
    {
        // GTGUI.Server._Elems[element->id]:OnRelease()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(element.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnRelease");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnPressed(GUIElement &element)
    {
        // GTGUI.Server._Elems[element->id]:OnPressed()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(element.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnPressed");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnSize(GUIElement &element)
    {
        // GTGUI.Server._Elems[element->id]:OnSize()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(element.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnSize");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnMove(GUIElement &element)
    {
        // GTGUI.Server._Elems[element->id]:OnMove()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(element.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnMove");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }


    void GUIScriptServer::PostEvent_OnKeyPressed(GUIElement &receiver, char32_t key)
    {
        // GTGUI.Server.OnKeyPressed(receiver->id, key)

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("OnKeyPressed");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->Push(static_cast<int>(key));
                    this->script->Call(2, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    bool GUIScriptServer::PostEvent_OnKeyDown(GUIElement &receiver, char32_t key)
    {
        // GTGUI.Server.OnKeyDown(receiver->id, key)

        bool result = true;

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("OnKeyDown");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->Push(static_cast<int>(key));
                    this->script->Call(2, 1);

                    // We need to check the return value of this call. 
                    if (this->script->IsBoolean(-1))
                    {
                        result = this->script->ToBoolean(-1);
                    }

                    // The return value needs to be removed.
                    this->script->Pop(1);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);

        return result;
    }

    void GUIScriptServer::PostEvent_OnKeyUp(GUIElement &receiver, char32_t key)
    {
        // GTGUI.Server.OnKeyUp(receiver->id, key)

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("OnKeyUp");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->Push(static_cast<int>(key));
                    this->script->Call(2, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }


    void GUIScriptServer::PostEvent_OnMouseButtonDown(GUIElement &receiver, int button)
    {
        // GTGUI.Server.OnMouseButtonDown(receiver->id, button)

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("OnMouseButtonDown");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->Push(button);
                    this->script->Call(2, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }
    
    void GUIScriptServer::PostEvent_OnMouseButtonUp(GUIElement &receiver, int button)
    {
        // GTGUI.Server.OnMouseButtonUp(receiver->id, button)

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("OnMouseButtonUp");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->Push(button);
                    this->script->Call(2, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }
    
    void GUIScriptServer::PostEvent_OnMouseButtonDoubleClick(GUIElement &receiver, int button)
    {
        // GTGUI.Server.OnMouseButtonDoubleClick(receiver->id, button)

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("OnMouseButtonDoubleClick");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->Push(button);
                    this->script->Call(2, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    
    void GUIScriptServer::PostEvent_OnMouseMove(GUIElement &receiver, int relativePosX, int relativePosY)
    {
        // GTGUI.Server.OnMouseMove(receiver.id, relativePosX, relativePosY);

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("OnMouseMove");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->Push(relativePosX);
                    this->script->Push(relativePosY);
                    this->script->Call(3, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnMouseWheel(GUIElement &receiver, int delta, int relativePosX, int relativePosY)
    {
        // GTGUI.Server.OnMouseWheel(receiver->id, delta, relativePosX, relativePosY)

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("OnMouseWheel");
                this->script->GetTableValue(-2);
                assert(this->script->IsFunction(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->Push(delta);
                    this->script->Push(relativePosX);
                    this->script->Push(relativePosY);
                    this->script->Call(4, 0);
                }
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }


    void GUIScriptServer::PostEvent_OnFocus(GUIElement &receiver)
    {
        // GTGUI.Server._Elems[element->id]:OnFocus()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnFocus");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnBlur(GUIElement &receiver)
    {
        // GTGUI.Server._Elems[element->id]:OnBlur()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnBlur");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnTear(GUIElement &receiver)
    {
        // GTGUI.Server._Elems[element->id]:OnTear()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnTear");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }


    // TODO: Use the proper C++ interface for posting these events. Compiling a script just for this just doesn't sit right with me.
    void GUIScriptServer::PostEvent_OnDrop(GUIElement &receiver, GUIElement &droppedElement)
    {
        // We're cheating here and manually doing this via a text script.
        GT::String scriptString;
        scriptString.AssignFormatted("GTGUI.Server._Elems['%s']:OnDrop({droppedElement = GTGUI.Server._Elems['%s']})", receiver.id, droppedElement.id);

        this->Execute(scriptString.c_str(), 0);
    }

    void GUIScriptServer::PostEvent_OnDragAndDropEnter(GUIElement &receiver, GUIElement &dragAndDropElement)
    {
        // GTGUI.Server._Elems[element->id]:OnDragAndDropEnter(dragAndDropElement.id)

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push(dragAndDropElement.id);
                        this->script->GetTableValue(-3);
                        assert(this->script->IsTable(-1));
                        {
                            this->script->Push("OnDragAndDropEnter");
                            this->script->GetTableValue(-2);
                            assert(this->script->IsFunction(-1));
                            {
                                // Push 'self'.
                                this->script->PushValue(-3);        // <-- 'self'

                                // Push 'data'
                                this->script->PushNewTable();
                                {
                                    this->script->Push("dragAndDropProxyElement");
                                    this->script->PushValue(-5);    // <-- 'dragAndDropElement'
                                    this->script->SetTableValue(-3);
                                }

                                this->script->Call(2, 0);
                            }
                        }
                        this->script->Pop(1);
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnDragAndDropLeave(GUIElement &receiver, GUIElement &dragAndDropElement)
    {
        // GTGUI.Server._Elems[element->id]:OnDragAndDropLeave(dragAndDropElement.id)

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push(dragAndDropElement.id);
                        this->script->GetTableValue(-3);
                        assert(this->script->IsTable(-1));
                        {
                            this->script->Push("OnDragAndDropLeave");
                            this->script->GetTableValue(-2);
                            assert(this->script->IsFunction(-1));
                            {
                                // Push 'self'.
                                this->script->PushValue(-3);                            // <-- 'self'

                                // Push 'data'
                                this->script->PushNewTable();
                                {
                                    this->script->Push("dragAndDropProxyElement");
                                    this->script->PushValue(-5);                        // <-- 'dragAndDropElement'
                                    this->script->SetTableValue(-3);
                                }

                                this->script->Call(2, 0);
                            }
                        }
                        this->script->Pop(1);
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnDragAndDropProxyRemoved(GUIElement &receiver)
    {
        // GTGUI.Server._Elems[element->id]:OnDragAndDropProxyRemoved()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnDragAndDropProxyRemoved");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }



    void GUIScriptServer::PostEvent_OnTextChanged(GUIElement &receiver)
    {
        GT::String scriptString;
        scriptString.AssignFormatted("GTGUI.Server._Elems['%s']:OnTextChanged()", receiver.id);

        this->Execute(scriptString.c_str(), 0);
    }

    void GUIScriptServer::PostEvent_OnInnerXOffsetChanged(GUIElement &receiver)
    {
        GT::String scriptString;
        scriptString.AssignFormatted("GTGUI.Server._Elems['%s']:OnInnerXOffsetChanged()", receiver.id);

        this->Execute(scriptString.c_str(), 0);
    }

    void GUIScriptServer::PostEvent_OnInnerYOffsetChanged(GUIElement &receiver)
    {
        GT::String scriptString;
        scriptString.AssignFormatted("GTGUI.Server._Elems['%s']:OnInnerYOffsetChanged()", receiver.id);

        this->Execute(scriptString.c_str(), 0);
    }


    void GUIScriptServer::PostEvent_OnShow(GUIElement &receiver)
    {
        // GTGUI.Server._Elems[element->id]:OnShow()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnShow");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnHide(GUIElement &receiver)
    {
        // GTGUI.Server._Elems[element->id]:OnHide()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnHide");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }



    void GUIScriptServer::PostEvent_OnShowTooltip(GUIElement &receiver)
    {
        // GTGUI.Server._Elems[element->id]:OnShowTooltip()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnShowTooltip");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    void GUIScriptServer::PostEvent_OnHideTooltip(GUIElement &receiver)
    {
        // GTGUI.Server._Elems[element->id]:OnHideTooltip()

        this->script->GetGlobal("GTGUI");
        assert(this->script->IsTable(-1));
        {
            this->script->Push("Server");
            this->script->GetTableValue(-2);
            assert(this->script->IsTable(-1));
            {
                this->script->Push("_Elems");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->Push(receiver.id);
                    this->script->GetTableValue(-2);
                    assert(this->script->IsTable(-1));
                    {
                        this->script->Push("OnHideTooltip");
                        this->script->GetTableValue(-2);
                        assert(this->script->IsFunction(-1));
                        {
                            // Push 'self'.
                            this->script->PushValue(-2);
                            this->script->Call(1, 0);
                        }
                    }
                    this->script->Pop(1);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        this->script->Pop(1);
    }

    
    
    bool GUIScriptServer::LoadDefaults()
    {
        //bool success = this->script->LoadGTLibrary();
        //bool success = GT::LoadGTCoreScriptLibrary(*this);
        
        // Here we add a pointer to the server.
        this->script->Push(&this->server);
        this->script->SetGlobal("_GTGUI_Server");
        
        
        // GTGUI utilities
        bool success = true;
        success = success && this->Execute
        (
            "GTGUI = {};"
        );

        success = success && this->Execute
        (
            "GTGUI.System = {};"
        );

        success = success && this->Execute
        (
            "GTGUI.Keys = GT.Keys;"     // <-- TODO: Remove this - redundant once we merge GTGUI into GT.
        );
        
        success = success && this->Execute
        (
            "GTGUI.Server        = {};"
            "GTGUI.Server._Elems = {};"

            "GTGUI.Server.MouseButtonDownWatchers        = {};"
            "GTGUI.Server.MouseButtonUpWatchers          = {};"
            "GTGUI.Server.MouseButtonDoubleClickWatchers = {};"
            "GTGUI.Server.MouseMoveWatchers              = {};"
            "GTGUI.Server.MouseWheelWatchers             = {};"
            "GTGUI.Server.KeyPressedWatchers             = {};"
            "GTGUI.Server.KeyDownWatchers                = {};"
            "GTGUI.Server.KeyUpWatchers                  = {};"

            "function GTGUI.Server.RemoveElementFromWatchersByID(elementID)"
            "    if GTGUI.Server.MouseButtonDownWatchers[elementID]        then GTGUI.Server.MouseButtonDownWatchers[elementID]        = nil end;"
            "    if GTGUI.Server.MouseButtonUpWatchers[elementID]          then GTGUI.Server.MouseButtonUpWatchers[elementID]          = nil end;"
            "    if GTGUI.Server.MouseButtonDoubleClickWatchers[elementID] then GTGUI.Server.MouseButtonDoubleClickWatchers[elementID] = nil end;"
            "    if GTGUI.Server.MouseMoveWatchers[elementID]              then GTGUI.Server.MouseMoveWatchers[elementID]              = nil end;"
            "    if GTGUI.Server.MouseWheelWatchers[elementID]             then GTGUI.Server.MouseWheelWatchers[elementID]             = nil end;"
            "    if GTGUI.Server.KeyPressedWatchers[elementID]             then GTGUI.Server.KeyPressedWatchers[elementID]             = nil end;"
            "    if GTGUI.Server.KeyDownWatchers[elementID]                then GTGUI.Server.KeyDownWatchers[elementID]                = nil end;"
            "    if GTGUI.Server.KeyUpWatchers[elementID]                  then GTGUI.Server.KeyUpWatchers[elementID]                  = nil end;"
            "end;"

            "function GTGUI.Server.CallWatchers(eventName, data, watchers)"
            "    local watchersCopy = GT.table.shallowcopy(watchers);"
            "    for key, value in pairs(watchersCopy) do"
            "        value:CallEvent(eventName, data);"
            "    end;"
            "end;"



            "function GTGUI.Server.CreateElement(parent, styleClass)"
            "    local creationString = '<div ';"
            ""
            //   parentid='parent' or 'parent:GetID()'
            "    local parentID = nil;"
            "    if type(parent) == 'string' then"
            "        parentID = parent;"
            "    elseif type(parent) == 'table' and getmetatable(parent) == GTGUI.Element then"
            "        parentID = parent:GetID();"
            "    end;"
            ""
            "    if parentID ~= nil then"
            "        creationString = creationString .. 'parentid=\"' .. parentID .. '\" ';"
            "    end;"
            ""
            ""
            //   styleclass='styleClass'
            "    local styleClassString = '';"
            "    if type(styleClass) == 'string' then"
            "        styleClassString = styleClass;"
            "    elseif type(styleClass) == 'table' then"
            "        for i,value in ipairs(styleClass) do"
            "            styleClassString = styleClassString .. value .. ' ';"
            "        end;"
            "    end;"
            ""
            "    creationString = creationString .. 'styleclass=\"' .. styleClassString .. '\" ';"
            ""
            ""
            //   End the <div> tag.
            "    creationString = creationString .. ' />';"
            ""
            "    return GTGUI.Server.New(creationString);"
            "end;"


            "function GTGUI.Server.DeleteElement(element)"
            "    if element then"
            "        GTGUI.System._DeleteElement(element._ptr);"
            "    end;"
            "end;"


            
            "GTGUI.Server.GetElementByID = function(id)"
            "    if id ~= nil then"
            "        return GTGUI.Server._Elems[id];"
            "    end;"
            ""
            "    return nil;"
            "end;"

            "GTGUI.Server.GetElementByPtr = function(ptr)"
            "    return GTGUI.Server.GetElementByID(GTGUI.System._GetElementID(ptr));"
            "end;"

            "GTGUI.Server.GetRootElement = function()"
            "    return GTGUI.Server.GetElementByID('_Root');"
            "end;"

            // Note that RemoveElementByID() doesn't actually do any deletions. It simply orphans the element and marks everything as nil.
            "GTGUI.Server.RemoveElementByID = function(id)"
            "    local self = GTGUI.Server._Elems[id];"
            "    GTGUI.Server.RemoveElementFromWatchersByID(id);"
            "    if self and self.Parent then"
            "        self.Parent:RemoveChild(self);"
            "    end;"
            "    self._ptr = nil;"
            "    GTGUI.Server._Elems[id] = nil;"
            "end;"


            "GTGUI.Server.RemoveChildByID = function(parentID, childID)"
            "    local parent = GTGUI.Server._Elems[parentID];"
            "    local child  = GTGUI.Server._Elems[childID];"
            "    if parent and child then"
            "        parent.Children[childID] = nil;"
            "        parent.ChildCount        = parent.ChildCount - 1;"
            "        child.Parent             = nil;"
            "    end;"
            "end;"

            "GTGUI.Server.RemoveAllChildrenByID = function(parentID)"
            "    local parent = GTGUI.Server._Elems[parentID];"
            "    if parent then"
            "        for key,value in pairs(parent.Children) do"
            "            value.Parent = nil;"
            "        end;"
            "        parent.Children   = {};"
            "        parent.ChildCount = 0;"
            "    end;"
            "end;"


            "function GTGUI.Server.LoadStyleScriptFromFile(filePath)"
            "    return GTGUI.System._LoadStyleScriptFromFile(filePath);"
            "end;"


            "function GTGUI.Server.OnMouseButtonDown(receiverID, button)"
            "    local receiver = GTGUI.Server.GetElementByID(receiverID);"
            "    local data     = {};"
            "    data.receiver  = receiver;"
            "    data.button    = button;"
            "    if receiver then receiver:OnMouseButtonDown(data) end;"
            "    GTGUI.Server.CallWatchers('WatchMouseButtonDown', data, GTGUI.Server.MouseButtonDownWatchers);"
            "end;"

            "function GTGUI.Server.OnMouseButtonUp(receiverID, button)"
            "    local receiver = GTGUI.Server.GetElementByID(receiverID);"
            "    local data     = {};"
            "    data.receiver  = receiver;"
            "    data.button    = button;"
            "    if receiver then receiver:OnMouseButtonUp(data) end;"
            "    GTGUI.Server.CallWatchers('WatchMouseButtonUp', data, GTGUI.Server.MouseButtonUpWatchers);"
            "end;"

            "function GTGUI.Server.OnMouseButtonDoubleClick(receiverID, button)"
            "    local receiver = GTGUI.Server.GetElementByID(receiverID);"
            "    local data     = {};"
            "    data.receiver  = receiver;"
            "    data.button    = button;"
            "    if receiver then receiver:OnMouseButtonDoubleClick(data) end;"
            "    GTGUI.Server.CallWatchers('WatchMouseButtonDoubleClick', data, GTGUI.Server.MouseButtonDoubleClickWatchers);"
            "end;"


            "function GTGUI.Server.OnMouseMove(receiverID, x, y)"
            "    local receiver = GTGUI.Server.GetElementByID(receiverID);"
            "    local data     = {};"
            "    data.receiver  = receiver;"
            "    data.x         = x;"
            "    data.y         = y;"
            "    if receiver then receiver:OnMouseMove(data) end;"
            "    GTGUI.Server.CallWatchers('WatchMouseMove', data, GTGUI.Server.MouseMoveWatchers);"
            "end;"


            "function GTGUI.Server.OnMouseWheel(receiverID, delta, x, y)"           // <-- 'x' and 'y' is relative to the receiver.
            "    local receiver = GTGUI.Server.GetElementByID(receiverID);"
            "    local data     = {};"
            "    data.receiver  = receiver;"
            "    data.delta     = delta;"
            "    data.x         = x;"
            "    data.y         = y;"
            "    if receiver then receiver:OnMouseWheel(data) end;"
            "    GTGUI.Server.CallWatchers('WatchMouseWheel', data, GTGUI.Server.MouseWheelWatchers);"
            "end;"


            "function GTGUI.Server.OnKeyPressed(receiverID, key)"
            "    local receiver = GTGUI.Server.GetElementByID(receiverID);"
            "    local data     = {};"
            "    data.receiver  = receiver;"
            "    data.key       = key;"
            "    if receiver then receiver:OnKeyPressed(data) end;"
            "    GTGUI.Server.CallWatchers('WatchKeyPressed', data, GTGUI.Server.KeyPressedWatchers);"
            "end;"

            "function GTGUI.Server.OnKeyDown(receiverID, key)"
            "    local result   = true;"
            "    local receiver = GTGUI.Server.GetElementByID(receiverID);"
            "    local data     = {};"
            "    data.receiver  = receiver;"
            "    data.key       = key;"
            "    if receiver then result = receiver:OnKeyDown(data) end;"
            "    GTGUI.Server.CallWatchers('WatchKeyDown', data, GTGUI.Server.KeyDownWatchers);"
            "    return result;"
            "end;"

            "function GTGUI.Server.OnKeyUp(receiverID, key)"
            "    local receiver = GTGUI.Server.GetElementByID(receiverID);"
            "    local data     = {};"
            "    data.receiver  = receiver;"
            "    data.key       = key;"
            "    if receiver then receiver:OnKeyUp(data) end;"
            "    GTGUI.Server.CallWatchers('WatchKeyUp', data, GTGUI.Server.KeyUpWatchers);"
            "end;"


            "function GTGUI.Server.SetDragAndDropProxyElement(dragAndDropProxyElement, offsetX, offsetY)"
            "    GTGUI.System._SetDragAndDropProxyElement(dragAndDropProxyElement._ptr);"
            "    GTGUI.System._SetDragAndDropProxyElementOffset(offsetX, offsetY);"
            "end;"

            "function GTGUI.Server.GetDragAndDropProxyElement()"
            "    return GTGUI.Server.GetElementByID(GTGUI.System._GetDragAndDropProxyElementID());"
            "end;"

            "function GTGUI.Server.RemoveCurrentDragAndDropProxyElement()"
            "    GTGUI.System._RemoveCurrentDragAndDropProxyElement();"
            "end;"


            "function GTGUI.Server.SetDragAndDropProxyElementOffset(offsetX, offsetY)"
            "    GTGUI.System._SetDragAndDropProxyElementOffset(offsetX, offsetY);"
            "end;"



            "function GTGUI.Server.GetViewportWidth()"
            "    return GTGUI.System._GetViewportWidth();"
            "end;"

            "function GTGUI.Server.GetViewportHeight()"
            "    return GTGUI.System._GetViewportHeight();"
            "end;"



            "function GTGUI.Server.GetFocusedElement()"
            "    return GTGUI.Server.GetElementByID(GTGUI.System._GetFocusedElementID());"
            "end;"

            "function GTGUI.Server.FocusElement(element)"
            "    if element then"
            "        if type(element) == 'userdata' then"
            "            GTGUI.System._FocusElement(element);"
            "        end;"
            ""
            "        if type(element) == 'string' then"
            "            element = GTGUI.Server.GetElementByID(element);"
            "        end;"
            ""
            "        if element.isGTLibGUIElement then"
            "            GTGUI.System._FocusElement(element._ptr);"
            "        end;"
            "    end;"
            "end;"



            //////////////////////////////////////////////////////////////
            // GTGUI.Element

            "GTGUI.Element = {};"
            "GTGUI.Element.__index = GTGUI.Element;"
            "GTGUI.Element.isGTLibGUIElement = true;"

            "function GTGUI.Element.Create(id)"
            "    local new = {};"
            "    setmetatable(new, GTGUI.Element);"
            "        new._ptr       = GTGUI.System._CreateNewElement(id);"
            "        new.Parent     = nil;"
            "        new.Children   = {};"
            "        new.ChildCount = 0;"
            "        new.Callbacks  = GT.CallbackManager:Create();"
            ""
            "        GTGUI.Server._Elems[id] = new;"
            "    return new;"
            "end;"

            "function GTGUI.Element:Delete()"
            "    GTGUI.System._DeleteElement(self._ptr);"
            "end;"

            "function GTGUI.Element:GetID()"
            "    return GTGUI.System._GetElementID(self._ptr);"
            "end;"

            "function GTGUI.Element:AppendChild(element)"
            "    if element then"
            "        if element.Parent then"
            "            element.Parent:RemoveChild(element);"
            "        end;"
            "        element.Parent                 = self;"
            "        self.Children[element:GetID()] = element;"
            "        self.ChildCount                = self.ChildCount + 1;"
            "        GTGUI.System._AppendChild(element._ptr, self._ptr);"
            "    end;"
            "    return self;"
            "end;"

            "function GTGUI.Element:PrependChild(element)"
            "    if element then"
            "        if element.Parent then"
            "            element.Parent:RemoveChild(element);"
            "        end;"
            "        element.Parent                 = self;"
            "        self.Children[element:GetID()] = element;"
            "        self.ChildCount                = self.ChildCount + 1;"
            "        GTGUI.System._PrependChild(element._ptr, self._ptr);"
            "    end;"
            "    return self;"
            "end;"

            "function GTGUI.Element:RemoveChild(element)"
            "    if element and element.Parent == self then"
            "        GTGUI.System._RemoveChild(element._ptr, self._ptr);"
            "    end;"
            "    return self;"
            "end;"

            "function GTGUI.Element:RemoveAllChildren(element)"
            "    GTGUI.System._RemoveAllChildren(self._ptr);"
            "end;"

            "function GTGUI.Element:DeleteAllChildren()"
            "    GTGUI.System._DeleteAllChildren(self._ptr);"
            "end;"



            // TODO: Delete these deprecated functions.
#if 0
            "function GTGUI.Element:AttachChild(element)"
            "    GTGUI.Server.g_EngineContext->Logf('GTGUI: AttachChild() is deprecated. Use AppendChild() or PrependChild()');"
            "    return self:AppendChild(element);"
            "end;\n"
            
            "function GTGUI.Element:DetachChild(element)"
            "    GTGUI.Server.g_EngineContext->Logf('GTGUI: DetachChild() is deprecated. Use RemoveChild()');"
            "    return self:RemoveChild(element);"
            "end;"

            "function GTGUI.Element:DetachAllChildren()"
            "    GTGUI.Server.g_EngineContext->Logf('GTGUI: DetachAllChildren() is deprecated. Use RemoveAllChildren()');"
            "    self:RemoveAllChildren();"
            "end;"
#endif

            
            


            "function GTGUI.Element:GetChildCount()"
            "    return self.ChildCount;"
            "end;"
            
            "function GTGUI.Element:GetChildByIndex(index)"
            "    return self.Children[index];"
            "end;"

            "function GTGUI.Element:IsAncestor(ancestor)"
            "    if ancestor ~= nil then"
            "        return GTGUI.System._IsAncestor(self._ptr, ancestor._ptr);"
            "    else"
            "        return false;"
            "    end;"
            "end;"

            "function GTGUI.Element:IsChild(child)"
            "    if child ~= nil then"
            "        return GTGUI.System._IsChild(self._ptr, child._ptr);"
            "    else"
            "        return false;"
            "    end;"
            "end;"

            "function GTGUI.Element:AttachStyleClass(styleClassName, noRefresh)"
            "    GTGUI.System._AttachStyleClass(self._ptr, styleClassName, noRefresh);"
            "end;"
            
            "function GTGUI.Element:DetachStyleClass(styleClassName, noRefresh)"
            "    GTGUI.System._DetachStyleClass(self._ptr, styleClassName, noRefresh);"
            "end;"
            
            "function GTGUI.Element:OnMouseEnter(callback)"
            "    if callback then"
            "        return self:BindEvent('OnMouseEnter', callback);"
            "    else"
            "        self:CallEvent('OnMouseEnter');"
            "    end;"
            "end;\n"
            
            "function GTGUI.Element:OnMouseLeave(callback)"
            "    if callback then"
            "        return self:BindEvent('OnMouseLeave', callback);"
            "    else"
            "        self:CallEvent('OnMouseLeave');"
            "    end;"
            "end;"

            "function GTGUI.Element:OnPush(callback)"
            "    if callback then"
            "        return self:BindEvent('OnPush', callback);"
            "    else"
            "        self:CallEvent('OnPush');"
            "    end;"
            "end;"
            
            "function GTGUI.Element:OnRelease(callback)"
            "    if callback then"
            "        return self:BindEvent('OnRelease', callback);"
            "    else"
            "        self:CallEvent('OnRelease');"
            "    end;"
            "end;"

            "function GTGUI.Element:OnPressed(callback)"
            "    if callback then"
            "        return self:BindEvent('OnPressed', callback);"
            "    else"
            "        self:CallEvent('OnPressed');"
            "    end;"
            "end;"


            "function GTGUI.Element:OnMouseButtonDown(arg1)"
            "    if arg1 and type(arg1) == 'function' then"   // <-- Is arg1 a callback? If so, we're binding.
            "        return self:BindEvent('OnMouseButtonDown', arg1);"
            "    else"
            "        self:CallEvent('OnMouseButtonDown', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:WatchMouseButtonDown(callback)"
            "    if callback then"
            "        self:BindEvent('WatchMouseButtonDown', callback);"
            "        GTGUI.Server.MouseButtonDownWatchers[self:GetID()] = self;"
            "        return self;"
            "    end;"
            "end;"


            "function GTGUI.Element:OnMouseButtonUp(arg1)"
            "    if arg1 and type(arg1) == 'function' then"   // <-- Is arg1 a callback? If so, we're binding.
            "        return self:BindEvent('OnMouseButtonUp', arg1);"
            "    else"
            "        self:CallEvent('OnMouseButtonUp', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:WatchMouseButtonUp(callback)"
            "    if callback then"
            "        self:BindEvent('WatchMouseButtonUp', callback);"
            "        GTGUI.Server.MouseButtonUpWatchers[self:GetID()] = self;"
            "        return self;"
            "    end;"
            "end;\n"


            "function GTGUI.Element:OnMouseButtonDoubleClick(arg1)"
            "    if arg1 and type(arg1) == 'function' then"   // <-- Is arg1 a callback? If so, we're binding.
            "        return self:BindEvent('OnMouseButtonDoubleClick', arg1);"
            "    else"
            "        self:CallEvent('OnMouseButtonDoubleClick', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:WatchMouseButtonDoubleClick(callback)"
            "    if callback then"
            "        self:BindEvent('WatchMouseButtonDoubleClick', callback);"
            "        GTGUI.Server.MouseButtonDoubleClickWatchers[self:GetID()] = self;"
            "        return self;"
            "    end;"
            "end;"


            "function GTGUI.Element:OnMouseMove(arg1)"
            "    if arg1 and type(arg1) == 'function' then"   // <-- Is arg1 a callback? If so, we're binding.
            "        return self:BindEvent('OnMouseMove', arg1);"
            "    else"
            "        self:CallEvent('OnMouseMove', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:WatchMouseMove(callback)"
            "    if callback then"
            "        self:BindEvent('WatchMouseMove', callback);"
            "        GTGUI.Server.MouseMoveWatchers[self:GetID()] = self;"
            "        return self;"
            "    end;"
            "end;"


            "function GTGUI.Element:OnMouseWheel(arg1)"
            "    if arg1 and type(arg1) == 'function' then"   // <-- Is arg1 a callback? If so, we're binding.
            "        return self:BindEvent('OnMouseWheel', arg1);"
            "    else"
            "        self:CallEvent('OnMouseWheel', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:WatchMouseWheel(callback)"
            "    if callback then"
            "        self:BindEvent('WatchMouseWheel', callback);"
            "        GTGUI.Server.MouseWheelWatchers[self:GetID()] = self;"
            "        return self;"
            "    end;"
            "end;"


            "function GTGUI.Element:OnSize(callback)"
            "    if callback then"
            "        return self:BindEvent('OnSize', callback);"
            "    else"
            "        self:CallEvent('OnSize', { width = self:GetWidth(); height = self:GetHeight(); });"
            "    end;"
            "end;"

            "function GTGUI.Element:OnMove(callback)"
            "    if callback then"
            "        return self:BindEvent('OnMove', callback);"
            "    else"
            "        self:CallEvent('OnMove', { x = self:GetXPosition(); y = self:GetYPosition(); });"
            "    end;"
            "end;"


            "function GTGUI.Element:OnKeyPressed(arg1)"
            "    if arg1 and type(arg1) == 'function' then"    // <-- Is arg1 a callback? If so, we're binding.
            "        return self:BindEvent('OnKeyPressed', arg1);"
            "    else"
            "        return self:CallEvent('OnKeyPressed', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:OnKeyDown(arg1)"
            "    if arg1 and type(arg1) == 'function' then"    // <-- Is arg1 a callback? If so, we're binding.
            "        return self:BindEvent('OnKeyDown', arg1);"
            "    else"
            "        return self:CallEvent('OnKeyDown', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:OnKeyUp(arg1)"
            "    if arg1 and type(arg1) == 'function' then"    // <-- Is arg1 a callback? If so, we're binding.
            "        return self:BindEvent('OnKeyUp', arg1);"
            "    else"
            "        return self:CallEvent('OnKeyUp', arg1);"
            "    end;"
            "end;"


            "function GTGUI.Element:WatchKeyPressed(callback)"
            "    if callback then"
            "        self:BindEvent('WatchKeyPressed', callback);"
            "        GTGUI.Server.KeyPressedWatchers[self:GetID()] = self;"
            "        return self;"
            "    end;"
            "end;\n"

            "function GTGUI.Element:WatchKeyDown(callback)"
            "    if callback then"
            "        self:BindEvent('WatchKeyDown', callback);"
            "        GTGUI.Server.KeyDownWatchers[self:GetID()] = self;"
            "        return self;"
            "    end;"
            "end;\n"

            "function GTGUI.Element:WatchKeyUp(callback)"
            "    if callback then"
            "        self:BindEvent('WatchKeyUp', callback);"
            "        GTGUI.Server.KeyUpWatchers[self:GetID()] = self;"
            "        return self;"
            "    end;"
            "end;\n"


            "function GTGUI.Element:OnFocus(callback)"
            "    if callback then"
            "        return self:BindEvent('OnFocus', callback);"
            "    else"
            "        self:CallEvent('OnFocus');"
            "    end;"
            "end;"

            "function GTGUI.Element:OnBlur(callback)"
            "    if callback then"
            "        return self:BindEvent('OnBlur', callback);"
            "    else"
            "        self:CallEvent('OnBlur');"
            "    end;"
            "end;"


            "function GTGUI.Element:OnTear(callback)"
            "    if callback then"
            "        return self:BindEvent('OnTear', callback);"
            "    else"
            "        self:CallEvent('OnTear');"
            "    end;"
            "end;"

            "function GTGUI.Element:OnDrop(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnDrop', arg1);"
            "    else"
            "        self:CallEvent('OnDrop', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:OnDragAndDropEnter(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnDragAndDropEnter', arg1);"
            "    else"
            "        self:CallEvent('OnDragAndDropEnter', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:OnDragAndDropLeave(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnDragAndDropLeave', arg1);"
            "    else"
            "        self:CallEvent('OnDragAndDropLeave', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:OnDragAndDropProxyRemoved(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnDragAndDropProxyRemoved', arg1);"
            "    else"
            "        self:CallEvent('OnDragAndDropProxyRemoved', arg1);"
            "    end;"
            "end;"



            "function GTGUI.Element:OnTextChanged(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnTextChanged', arg1);"
            "    else"
            "        self:CallEvent('OnTextChanged', arg1);"
            "    end;"
            "end;"


            "function GTGUI.Element:OnInnerXOffsetChanged(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnInnerXOffsetChanged', arg1);"
            "    else"
            "        self:CallEvent('OnInnerXOffsetChanged', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:OnInnerYOffsetChanged(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnInnerYOffsetChanged', arg1);"
            "    else"
            "        self:CallEvent('OnInnerYOffsetChanged', arg1);"
            "    end;"
            "end;"


            "function GTGUI.Element:OnShow(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnShow', arg1);"
            "    else"
            "        self:CallEvent('OnShow', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:OnHide(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnHide', arg1);"
            "    else"
            "        self:CallEvent('OnHide', arg1);"
            "    end;"
            "end;"


            "function GTGUI.Element:OnShowTooltip(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnShowTooltip', arg1);"
            "    else"
            "        self:CallEvent('OnShowTooltip', arg1);"
            "    end;"
            "end;"

            "function GTGUI.Element:OnHideTooltip(arg1)"
            "    if arg1 and type(arg1) == 'function' then"
            "        return self:BindEvent('OnHideTooltip', arg1);"
            "    else"
            "        self:CallEvent('OnHideTooltip', arg1);"
            "    end;"
            "end;"


            "function GTGUI.Element:BindEvent(name, callback)"
            "    self.Callbacks:Bind(name, callback);"
            "    return self;"
            "end;"

            "function GTGUI.Element:UnbindEvent(name, callback)"
            "    if callback then"
            "        self.Callbacks:Unbind(name, callback);"
            "    else"
            "        self.Callbacks:UnbindAll(name);"
            "    end;"
            ""
            "    return self;"
            "end;"
            
            "function GTGUI.Element:CallEvent(name, data)"
            "    return self.Callbacks:Call(name, data);"
            "end;"

            
            "function GTGUI.Element:GetStyle(styleName)"
            "    return GTGUI.System._GetStyle(self._ptr, styleName);"
            "end;"

            "function GTGUI.Element:SetStyle(styleName, value)"
            "    GTGUI.System._SetStyle(self._ptr, styleName, value);"
            "end;"


            //////////////////////////////////////////////
            // Text

            "function GTGUI.Element:GetText()"
            "    return GTGUI.System._GetText(self._ptr);"
            "end;"

            "function GTGUI.Element:SetText(text)"
            "    GTGUI.System._SetText(self._ptr, text);"
            "end;"

            "function GTGUI.Element:GetTextWidth()"
            "    return GTGUI.System._GetTextWidth(self._ptr);"
            "end;"

            "function GTGUI.Element:GetTextHeight()"
            "    return GTGUI.System._GetTextHeight(self._ptr);"
            "end;"

            "function GTGUI.Element:GetTextLineHeight()"
            "    return GTGUI.System._GetTextLineHeight(self._ptr);"
            "end;"

            "function GTGUI.Element:GetTextLineCount()"
            "    return GTGUI.System._GetTextLineCount(self._ptr);"
            "end;"

            "function GTGUI.Element:GetTextPageLineCount()"
            "    return GTGUI.System._GetTextPageLineCount(self._ptr);"
            "end;"

            "function GTGUI.Element:SelectAllText()"
            "    GTGUI.System._SelectAllText(self._ptr);"
            "end;"

            "function GTGUI.Element:GoToLine(lineNumber)"
            "    GTGUI.System._GoToLine(self._ptr, lineNumber);"
            "end;"



            "function GTGUI.Element:GetWidth()"
            "    return GTGUI.System._GetWidth(self._ptr);"
            "end;"

            "function GTGUI.Element:SetWidth(widthInPixels)"
            "    return self:SetStyle('width', tostring(widthInPixels) .. 'px');"
            "end;"

            "function GTGUI.Element:GetHeight()"
            "    return GTGUI.System._GetHeight(self._ptr);"
            "end;"

            "function GTGUI.Element:SetHeight(heightInPixels)"
            "    return self:SetStyle('height', tostring(heightInPixels) .. 'px');"
            "end;"


            "function GTGUI.Element:GetInnerWidth()"
            "    return GTGUI.System._GetInnerWidth(self._ptr);"
            "end;"

            "function GTGUI.Element:GetInnerHeight()"
            "    return GTGUI.System._GetInnerHeight(self._ptr);"
            "end;"


            "function GTGUI.Element:GetXPosition()"
            "    return GTGUI.System._GetXPosition(self._ptr);"
            "end;"

            "function GTGUI.Element:GetYPosition()"
            "    return GTGUI.System._GetYPosition(self._ptr);"
            "end;"

            "function GTGUI.Element:GetInnerXPosition()"
            "    return GTGUI.System._GetInnerXPosition(self._ptr);"
            "end;"

            "function GTGUI.Element:GetInnerYPosition()"
            "    return GTGUI.System._GetInnerYPosition(self._ptr);"
            "end;"


            "function GTGUI.Element:GetInnerXOffset()"
            "    return GTGUI.System._GetInnerXOffset(self._ptr);"
            "end;"

            "function GTGUI.Element:GetInnerYOffset()"
            "    return GTGUI.System._GetInnerYOffset(self._ptr);"
            "end;"


            "function GTGUI.Element:GetAbsolutePosition()"
            "    return GTGUI.System._GetAbsolutePosition(self._ptr);"
            "end;"



            "function GTGUI.Element:GetLeftPadding()"
            "    return GTGUI.System._GetLeftPadding(self._ptr);"
            "end;"

            "function GTGUI.Element:GetRightPadding()"
            "    return GTGUI.System._GetRightPadding(self._ptr);"
            "end;"

            "function GTGUI.Element:GetTopPadding()"
            "    return GTGUI.System._GetTopPadding(self._ptr);"
            "end;"

            "function GTGUI.Element:GetBottomPadding()"
            "    return GTGUI.System._GetBottomPadding(self._ptr);"
            "end;"

            "function GTGUI.Element:GetHorizontalPadding()"
            "    return GTGUI.System._GetHorizontalPadding(self._ptr);"
            "end;"

            "function GTGUI.Element:GetVerticalPadding()"
            "    return GTGUI.System._GetVerticalPadding(self._ptr);"
            "end;"


            "function GTGUI.Element:SetPosition(x, y)"
            "    self:SetStyle('left', x);"
            "    self:SetStyle('top',  y);"
            "end;"



            "function GTGUI.Element:LockStyleRefreshing()"
            "    return GTGUI.System._LockStyleRefreshing(self._ptr);"
            "end;"

            "function GTGUI.Element:UnlockStyleRefreshing()"
            "    return GTGUI.System._UnlockStyleRefreshing(self._ptr);"
            "end;"

            "function GTGUI.Element:RefreshStyle()"
            "    return GTGUI.System._RefreshStyle(self._ptr);"
            "end;"


            
            "function GTGUI.Element:Show()"
            "      GTGUI.System._Show(self._ptr);"
            "end;"
            
            "function GTGUI.Element:Hide()"
            "      GTGUI.System._Hide(self._ptr);"
            "end;"

            "function GTGUI.Element:IsVisible()"
            "   return GTGUI.System._IsVisible(self._ptr);"
            "end;"


            "function GTGUI.Element:Enable()"
            "    GTGUI.System._SetStyle(self._ptr, 'enabled', 'true');"
            "end;"
            
            "function GTGUI.Element:Disable()"
            "    GTGUI.System._SetStyle(self._ptr, 'enabled', 'false');"
            "end;"

            "function GTGUI.Element:IsEnabled()"
            "    return GTGUI.System._GetStyle(self._ptr, 'enabled') == 'true';"
            "end;"


            "function GTGUI.Element:GetNextSibling()"
            "    local siblingID = GTGUI.System._GetNextSiblingID(self._ptr);"
            "    if siblingID ~= nil then"
            "        return GTGUI.Server._Elems[siblingID];"
            "    end;"
            "    return nil;"
            "end;"

            "function GTGUI.Element:GetPrevSibling()"
            "    local siblingID = GTGUI.System._GetPrevSiblingID(self._ptr);"
            "    if siblingID ~= nil then"
            "        return GTGUI.Server._Elems[siblingID];"
            "    end;"
            "    return nil;"
            "end;"

            
            "function GTGUI.Element:Focus()"
            "    GTGUI.System._FocusElement(self._ptr);"
            "end;"


            "function GTGUI.Element:BringToTop()"
            "    GTGUI.System._BringToTop(self._ptr);"
            "end;"

            "function GTGUI.Element:PushToBack()"
            "    GTGUI.System.PushToBack(self._ptr);"
            "end;"
        );
        
        if (success)
        {
            // Now we need to add a few C functions.
            // System
            this->script->GetGlobal("GTGUI");
            assert(this->script->IsTable(-1));
            {
                this->script->Push("System");
                this->script->GetTableValue(-2);
                assert(this->script->IsTable(-1));
                {
                    this->script->SetTableFunction(-1, "_CreateNewElement",        FFI::_CreateNewElement);
                    this->script->SetTableFunction(-1, "_DeleteElement",           FFI::_DeleteElement);
                    
                    this->script->SetTableFunction(-1, "_GetElementID",            FFI::_GetElementID);

                    this->script->SetTableFunction(-1, "_AppendChild",             FFI::_AppendChild);
                    this->script->SetTableFunction(-1, "_PrependChild",            FFI::_PrependChild);
                    this->script->SetTableFunction(-1, "_RemoveChild",             FFI::_RemoveChild);
                    this->script->SetTableFunction(-1, "_RemoveAllChildren",       FFI::_RemoveAllChildren);
                    this->script->SetTableFunction(-1, "_DeleteAllChildren",       FFI::_DeleteAllChildren);

                    this->script->SetTableFunction(-1, "_IsAncestor",              FFI::_IsAncestor);
                    this->script->SetTableFunction(-1, "_IsChild",                 FFI::_IsChild);

                    this->script->SetTableFunction(-1, "_AttachStyleClass",        FFI::_AttachStyleClass);
                    this->script->SetTableFunction(-1, "_DetachStyleClass",        FFI::_DetachStyleClass);

                    this->script->SetTableFunction(-1, "_GetStyle",                FFI::_GetStyle);
                    this->script->SetTableFunction(-1, "_SetStyle",                FFI::_SetStyle);

                    this->script->SetTableFunction(-1, "_LoadStyleScriptFromFile", FFI::_LoadStyleScriptFromFile);


                    this->script->SetTableFunction(-1, "_GetText",                 FFI::_GetText);
                    this->script->SetTableFunction(-1, "_SetText",                 FFI::_SetText);
                    this->script->SetTableFunction(-1, "_GetTextWidth",            FFI::_GetTextWidth);
                    this->script->SetTableFunction(-1, "_GetTextHeight",           FFI::_GetTextHeight);
                    this->script->SetTableFunction(-1, "_GetTextLineHeight",       FFI::_GetTextLineHeight);
                    this->script->SetTableFunction(-1, "_GetTextLineCount",        FFI::_GetTextLineCount);
                    this->script->SetTableFunction(-1, "_GetTextPageLineCount",    FFI::_GetTextPageLineCount);
                    this->script->SetTableFunction(-1, "_SelectAllText",           FFI::_SelectAllText);
                    this->script->SetTableFunction(-1, "_GoToLine",                FFI::_GoToLine);

                    this->script->SetTableFunction(-1, "_GetWidth",                FFI::_GetWidth);
                    this->script->SetTableFunction(-1, "_GetHeight",               FFI::_GetHeight);
                    this->script->SetTableFunction(-1, "_GetInnerWidth",           FFI::_GetInnerWidth);
                    this->script->SetTableFunction(-1, "_GetInnerHeight",          FFI::_GetInnerHeight);

                    this->script->SetTableFunction(-1, "_GetXPosition",            FFI::_GetXPosition);
                    this->script->SetTableFunction(-1, "_GetYPosition",            FFI::_GetYPosition);
                    this->script->SetTableFunction(-1, "_GetInnerXPosition",       FFI::_GetInnerXPosition);
                    this->script->SetTableFunction(-1, "_GetInnerYPosition",       FFI::_GetInnerYPosition);
                    this->script->SetTableFunction(-1, "_GetInnerXOffset",         FFI::_GetInnerXOffset);
                    this->script->SetTableFunction(-1, "_GetInnerYOffset",         FFI::_GetInnerYOffset);
                    this->script->SetTableFunction(-1, "_GetAbsolutePosition",     FFI::_GetAbsolutePosition);
                    
                    this->script->SetTableFunction(-1, "_GetLeftPadding",          FFI::_GetLeftPadding);
                    this->script->SetTableFunction(-1, "_GetRightPadding",         FFI::_GetRightPadding);
                    this->script->SetTableFunction(-1, "_GetTopPadding",           FFI::_GetTopPadding);
                    this->script->SetTableFunction(-1, "_GetBottomPadding",        FFI::_GetBottomPadding);
                    this->script->SetTableFunction(-1, "_GetHorizontalPadding",    FFI::_GetHorizontalPadding);
                    this->script->SetTableFunction(-1, "_GetVerticalPadding",      FFI::_GetVerticalPadding);

                    this->script->SetTableFunction(-1, "_Show",                    FFI::_Show);
                    this->script->SetTableFunction(-1, "_Hide",                    FFI::_Hide);
                    
                    this->script->SetTableFunction(-1, "_IsVisible",               FFI::_IsVisible);

                    this->script->SetTableFunction(-1, "_GetNextSiblingID",        FFI::_GetNextSiblingID);
                    this->script->SetTableFunction(-1, "_GetPrevSiblingID",        FFI::_GetPrevSiblingID);

                    this->script->SetTableFunction(-1, "_SetDragAndDropProxyElement",           FFI::_SetDragAndDropProxyElement);
                    this->script->SetTableFunction(-1, "_GetDragAndDropProxyElementID",         FFI::_GetDragAndDropProxyElementID);
                    this->script->SetTableFunction(-1, "_RemoveCurrentDragAndDropProxyElement", FFI::_RemoveCurrentDragAndDropProxyElement);
                    this->script->SetTableFunction(-1, "_SetDragAndDropProxyElementOffset",     FFI::_SetDragAndDropProxyElementOffset);
                    
                    this->script->SetTableFunction(-1, "_FocusElement",            FFI::_FocusElement);
                    this->script->SetTableFunction(-1, "_GetFocusedElementID",     FFI::_GetFocusedElementID);

                    this->script->SetTableFunction(-1, "_BringToTop",              FFI::_BringToTop);
                    this->script->SetTableFunction(-1, "_PushToBack",              FFI::_PushToBack);

                    this->script->SetTableFunction(-1, "_GetViewportWidth",        FFI::_GetViewportWidth);
                    this->script->SetTableFunction(-1, "_GetViewportHeight",       FFI::_GetViewportHeight);

                    
                    this->script->SetTableFunction(-1, "_LockStyleRefreshing",     FFI::_LockStyleRefreshing);
                    this->script->SetTableFunction(-1, "_UnlockStyleRefreshing",   FFI::_UnlockStyleRefreshing);
                    this->script->SetTableFunction(-1, "_RefreshStyle",            FFI::_RefreshStyle);
                }
                this->script->Pop(1);

                // Server
                this->script->Push("Server");
                this->script->GetTableValue(-2);
                if (this->script->IsTable(-1))
                {
                    this->script->SetTableFunction(-1, "Log",                                FFI::Log);
                    this->script->SetTableFunction(-1, "New",                                FFI::New);
                    this->script->SetTableFunction(-1, "BlurFocusedElement",                 FFI::BlurFocusedElement);
                    this->script->SetTableFunction(-1, "IsCTRLKeyDown",                      FFI::IsCTRLKeyDown);
                    this->script->SetTableFunction(-1, "IsShiftKeyDown",                     FFI::IsShiftKeyDown);
                    this->script->SetTableFunction(-1, "GetMousePosition",                   FFI::GetMousePosition);
                    
                    this->script->SetTableFunction(-1, "IsLMBDown",                          FFI::IsLMBDown);
                    this->script->SetTableFunction(-1, "IsRMBDown",                          FFI::IsRMBDown);
                    this->script->SetTableFunction(-1, "IsMMBDown",                          FFI::IsMMBDown);

                    this->script->SetTableFunction(-1, "DoesFocusedElementHaveEditableText", FFI::DoesFocusedElementHaveEditableText);
                }
                this->script->Pop(1);
            }
            this->script->Pop(1);
        }
        
        return success;
    }
    
    
    GUIServer & GUIScriptServer::FFI::GetServer(GT::Script &script)
    {
        script.GetGlobal("_GTGUI_Server");
        auto server = static_cast<GUIServer*>(script.ToPointer(-1));
        script.Pop(1);

        assert(server != nullptr);

        return *server;
    }



    int GUIScriptServer::FFI::Log(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);

        if (script.IsString(1))
        {
            server.PostError(script.ToString(1));
        }
        
        return 0;
    }

    int GUIScriptServer::FFI::_CreateNewElement(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        
        // First parameter should be the ID.
        if (script.IsString(1))
        {
            auto newElement = server.CreateNewElement(script.ToString(1));
            script.Push((void *)newElement);
            
            return 1;
        }
        
        return 0;
    }
    
    int GUIScriptServer::FFI::_DeleteElement(GT::Script &script)
    {
        auto elementToDelete = static_cast<GUIElement*>(script.ToPointer(1));
        if (elementToDelete)
        {
            elementToDelete->server.DeleteElement(elementToDelete);
        }
        
        return 0;
    }

    int GUIScriptServer::FFI::_DeleteAllChildren(GT::Script &script)
    {
        auto parent = static_cast<GUIElement*>(script.ToPointer(1));
        if (parent != nullptr)
        {
            parent->DeleteAllChildren();
        }

        return 0;
    }
    
    int GUIScriptServer::FFI::_GetElementID(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->id);
            return 1;
        }

        return 0;
    }
    
    int GUIScriptServer::FFI::New(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);

        if (script.IsString(1))
        {
            // We need to create the element via the main server and then push the ID of the top element.
            GT::String topID;
            if (server.Load(script.ToString(1), topID))
            {
                // Note how we do not pop anything here. Lua will handle that for us when it returns from this call.
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
                            script.Push(topID.c_str());
                            script.GetTableValue(-2);
                        }
                    }
                }
            }
            else
            {
                script.Error();
                script.PushNil();
            }
        }
        else
        {
            server.PostError("Script Error: GTGUI.Server.New() called with invalid arguments. Should take a string of XML as it's only argument.");
            script.Error();
            script.PushNil();
        }

        return 1;
    }

    int GUIScriptServer::FFI::BlurFocusedElement(GT::Script &script)
    {
        FFI::GetServer(script).BlurFocusedElement();
        return 0;
    }

    int GUIScriptServer::FFI::IsCTRLKeyDown(GT::Script &script)
    {
        script.Push(FFI::GetServer(script).IsCTRLKeyDown());
        return 1;
    }

    int GUIScriptServer::FFI::IsShiftKeyDown(GT::Script &script)
    {
        script.Push(FFI::GetServer(script).IsShiftKeyDown());
        return 1;
    }

    int GUIScriptServer::FFI::GetMousePosition(GT::Script &script)
    {
        int xPos;
        int yPos;
        FFI::GetServer(script).GetMousePosition(xPos, yPos);

        script.Push(xPos);
        script.Push(yPos);

        return 2;
    }


    int GUIScriptServer::FFI::IsLMBDown(GT::Script &script)
    {
        script.Push(FFI::GetServer(script).IsLMBDown());
        return 1;
    }

    int GUIScriptServer::FFI::IsRMBDown(GT::Script &script)
    {
        script.Push(FFI::GetServer(script).IsRMBDown());
        return 1;
    }

    int GUIScriptServer::FFI::IsMMBDown(GT::Script &script)
    {
        script.Push(FFI::GetServer(script).IsMMBDown());
        return 1;
    }


    int GUIScriptServer::FFI::DoesFocusedElementHaveEditableText(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);

        if (server.GetFocusedElement() != nullptr)
        {
            script.Push(server.GetFocusedElement()->HasEditableText());
        }
        else
        {
            script.Push(false);
        }

        return 1;
    }
    


    int GUIScriptServer::FFI::_AppendChild(GT::Script &script)
    {
        auto child  = static_cast<GUIElement*>(script.ToPointer(1));
        auto parent = static_cast<GUIElement*>(script.ToPointer(2));
        if (child != nullptr && parent != nullptr)
        {
            parent->AppendChild(*child);
        }

        return 0;
    }

    int GUIScriptServer::FFI::_PrependChild(GT::Script &script)
    {
        auto child  = static_cast<GUIElement*>(script.ToPointer(1));
        auto parent = static_cast<GUIElement*>(script.ToPointer(2));
        if (child != nullptr && parent != nullptr)
        {
            parent->PrependChild(*child);
        }

        return 0;
    }
    
    int GUIScriptServer::FFI::_RemoveChild(GT::Script &script)
    {
        auto child  = static_cast<GUIElement*>(script.ToPointer(1));
        auto parent = static_cast<GUIElement*>(script.ToPointer(2));
        if (child != nullptr && parent != nullptr)
        {
            parent->RemoveChild(*child);
        }
        
        return 0;
    }

    int GUIScriptServer::FFI::_RemoveAllChildren(GT::Script &script)
    {
        auto parent = static_cast<GUIElement*>(script.ToPointer(1));
        if (parent != nullptr)
        {
            parent->RemoveAllChildren();
        }

        return 0;
    }


    int GUIScriptServer::FFI::_IsAncestor(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        auto other   = static_cast<GUIElement*>(script.ToPointer(2));
        if (element != nullptr && other != nullptr)
        {
            script.Push(element->IsAncestor(*other));
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_IsChild(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        auto other   = static_cast<GUIElement*>(script.ToPointer(2));
        if (element != nullptr && other != nullptr)
        {
            script.Push(element->IsChild(*other));
            return 1;
        }

        return 0;
    }
    
    int GUIScriptServer::FFI::_AttachStyleClass(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        
        auto element        = static_cast<GUIElement*>(script.ToPointer(1));
        auto styleClassName = script.ToString(2);
        if (element != nullptr && styleClassName != nullptr)
        {
            auto styleClass = server.GetStyleServer().GetStyleClass(styleClassName);
            if (styleClass != nullptr)
            {
                element->AttachStyleClass(*styleClass, !script.ToBoolean(3));
            }
        }

        return 0;
    }
    
    int GUIScriptServer::FFI::_DetachStyleClass(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        
        auto element        = static_cast<GUIElement*>(script.ToPointer(1));
        auto styleClassName = script.ToString(2);
        if (element != nullptr && styleClassName != nullptr)
        {
            auto styleClass = server.GetStyleServer().GetStyleClass(styleClassName);
            if (styleClass != nullptr)
            {
                element->DetachStyleClass(*styleClass, !script.ToBoolean(3));
            }
        }

        return 0;
    }

    
    int GUIScriptServer::FFI::_GetStyle(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            auto style = script.ToString(2);

            // The style is set on the elements ID style.
            auto styleClass = element->GetPrimaryStyleClass();
            if (styleClass != nullptr)
            {
                script.Push(styleClass->GetAttribute(style).c_str());
                return 1;
            }
        }

        return 0;
    }
    
    int GUIScriptServer::FFI::_SetStyle(GT::Script &script)
    {
        auto element  = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            auto style = script.ToString(2);
            auto value = script.ToString(3);
            element->SetGUIStyleAttribute(style, value);
        }

        return 0;
    }


    int GUIScriptServer::FFI::_LoadStyleScriptFromFile(GT::Script &script)
    {
        script.Push(FFI::GetServer(script).GetStyleServer().LoadFromFile(script.ToString(1)));
        return 1;
    }


    ///////////////////////////////////////////////////////////////
    // Text

    int GUIScriptServer::FFI::_GetText(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            auto text = element->GetText();
            if (text != nullptr)
            {
                script.Push(text);
            }
            else
            {
                script.Push("");
            }
        }
        else
        {
            script.Push("");
        }

        return 1;
    }
    
    int GUIScriptServer::FFI::_SetText(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            auto text       = script.ToString(2);
            auto blockEvent = true;

            element->SetText(text, blockEvent);
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetTextWidth(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(static_cast<int>(element->textManager.GetTextWidth()));
        }
        else
        {
            script.Push(0);
        }

        return 1;
    }

    int GUIScriptServer::FFI::_GetTextHeight(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(static_cast<int>(element->textManager.GetTextHeight()));
        }
        else
        {
            script.Push(0);
        }

        return 1;
    }

    int GUIScriptServer::FFI::_GetTextLineHeight(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            auto font = element->textManager.GetDefaultFont();
            if (font != nullptr)
            {
                script.Push(static_cast<int>(font->GetLineHeight()));
            }
            else
            {
                script.Push(0);
            }
        }
        else
        {
            script.Push(0);
        }

        return 1;
    }

    int GUIScriptServer::FFI::_GetTextLineCount(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(static_cast<int>(element->textManager.GetLineCount()));
        }
        else
        {
            script.Push(0);
        }

        return 1;
    }

    int GUIScriptServer::FFI::_GetTextPageLineCount(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(static_cast<int>(element->textManager.GetPageLineCount()));
        }
        else
        {
            script.Push(0);
        }

        return 1;
    }

    int GUIScriptServer::FFI::_SelectAllText(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->SelectAllText();
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GoToLine(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->GoToLine(static_cast<unsigned int>(script.ToInteger(2)));
        }

        return 0;
    }





    int GUIScriptServer::FFI::_GetWidth(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetWidth());
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetHeight(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetHeight());
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetInnerWidth(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetInnerWidth());
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetInnerHeight(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetInnerHeight());
            return 1;
        }

        return 0;
    }


    int GUIScriptServer::FFI::_GetXPosition(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->x);
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetYPosition(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->y);
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetInnerXPosition(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            if (element->parent != nullptr)
            {
                script.Push(element->x - element->parent->GetLeftPadding());
            }
            else
            {
                script.Push(element->x);
            }

            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetInnerYPosition(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            if (element->parent != nullptr)
            {
                script.Push(element->y - element->parent->GetTopPadding());
            }
            else
            {
                script.Push(element->y);
            }
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetInnerXOffset(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(static_cast<int>(element->GetInnerXOffset()));
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetInnerYOffset(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(static_cast<int>(element->GetInnerYOffset()));
            return 1;
        }

        return 0;
    }


    int GUIScriptServer::FFI::_GetAbsolutePosition(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            GT::Rect<int> rect;
            element->GetAbsoluteRect(rect);

            script.Push(rect.left);
            script.Push(rect.top);

            return 2;
        }

        return 0;
    }



    int GUIScriptServer::FFI::_GetLeftPadding(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetLeftPadding());
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetRightPadding(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetRightPadding());
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetTopPadding(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetTopPadding());
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetBottomPadding(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetBottomPadding());
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetHorizontalPadding(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetHorizontalPadding());
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetVerticalPadding(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->GetVerticalPadding());
            return 1;
        }

        return 0;
    }


    int GUIScriptServer::FFI::_Show(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->Show();
        }

        return 0;
    }

    int GUIScriptServer::FFI::_Hide(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->Hide();
        }

        return 0;
    }

    int GUIScriptServer::FFI::_IsVisible(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            script.Push(element->IsVisible());
            return 1;
        }

        return 0;
    }

    int GUIScriptServer::FFI::_LockStyleRefreshing(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->LockStyleRefreshing();
        }

        return 0;
    }

    int GUIScriptServer::FFI::_UnlockStyleRefreshing(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->UnlockStyleRefreshing();
        }

        return 0;
    }

    int GUIScriptServer::FFI::_RefreshStyle(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->RefreshStyle();
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetNextSiblingID(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            if (element->nextSibling != nullptr)
            {
                script.Push(element->nextSibling->id);
                return 1;
            }
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetPrevSiblingID(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            if (element->prevSibling != nullptr)
            {
                script.Push(element->prevSibling->id);
                return 1;
            }
        }

        return 0;
    }


    int GUIScriptServer::FFI::_SetDragAndDropProxyElement(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        {
            auto element = static_cast<GUIElement*>(script.ToPointer(1));
            if (element != nullptr)
            {
                server.SetDragAndDropProxyElement(*element);
            }
            else
            {
                server.RemoveCurrentDragAndDropProxyElement();
            }

            return 0;
        }
    }

    int GUIScriptServer::FFI::_GetDragAndDropProxyElementID(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        {
            auto element = server.GetDragAndDropProxyElement();
            if (element != nullptr)
            {
                script.Push(element->id);
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }
    }

    int GUIScriptServer::FFI::_RemoveCurrentDragAndDropProxyElement(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        {
            server.RemoveCurrentDragAndDropProxyElement();

            return 0;
        }
    }

    int GUIScriptServer::FFI::_SetDragAndDropProxyElementOffset(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        {
            server.SetDragAndDropProxyElementOffset(script.ToInteger(1), script.ToInteger(2));
            return 0;
        }
    }


    int GUIScriptServer::FFI::_FocusElement(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->server.FocusElement(element);
        }

        return 0;
    }

    int GUIScriptServer::FFI::_GetFocusedElementID(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        {
            auto focusedElement = server.GetFocusedElement();
            if (focusedElement != nullptr)
            {
                script.Push(focusedElement->GetID());
                return 1;
            }
        }

        return 0;
    }


    int GUIScriptServer::FFI::_BringToTop(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->server.BringToTop(*element);
        }

        return 0;
    }

    int GUIScriptServer::FFI::_PushToBack(GT::Script &script)
    {
        auto element = static_cast<GUIElement*>(script.ToPointer(1));
        if (element != nullptr)
        {
            element->server.PushToBack(*element);
        }

        return 0;
    }


    int GUIScriptServer::FFI::_GetViewportWidth(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        {
            unsigned int width;
            unsigned int height;
            server.GetViewportSize(width, height);

            script.Push(static_cast<int>(width));
        }

        return 1;
    }

    int GUIScriptServer::FFI::_GetViewportHeight(GT::Script &script)
    {
        auto &server = FFI::GetServer(script);
        {
            unsigned int width;
            unsigned int height;
            server.GetViewportSize(width, height);

            script.Push(static_cast<int>(height));
        }

        return 1;
    }
}
