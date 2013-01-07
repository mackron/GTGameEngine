
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>


namespace GTEngine
{
    Editor_ModelEditor::Editor_ModelEditor(Editor &editor)
        : editor(editor),
          scene(), viewport(), camera(),
          modelNode(), convexHullParentNode(), convexHullNodes(),
          GUI(), viewportEventHandler(editor.GetGame(), viewport),
          cameraXRotation(), cameraYRotation(),
          currentState(nullptr), loadedStates(),
          random()
    {
        this->viewport.SetCameraNode(this->camera);
        this->scene.AddViewport(this->viewport);
        this->scene.GetRenderer().EnableBackgroundColourClearing(0.5f, 0.5f, 0.5f);


        // We need to ensure the model node has a model component.
        this->modelNode.AddComponent<GTEngine::ModelComponent>();


        this->ResetCamera();
    }

    Editor_ModelEditor::~Editor_ModelEditor()
    {
        // All of our state objects need to be deleted.
        for (size_t i = 0; i < this->loadedStates.count; ++i)
        {
            delete this->loadedStates.buffer[i]->value;
        }

        this->DeleteConvexHulls();

        ModelLibrary::Delete(this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel());
    }

    void Editor_ModelEditor::Startup()
    {
        auto &gui = this->editor.GetGame().GetGUI();

        this->GUI.Viewport = gui.GetElementByID("Editor_ModelEditor_Viewport");
        this->GUI.Viewport->AttachEventHandler(this->viewportEventHandler);

        // We use the camera for our lights.
        this->camera.AddComponent<GTEngine::CameraComponent>();
        this->camera.AddDirectionalLightComponent(0.25f, 0.25f, 0.25f);
        this->camera.AddAmbientLightComponent(0.6f, 0.6f, 0.6f);


        // Now we add the scene nodes to the scene.
        this->scene.AddSceneNode(this->camera);
        this->scene.AddSceneNode(this->modelNode);


        // Convex hull.
        this->convexHullParentNode.Hide();
        this->scene.AddSceneNode(this->convexHullParentNode);
    }

    bool Editor_ModelEditor::LoadModel(const char* fileName, const char* makeRelativeTo)
    {
        auto newModel = ModelLibrary::LoadFromFile(fileName, makeRelativeTo);
        if (newModel != nullptr)
        {
            // The old model needs to be deleted.
            auto oldModel = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
            if (oldModel != nullptr)
            {
                this->DeleteConvexHulls();
                ModelLibrary::Delete(oldModel);
            }


            // Now we need to set the new model, remembering to refresh the object.
            this->modelNode.GetComponent<GTEngine::ModelComponent>()->SetModel(newModel);
            this->modelNode.Refresh();


            // Here we need to make sure the state is saved.
            this->SaveState();



            // Now we need to setup some state for the new model. If the model has already had it's state recorded, we can just restore it from the map.
            auto iState = this->loadedStates.Find(fileName);
            if (iState != nullptr)
            {
                this->currentState = iState->value;
            }
            else
            {
                this->currentState = new State;
                this->loadedStates.Add(fileName, this->currentState);

                // Here we will initialise the state based on the model's definition.
                auto &modelDefinition = newModel->GetDefinition();

                // Materials.
                for (size_t iMaterial = 0; iMaterial < modelDefinition.meshMaterials.count; ++iMaterial)
                {
                    this->currentState->materials.PushBack(modelDefinition.meshMaterials[iMaterial]->GetDefinition().fileName.c_str());
                }

                // Convex decomposition.
                this->currentState->convexDecompositionSettings = modelDefinition.convexHullBuildSettings;

                // TODO: Do some kind of automatic positioning based on the size of the bounding box.
                this->currentState->cameraPosition.z = 10.0f;
            }

            // Here we take 'currentState' and restore everything from that.
            this->RestoreState();

            return true;
        }

        return false;
    }

