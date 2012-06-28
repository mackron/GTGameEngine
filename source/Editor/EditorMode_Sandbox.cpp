
#include <GTEngine/Editor/EditorMode_Sandbox.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Math.hpp>

namespace GTEngine
{
    EditorMode_Sandbox::EditorMode_Sandbox(Editor &editor)
        : EditorMode(editor),
          editor(editor), GUI(), scene(), viewport(0, 0), renderer(),
          cameraNode(),
          cameraXRotation(0.0f), cameraYRotation(0.0f),
          viewportEventHandler(editor.GetGame(), viewport)
    {
        cameraNode.Add3DCameraComponent(90.0f, static_cast<float>(16.0f) / static_cast<float>(9.0f), 0.1f, 1000.0f);
        cameraNode.AddDirectionalLightComponent(0.5f, 0.5f, 0.5f);
        cameraNode.AddAmbientLightComponent(0.25f, 0.25f, 0.25f);
        cameraNode.MoveForward(-10.0f);


        this->GenerateFloor();
        this->cameraNode.SetPosition(0.0f, 250.0f, 0.0f);
        this->cameraNode.SetOrientation(glm::angleAxis(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
        this->cameraXRotation = -90.0f;



        // Here we setup the viewport.
        this->renderer.SetClearColour(0.1f, 0.1f, 0.1f);
        this->renderer.EnableColourClears();

        this->viewport.SetRenderer(&this->renderer);
        this->viewport.SetCameraNode(&this->cameraNode);

        this->scene.AddViewport(this->viewport);

        this->scene.AddSceneNode(this->cameraNode);
    }

    EditorMode_Sandbox::~EditorMode_Sandbox()
    {
    }

    bool EditorMode_Sandbox::Startup(GTGUI::Server &guiServer)
    {
        this->GUI.Main    = guiServer.GetElementByID("Sandbox");
        this->GUI.MenuBar = guiServer.GetElementByID("MenuBar_Sandbox");

        this->GUI.Viewport = guiServer.GetElementByID("SandboxViewport");
        if (this->GUI.Viewport != nullptr)
        {
            this->GUI.Viewport->AttachEventHandler(this->viewportEventHandler);
        }
        else
        {
            return false;
        }

        return true;
    }

    void EditorMode_Sandbox::OnActivate()
    {
        if (this->GUI.Main != nullptr)
        {
            this->GUI.Main->Show();
        }

        if (this->GUI.MenuBar != nullptr)
        {
            this->GUI.MenuBar->Show();
        }
    }

    void EditorMode_Sandbox::OnDeactivate()
    {
        if (this->GUI.Main != nullptr)
        {
            this->GUI.Main->Hide();
        }

        if (this->GUI.MenuBar != nullptr)
        {
            this->GUI.MenuBar->Hide();
        }
    }

    void EditorMode_Sandbox::OnUpdate(double deltaTimeInSeconds)
    {
        if (this->GUI.Viewport != nullptr && this->GUI.Viewport->IsVisible())
        {
            auto &game = this->GetGame();       // <-- For ease of use.

            // If the mouse is captured we may need to move the screen around.
            if (game.IsMouseCaptured())
            {
                const float moveSpeed   = 0.05f;
                const float rotateSpeed = 0.1f;

                float mouseOffsetX;
                float mouseOffsetY;
                game.GetSmoothedMouseOffset(mouseOffsetX, mouseOffsetY);

                if (game.IsMouseButtonDown(GTCore::MouseButton_Left))
                {
                    if (game.IsMouseButtonDown(GTCore::MouseButton_Right))
                    {
                        this->cameraNode.MoveUp(-mouseOffsetY * moveSpeed);
                        this->cameraNode.MoveRight(mouseOffsetX * moveSpeed);
                    }
                    else
                    {
                        this->cameraNode.MoveForward(-mouseOffsetY * moveSpeed);
                        this->cameraYRotation += -mouseOffsetX * rotateSpeed;
                    }
                }
                else
                {
                    if (game.IsMouseButtonDown(GTCore::MouseButton_Right))
                    {
                        this->cameraXRotation += -mouseOffsetY * rotateSpeed;
                        this->cameraYRotation += -mouseOffsetX * rotateSpeed;
                    }
                }

                // Doing the rotation this way allows us to keep the up axis constant.
                this->cameraNode.SetOrientation(glm::quat());
                this->cameraNode.RotateY(this->cameraYRotation);
                this->cameraNode.RotateX(this->cameraXRotation);
            }

            this->scene.Update(deltaTimeInSeconds);
        }
    }

    void EditorMode_Sandbox::OnSwapRCQueues()
    {
        this->renderer.OnSwapRCQueues();
    }
}



#include <GTEngine/Random/Floor/FloorGenerator.hpp>

namespace GTEngine
{
    void EditorMode_Sandbox::GenerateFloor()
    {
        FloorProperties properties;
        properties.width  = 10;
        properties.height = 10;
        properties.minRoomHeight = 2;
        properties.minRoomWidth  = 2;
        properties.maxRoomWidth  = 4;
        properties.maxRoomHeight = 4;
        properties.sideRoomCount = 2;
        properties.minSideRoomWidth  = 1;
        properties.minSideRoomHeight = 1;
        properties.maxSideRoomWidth  = 1;
        properties.maxSideRoomHeight = 1;

        FloorGenerator generator;
        auto definition = generator.GenerateFloorDefinition(properties);


        // We need to go through and build geometry from every tile.
        GTCore::Vector<FloorTile*> tiles;
        definition->GetQuadtree().QueryAll(tiles);


        for (size_t i = 0; i < tiles.count; ++i)
        {
            auto sceneNode = new SceneNode;
            sceneNode->SetPosition(tiles[i]->GetXPosition() * 8.0f, 0.0f, tiles[i]->GetYPosition() * 8.0f);

            auto model = sceneNode->AddModelComponent(GTEngine::ModelLibrary::LoadFromFile("models/floor01.dae"))->GetModel();
            model->meshes[0]->SetMaterial("materials/ceiling01.material");
            model->meshes[1]->SetMaterial("materials/floor01.material");

            this->scene.AddSceneNode(*sceneNode);
        }



        generator.DeleteFloorDefinition(definition);
    }
}

