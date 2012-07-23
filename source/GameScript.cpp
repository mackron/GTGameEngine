
#include <GTEngine/GameScript.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Audio.hpp>

namespace GTEngine
{
    GameScript::GameScript(Game &game)
        : game(game), lastError()
    {
        // The first thing we want to do is load some defaults.
        this->Execute
        (
            "Game   = {};"
            "Engine = {};"
            "Editor = {};"
            "Editor.ModelEditor    = {};"
            "Editor.MaterialEditor = {};"
            "Editor.SceneEditor    = {};"
            "Editor.Sandbox        = {};"
            ""
            "Display ="
            "{"
            "    Width      = 1280;"
            "    Height     = 720;"
            "    FOV        = 90;"
            "    Fullscreen = false;"
            ""
            "    Textures ="
            "    {"
            "        Anisotropy = 16;"
            "    };"
            "};"
        );

        // Here is where we register the foreign function interface.
        this->RegisterFFI();
    }

    GameScript::~GameScript()
    {
    }

    bool GameScript::Load(const char* script)
    {
        if (!GTCore::Script::Load(script))
        {
            this->lastError = this->ToString(-1);
            return false;
        }

        return true;
    }

    bool GameScript::Execute()
    {
        if (!GTCore::Script::Execute())
        {
            this->lastError = this->ToString(-1);
            PostError("Script Error: %s", this->lastError.c_str());

            return false;
        }

        return true;
    }
}

///////////////////////////////////////////////////////////
// FFI

namespace GTEngine
{
    Game & GameScript::FFI::GetGameObject(GTCore::Script &script)
    {
        script.GetGlobal("__GamePtr");
        auto game = static_cast<Game*>(script.ToPointer(-1));
        script.Pop(1);

        assert(game != nullptr);

        return *game;
    }


    ////////////////////////////////////////////////////////////////
    // Game FFI

    int FFI_Game_Close(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.Close();
        return 0;
    }

    int FFI_Game_Pause(GTCore::Script &script)
    {
        auto& game = GameScript::FFI::GetGameObject(script);

        game.Pause();
        return 0;
    }

    int FFI_Game_Resume(GTCore::Script &script)
    {
        auto& game = GameScript::FFI::GetGameObject(script);

        game.Resume();
        return 0;
    }

    int FFI_Game_EnableFullscreen(GTCore::Script &script)
    {
        auto& game = GameScript::FFI::GetGameObject(script);

        game.EnableFullscreen();
        return 0;
    }

    int FFI_Game_DisableFullscreen(GTCore::Script &script)
    {
        auto& game = GameScript::FFI::GetGameObject(script);

        game.DisableFullscreen();
        return 0;
    }





    ////////////////////////////////////////////////////////////////
    // Engine FFI

    int FFI_Engine_ExecuteScript(GTCore::Script &script)
    {
        auto& game = GameScript::FFI::GetGameObject(script);

        script.Push(game.ExecuteScript(script.ToString(1)));
        return 1;
    }

    int FFI_Engine_GetLastScriptError(GTCore::Script &script)
    {
        auto& game = GameScript::FFI::GetGameObject(script);

        script.Push(game.GetScript().GetLastError());
        return 1;
    }

    int FFI_Engine_ShowDebug(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.ShowDebugging();
        return 0;
    }

    int FFI_Engine_HideDebug(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.HideDebugging();
        return 0;
    }


    int FFI_Engine_EnableVSync(GTCore::Script &)
    {
        Renderer::SetSwapInterval(1);
        return 0;
    }

    int FFI_Engine_DisableVSync(GTCore::Script &)
    {
        Renderer::SetSwapInterval(0);
        return 0;
    }


    int FFI_Engine_PlaySound(GTCore::Script &script)
    {
        GTEngine::AudioComposer::Play(script.ToString(1));
        return 0;
    }


    ////////////////////////////////////////////////////////////////
    // Editor FFI

    int FFI_Editor_Open(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.OpenEditor();
        return 0;
    }

    int FFI_Editor_Close(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.CloseEditor();
        return 0;
    }

    int FFI_Editor_SwitchToModelEditorMode(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().SwitchToModelEditorMode();
        return 0;
    }

    int FFI_Editor_SwitchToSandboxMode(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().SwitchToSandboxMode();
        return 0;
    }

    ////////////////////////////////////////////////////////////////
    // Editor.ModelEditor FFI

    int FFI_Editor_ModelEditor_Load(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().LoadModel(script.ToString(1));
        return 0;
    }

    int FFI_Editor_ModelEditor_Save(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().SaveModel(script.ToString(1));
        return 0;
    }


    int FFI_Editor_ModelEditor_SetMaterial(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().SetMaterial(script.ToInteger(1), script.ToString(2));
        return 0;
    }


    int FFI_Editor_ModelEditor_PlayAnimation(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().PlayAnimation();
        return 0;
    }

    int FFI_Editor_ModelEditor_StopAnimation(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().StopAnimation();
        return 0;
    }


    int FFI_Editor_ModelEditor_ResetCamera(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().ResetCamera();
        return 0;
    }


    int FFI_Editor_ModelEditor_ShowConvexDecomposition(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().ShowConvexDecomposition();
        return 0;
    }

