
#include <GTEngine/Editor/ModelEditor.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Math.hpp>

#include <glm/gtx/string_cast.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    ModelEditor::ModelEditor(Game &game, Editor &editor)
        : game(game), editor(editor), GUI(), GUIEventHandlers(*this), scene(), viewport(0, 0), renderer(), cameraNode(), modelNode(),
          cameraXRotation(0.0f), cameraYRotation(0.0f)
    {
        cameraNode.Add3DCameraComponent(90.0f, static_cast<float>(16.0f) / static_cast<float>(9.0f), 0.1f, 1000.0f);
        cameraNode.AddDirectionalLightComponent(0.25f, 0.25f, 0.25f);
        cameraNode.AddAmbientLightComponent(0.25f, 0.25f, 0.25f);
        cameraNode.MoveForward(-10.0f);

        auto modelComponent = modelNode.AddModelComponent(ModelLibrary::LoadFromFile("engine/models/animation-test.dae"));
        modelComponent->GetModel()->meshes[0]->SetMaterial(GTEngine::MaterialLibrary::Create("engine/materials/floor.material"));

        // Here we setup the viewport.
        this->renderer.SetClearColour(0.1f, 0.1f, 0.1f);
        this->renderer.EnableColourClears();

        this->viewport.SetRenderer(&this->renderer);
        this->viewport.SetCameraNode(&this->cameraNode);

        this->scene.AddViewport(this->viewport);

        this->scene.AddSceneNode(this->cameraNode);
        //this->scene.AddSceneNode(this->modelNode);

        


        // TESTING.
        //
        // Here we are going to build a chain of scene nodes representing the bones.
        //
        // We'll build these in two passes. The first pass will construct a scene node for each bone, naming them appropriately. The second pass will
        // link the nodes together to form the hierarchy.
        auto &bones = modelComponent->GetModel()->bones;
        for (size_t i = 0; i < bones.count; ++i)
        {
            auto bone = bones.buffer[i]->value;
            assert(bone != nullptr);

            auto boneSceneNode = new SceneNode;
            boneSceneNode->SetName(bone->GetName());


            glm::mat4 transform = bone->GetTransform();

            boneSceneNode->SetPosition(glm::vec3(transform[3]));
            boneSceneNode->SetOrientation(glm::quat(transform));
            
            boneSceneNode->SetDataPointer(0, bone);

            auto model = boneSceneNode->AddModelComponent(ModelLibrary::LoadFromFile("engine/models/default.dae"))->GetModel();
            model->meshes[0]->SetMaterial(GTEngine::MaterialLibrary::Create("engine/materials/simple-diffuse.material"));
            
            this->boneSceneNodes.Add(bone->GetName(), boneSceneNode);
            this->scene.AddSceneNode(*boneSceneNode);

            //printf("Testing: %s\n", glm::to_string(boneSceneNode->GetWorldTransformMatrix()).c_str());
            //printf("Testing: %s\n", glm::to_string(bone->GetOffsetMatrix()).c_str());
            //printf("Testing: %s. %f %f %f.\n", boneSceneNode->GetName(), boneSceneNode->GetPosition().x, boneSceneNode->GetPosition().y, boneSceneNode->GetPosition().z);
        }

        for (size_t i = 0; i < bones.count; ++i)
        {
            auto bone = bones.buffer[i]->value;
            assert(bone != nullptr);

            if (bone->GetParent() != nullptr)
            {
                auto boneSceneNode = this->boneSceneNodes.Find(bone->GetName())->value;
                assert(boneSceneNode != nullptr);

                auto parentSceneNode = this->boneSceneNodes.Find(bone->GetParent()->GetName())->value;
                assert(parentSceneNode != nullptr);

                parentSceneNode->AttachChild(*boneSceneNode);
            }
        }

        modelComponent->GetModel()->PlayAnimation("", true);
    }

    ModelEditor::~ModelEditor()
    {
    }

    bool ModelEditor::Startup(GTGUI::Server &guiServer)
    {
        this->GUI.ModelViewport = guiServer.GetElementByID("ModelViewport");
        if (this->GUI.ModelViewport != nullptr)
        {
            this->GUI.ModelViewport->AttachEventHandler(this->GUIEventHandlers.ModelViewport);

            // Here we need to setup the model editor's FFI. We assert that the Game.Editor namespace has already been created.
            auto &script = guiServer.GetScriptServer().GetScript();

            script.Get("Game.Editor");
            if (script.IsTable(-1))
            {
            }
            script.Pop(1);  // Game
        }
        else
        {
            return false;
        }

        return true;
    }

    void ModelEditor::Update(double deltaTimeInSeconds)
    {
        if (this->GUI.ModelViewport->IsVisible())
        {
            // TESTING
            //
            // We will grab the model and step it's animation.
            {
                auto model = this->modelNode.GetComponent<ModelComponent>()->GetModel();
                assert(model != nullptr);

                model->StepAnimation(deltaTimeInSeconds);

                // With the animation stepped, we're going to need to update the scene nodes.
                for (size_t i = 0; i < this->boneSceneNodes.count; ++i)
                {
                    auto boneSceneNode = this->boneSceneNodes.buffer[i]->value;
                    auto bone = boneSceneNode->GetDataPointer<Bone>(0);

                    glm::mat4 transform = bone->GetTransform();

                    boneSceneNode->SetPosition(glm::vec3(transform[3]));
                    boneSceneNode->SetOrientation(glm::quat(transform));
                }
            }


            // If the mouse is captured we may need to move the screen around.
            if (this->game.IsMouseCaptured())
            {
                const float moveSpeed   = 3.0f;
                const float rotateSpeed = 14.0f;

                float mouseOffsetX, mouseOffsetY;
                this->game.GetSmoothedMouseOffset(mouseOffsetX, mouseOffsetY);

                mouseOffsetX *= static_cast<float>(deltaTimeInSeconds);
                mouseOffsetY *= static_cast<float>(deltaTimeInSeconds);

                if (this->game.IsMouseButtonDown(GTCore::MouseButton_Left))
                {
                    if (this->game.IsMouseButtonDown(GTCore::MouseButton_Right))
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
                    if (this->game.IsMouseButtonDown(GTCore::MouseButton_Right))
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

    void ModelEditor::SwapRCQueues()
    {
        this->renderer.OnSwapRCQueues();
    }
}

// FFI.
namespace GTEngine
{
}

namespace GTEngine
{
    ModelViewportEventHandler::ModelViewportEventHandler(ModelEditor &modelEditor)
        : modelEditor(modelEditor)
    {
    }

    void ModelViewportEventHandler::OnSize(GTGUI::Element &element)
    {
        int newWidth  = GTCore::Max(element.GetInnerWidth(), 1);
        int newHeight = GTCore::Max(element.GetInnerHeight(), 1);

        this->modelEditor.viewport.Resize(newWidth, newHeight);

        auto camera = this->modelEditor.cameraNode.GetComponent<GTEngine::CameraComponent>();
        camera->Set3DProjection(90.0f, static_cast<float>(newWidth) / static_cast<float>(newHeight), 0.1f, 1000.0f);
    }

    void ModelViewportEventHandler::OnDraw(GTGUI::Element &element)
    {
        GTEngine::Renderer::SetShader(GTEngine::ShaderLibrary::GetGUIQuadShader());
        GTEngine::Renderer::SetShaderParameter("Texture", this->modelEditor.viewport.GetColourOutputBuffer());
        GTEngine::Renderer::SetShaderParameter("Color",   1.0f, 1.0f, 1.0f, 1.0f);

        GTGUI::Rect viewportRect;
        element.GetAbsoluteRect(viewportRect);

        float quadLeft   = static_cast<float>(viewportRect.left);
        float quadRight  = static_cast<float>(viewportRect.right);
        float quadTop    = static_cast<float>(viewportRect.top);
        float quadBottom = static_cast<float>(viewportRect.bottom);

        // All we do is draw a quad over the viewport area.
        float quadVertices[] =
        {
            quadLeft,  quadBottom,
            0.0f,      0.0f,

            quadRight, quadBottom,
            1.0f,      0.0f,

            quadRight, quadTop,
            1.0f,      1.0f,

            quadLeft,  quadTop,
            0.0f,      1.0f
        };

        unsigned int quadIndices[] =
        {
            0, 1, 2,
            2, 3, 0,
        };

        GTEngine::Renderer::Draw(quadVertices, quadIndices, 6, GTEngine::VertexFormat::P2T2);
    }

    void ModelViewportEventHandler::OnLMBDown(GTGUI::Element &, int, int)
    {
        this->modelEditor.game.CaptureMouse();
    }

    void ModelViewportEventHandler::OnLMBUp(GTGUI::Element &, int, int)
    {
        if (!this->modelEditor.game.IsMouseButtonDown(GTCore::MouseButton_Right))
        {
            this->modelEditor.game.ReleaseMouse();
        }
    }

    void ModelViewportEventHandler::OnRMBDown(GTGUI::Element &, int, int)
    {
        this->modelEditor.game.CaptureMouse();
    }

    void ModelViewportEventHandler::OnRMBUp(GTGUI::Element &, int, int)
    {
        if (!this->modelEditor.game.IsMouseButtonDown(GTCore::MouseButton_Left))
        {
            this->modelEditor.game.ReleaseMouse();
        }
    }

    void ModelViewportEventHandler::OnMouseMove(GTGUI::Element &, int, int)
    {
        //printf("Mouse Moved: %d %d\n", x, y);
    }

    void ModelViewportEventHandler::OnMouseWheel(GTGUI::Element &, int delta, int x, int y)
    {
        printf("Mouse Wheel: %d %d %d\n", delta, x, y);
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