    bool Editor_ModelEditor::SaveModel(const char* fileName)
    {
        return ModelLibrary::WriteToFile(fileName);
    }

    bool Editor_ModelEditor::SetMaterial(int index, const char* fileName)
    {
        auto model = this->GetCurrentModel();
        if (model != nullptr)
        {
            if (model->meshes[index]->SetMaterial(fileName))
            {
                // This is painful, but we're going to cheat here and do a const_cast so we can modify the model's definition to hold the new base material.
                auto &definition = const_cast<ModelDefinition &>(model->GetDefinition());

                MaterialLibrary::Delete(definition.meshMaterials[index]);
                definition.meshMaterials[index] = MaterialLibrary::CreateCopy(*model->meshes[index]->GetMaterial());

                return true;
            }
        }

        return false;
    }


    void Editor_ModelEditor::ResetCamera()
    {
        this->camera.SetPosition(0.0f, 0.0f, 10.0f);
        this->SetCameraRotation(0.0f, 0.0f);
    }

    void Editor_ModelEditor::SetCameraRotation(float xRotation, float yRotation)
    {
        this->cameraXRotation = xRotation;
        this->cameraYRotation = yRotation;
        this->ApplyCameraRotation();
    }


    void Editor_ModelEditor::ShowConvexDecomposition()
    {
        // If we have content in our convex hull scene node list, it means we don't have to create anything.
        if (this->convexHullNodes.count == 0)
        {
            auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
            if (model != nullptr)
            {
                auto &convexHulls = model->GetConvexHulls();
                for (size_t i = 0; i < convexHulls.count; ++i)
                {
                    auto hull = convexHulls[i];
                    if (hull != nullptr)
                    {
                        auto hullModel = ModelLibrary::CreateFromConvexHull(*hull);
                        hullModel->meshes[0]->GetMaterial()->SetParameter("DiffuseColour", random.Next<float>(0.0f, 1.0f), random.Next<float>(0.0f, 1.0f), random.Next<float>(0.0f, 1.0f));

                        auto node = new SceneNode;
                        node->AddModelComponent(hullModel);

                        this->convexHullNodes.PushBack(node);

                        this->convexHullParentNode.AttachChild(*node);
                    }
                }
            }
        }

        // Now we just show the main scene node.
        this->convexHullParentNode.Show();
        this->modelNode.Hide();
    }

    void Editor_ModelEditor::HideConvexDecomposition()
    {
        this->convexHullParentNode.Hide();
        this->modelNode.Show();
    }

    void Editor_ModelEditor::BuildConvexDecomposition(ConvexHullBuildSettings &settings)
    {
        // The old convex hulls need to be deleted.
        this->DeleteConvexHulls();

        // We build the convex decomposition on the definition directly.
        auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
        if (model != nullptr)
        {
            // We're going to be naughty here and do a const_cast so we can build the convex hulls.
            auto &definition = const_cast<ModelDefinition &>(model->GetDefinition());
            definition.BuildConvexDecomposition(settings);
        }



        if (this->convexHullParentNode.IsVisible())
        {
            this->ShowConvexDecomposition();
        }
    }