    int FFI_Editor_ModelEditor_HideConvexDecomposition(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().HideConvexDecomposition();
        return 0;
    }

    int FFI_Editor_ModelEditor_BuildConvexDecomposition(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        ConvexHullBuildSettings settings;

        game.GetEditor().GetModelEditor().BuildConvexDecomposition(settings);
        return 0;
    }


    //////////////////////////////////////////////////////
    // RegisterFFI()

    bool GameScript::RegisterFFI()
    {
        // We assert that the tables have already been created.

        this->Push(&this->game);
        this->SetGlobal("__GamePtr");

        this->GetGlobal("Game");
        {
            this->Push("Close");
            this->PushClosure(FFI_Game_Close, 0);
            this->SetTableValue(-3);

            this->Push("Pause");
            this->PushClosure(FFI_Game_Pause, 0);
            this->SetTableValue(-3);

            this->Push("Resume");
            this->PushClosure(FFI_Game_Resume, 0);
            this->SetTableValue(-3);


            this->Push("EnableFullscreen");
            this->PushClosure(FFI_Game_EnableFullscreen, 0);
            this->SetTableValue(-3);

            this->Push("DisableFullscreen");
            this->PushClosure(FFI_Game_DisableFullscreen, 0);
            this->SetTableValue(-3);
        }
        this->Pop(1);    // Game


        this->GetGlobal("Engine");
        {
            this->Push("ExecuteScript");
            this->PushClosure(FFI_Engine_ExecuteScript, 0);
            this->SetTableValue(-3);

            this->Push("GetLastScriptError");
            this->PushClosure(FFI_Engine_GetLastScriptError, 0);
            this->SetTableValue(-3);


            this->Push("ShowDebug");
            this->PushClosure(FFI_Engine_ShowDebug, 0);
            this->SetTableValue(-3);

            this->Push("HideDebug");
            this->PushClosure(FFI_Engine_HideDebug, 0);
            this->SetTableValue(-3);


            this->Push("EnableVSync");
            this->PushClosure(FFI_Engine_EnableVSync, 0);
            this->SetTableValue(-3);

            this->Push("DisableVSync");
            this->PushClosure(FFI_Engine_DisableVSync, 0);
            this->SetTableValue(-3);


            this->Push("PlaySound");
            this->PushClosure(FFI_Engine_PlaySound, 0);
            this->SetTableValue(-3);
        }
        this->Pop(1);    // Engine


        this->GetGlobal("Editor");
        {
            this->Push("Open");
            this->PushClosure(FFI_Editor_Open, 0);
            this->SetTableValue(-3);

            this->Push("Close");
            this->PushClosure(FFI_Editor_Close, 0);
            this->SetTableValue(-3);

            this->Push("SwitchToModelEditorMode");
            this->PushClosure(FFI_Editor_SwitchToModelEditorMode, 0);
            this->SetTableValue(-3);

            this->Push("SwitchToSandboxMode");
            this->PushClosure(FFI_Editor_SwitchToSandboxMode, 0);
            this->SetTableValue(-3);


            this->Push("ModelEditor");
            this->GetTableValue(-2);
            if (this->IsTable(-1))
            {
                this->Push("Load");
                this->PushClosure(FFI_Editor_ModelEditor_Load, 0);
                this->SetTableValue(-3);

                this->Push("Save");
                this->PushClosure(FFI_Editor_ModelEditor_Save, 0);
                this->SetTableValue(-3);


                this->Push("SetMaterial");
                this->PushClosure(FFI_Editor_ModelEditor_SetMaterial, 0);
                this->SetTableValue(-3);


                this->Push("PlayAnimation");
                this->PushClosure(FFI_Editor_ModelEditor_PlayAnimation, 0);
                this->SetTableValue(-3);

                this->Push("StopAnimation");
                this->PushClosure(FFI_Editor_ModelEditor_StopAnimation, 0);
                this->SetTableValue(-3);


                this->Push("ResetCamera");
                this->PushClosure(FFI_Editor_ModelEditor_ResetCamera, 0);
                this->SetTableValue(-3);


                this->Push("ShowConvexDecomposition");
                this->PushClosure(FFI_Editor_ModelEditor_ShowConvexDecomposition, 0);
                this->SetTableValue(-3);

                this->Push("HideConvexDecomposition");
                this->PushClosure(FFI_Editor_ModelEditor_HideConvexDecomposition, 0);
                this->SetTableValue(-3);

                this->Push("BuildConvexDecomposition");
                this->PushClosure(FFI_Editor_ModelEditor_BuildConvexDecomposition, 0);
                this->SetTableValue(-3);
            }
            this->Pop(1);


            this->Push("MaterialEditor");
            this->GetTableValue(-2);
            if (this->IsTable(-1))
            {
            }
            this->Pop(1);


            this->Push("SceneEditor");
            this->GetTableValue(-2);
            if (this->IsTable(-1))
            {
            }
            this->Pop(1);


            this->Push("Sandbox");
            this->GetTableValue(-2);
            if (this->IsTable(-1))
            {
            }
            this->Pop(1);
        }
        this->Pop(1);    // Editor

        return true;
    }
}