    void Editor_ModelEditor::PlayAnimation()
    {
        auto modelComponent = this->modelNode.GetComponent<GTEngine::ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                AnimationSequence sequence;
                sequence.AddFrame(0, model->animation.GetKeyFrameCount(), 0.0f, true);      // '0.0f' is the transition time. 'true' says to loop.

                model->PlayAnimation(sequence);
            }
        }
    }

    void Editor_ModelEditor::StopAnimation()
    {
        auto modelComponent = this->modelNode.GetComponent<GTEngine::ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                AnimationSequence sequence;
                sequence.AddFrame(0, model->animation.GetKeyFrameCount(), 0.0f, true);      // '0.0f' is the transition time. 'true' says to loop.

                model->StopAnimation();
            }
        }
    }


    //////////////////////////////////
    // Events

    void Editor_ModelEditor::Update(double deltaTimeInSeconds)
    {
        if (this->GUI.Viewport != nullptr && this->GUI.Viewport->IsVisible())
        {
            auto &game = this->editor.GetGame();       // <-- For ease of use.

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
                        this->camera.MoveUp(-mouseOffsetY * moveSpeed);
                        this->camera.MoveRight(mouseOffsetX * moveSpeed);
                    }
                    else
                    {
                        this->camera.MoveForward(-mouseOffsetY * moveSpeed);
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

                this->ApplyCameraRotation();
            }

            this->scene.Update(deltaTimeInSeconds);
        }
    }



    void Editor_ModelEditor::OnModelDefinitionChanged(const char* absolutePath)
    {
        auto iState = this->loadedStates.Find(absolutePath);
        if (iState != nullptr)
        {
            auto state = iState->value;
            assert(state != nullptr);

            auto definition = ModelLibrary::FindDefinition(absolutePath);
            if (definition != nullptr)
            {
                state->materials.Clear();
                for (size_t i = 0; i < definition->meshMaterials.count; ++i)
                {
                    state->materials.PushBack(definition->meshMaterials[i]->GetDefinition().fileName.c_str());
                }

                state->convexDecompositionSettings = definition->convexHullBuildSettings;
            }


            if (state == this->currentState)
            {
                this->RestoreState();
            }
        }
    }






    //////////////////////////////////////
    // Private

    void Editor_ModelEditor::ApplyCameraRotation()
    {
        this->camera.SetOrientation(glm::quat());
        this->camera.RotateY(this->cameraYRotation);
        this->camera.RotateX(this->cameraXRotation);
    }

    void Editor_ModelEditor::SaveState()
    {
        if (this->currentState != nullptr)
        {
            auto &script = this->editor.GetGame().GetScript();

            this->currentState->cameraPosition    = this->camera.GetPosition();
            this->currentState->cameraOrientation = this->camera.GetOrientation();
            this->currentState->cameraXRotation   = this->cameraXRotation;
            this->currentState->cameraYRotation   = this->cameraYRotation;


            for (int i = 0; i < static_cast<int>(this->currentState->materials.count); ++i)
            {
                GTCore::String scriptString;
                scriptString.AssignFormatted("Editor_ModelEditor_Panel_Materials:GetMaterial(%d)", i + 1);      // <-- +1 because Lua is 1 based.

                this->currentState->materials[i] = script.GetString(scriptString.c_str());
            }



            this->currentState->showConvexDecomposition = script.GetBoolean("Editor_ModelEditor_Panel_CD.ShowConvexDecomposition:IsChecked()");

            this->currentState->convexDecompositionSettings.compacityWeight               = script.GetFloat("Editor_ModelEditor_Panel_CD.CompacityWeight:GetValue()");
            this->currentState->convexDecompositionSettings.volumeWeight                  = script.GetFloat("Editor_ModelEditor_Panel_CD.VolumeWeight:GetValue()");
            this->currentState->convexDecompositionSettings.minClusters                   = static_cast<unsigned int>(script.GetInteger("Editor_ModelEditor_Panel_CD.MinClusters:GetValue()"));
            this->currentState->convexDecompositionSettings.verticesPerCH                 = static_cast<unsigned int>(script.GetInteger("Editor_ModelEditor_Panel_CD.VerticesPerCH:GetValue()"));
            this->currentState->convexDecompositionSettings.concavity                     = script.GetFloat("Editor_ModelEditor_Panel_CD.Concavity:GetValue()");
            this->currentState->convexDecompositionSettings.smallClusterThreshold         = script.GetFloat("Editor_ModelEditor_Panel_CD.SmallThreshold:GetValue()");
            this->currentState->convexDecompositionSettings.connectedComponentsDist       = script.GetFloat("Editor_ModelEditor_Panel_CD.ConnectedDistance:GetValue()");
            this->currentState->convexDecompositionSettings.simplifiedTriangleCountTarget = static_cast<unsigned int>(script.GetInteger("Editor_ModelEditor_Panel_CD.SimplifiedTriangleCount:GetValue()"));
            this->currentState->convexDecompositionSettings.addExtraDistPoints            = script.GetBoolean("Editor_ModelEditor_Panel_CD.AddExtraDistPoints:IsChecked()");
            this->currentState->convexDecompositionSettings.addFacesPoints                = script.GetBoolean("Editor_ModelEditor_Panel_CD.AddFacePoints:IsChecked()");
        }
    }

    void Editor_ModelEditor::RestoreState()
    {
        if (this->currentState != nullptr)
        {
            auto &script = this->editor.GetGame().GetScript();

            this->camera.SetPosition(this->currentState->cameraPosition);
            this->camera.SetOrientation(this->currentState->cameraOrientation);
            this->cameraXRotation = this->currentState->cameraXRotation;
            this->cameraYRotation = this->currentState->cameraYRotation;


            // Materials.
            script.Execute("Editor_ModelEditor_Panel_Materials:RemoveMaterials()");
            for (size_t i = 0; i < this->currentState->materials.count; ++i)
            {
                GTCore::String scriptString;
                scriptString.AssignFormatted("Editor_ModelEditor_Panel_Materials:AddMaterial('%s')", this->currentState->materials[i].c_str());

                script.Execute(scriptString.c_str());
            }


            GTCore::String scriptString;
            scriptString.AssignFormatted("Editor_ModelEditor_Panel_CD.ShowConvexDecomposition:SetChecked(%s);", this->currentState->showConvexDecomposition ? "true" : "false");
            script.Execute(scriptString.c_str());

            scriptString.AssignFormatted
            (
                "Editor_ModelEditor_Panel_CD.CompacityWeight:SetValue(%f);"
                "Editor_ModelEditor_Panel_CD.VolumeWeight:SetValue(%f);"
                "Editor_ModelEditor_Panel_CD.MinClusters:SetValue(%d);"
                "Editor_ModelEditor_Panel_CD.VerticesPerCH:SetValue(%d);"
                "Editor_ModelEditor_Panel_CD.Concavity:SetValue(%f);"
                "Editor_ModelEditor_Panel_CD.SmallThreshold:SetValue(%f);"
                "Editor_ModelEditor_Panel_CD.ConnectedDistance:SetValue(%f);"
                "Editor_ModelEditor_Panel_CD.SimplifiedTriangleCount:SetValue(%d);"
                "Editor_ModelEditor_Panel_CD.AddExtraDistPoints:SetChecked(%s);"
                "Editor_ModelEditor_Panel_CD.AddFacePoints:SetChecked(%s);",

                this->currentState->convexDecompositionSettings.compacityWeight,
                this->currentState->convexDecompositionSettings.volumeWeight,
                this->currentState->convexDecompositionSettings.minClusters,
                this->currentState->convexDecompositionSettings.verticesPerCH,
                this->currentState->convexDecompositionSettings.concavity,
                this->currentState->convexDecompositionSettings.smallClusterThreshold,
                this->currentState->convexDecompositionSettings.connectedComponentsDist,
                this->currentState->convexDecompositionSettings.simplifiedTriangleCountTarget,
                this->currentState->convexDecompositionSettings.addExtraDistPoints ? "true" : "false",
                this->currentState->convexDecompositionSettings.addFacesPoints     ? "true" : "false"
            );
            script.Execute(scriptString.c_str());
        }
    }

    Model* Editor_ModelEditor::GetCurrentModel()
    {
        return this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
    }

    void Editor_ModelEditor::DeleteConvexHulls()
    {
        // We need to delete any existing convex hull nodes so that they are re-built to account for changes.
        for (size_t i = 0; i < this->convexHullNodes.count; ++i)
        {
            // TODO: We need to delete the model we created for this node.
            delete this->convexHullNodes[i];
        }
        this->convexHullNodes.Clear();
    }
}
