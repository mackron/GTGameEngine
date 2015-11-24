// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/ModelEditor/ModelEditor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTLib/Path.hpp>
#include <easy_path/easy_path.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    ModelEditor::ModelEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          modelDefinition(), m_model(modelDefinition),
          scene(), camera(),
          modelNode(), convexHullParentNode(), convexHullNodes(),
          mainElement(nullptr), viewportElement(nullptr), timelineElement(nullptr),
          viewportEventHandler(ownerEditor.GetGame(), scene.GetDefaultViewport()),
          cameraXRotation(0.0f), cameraYRotation(0.0f),
          grid(0.25f, 8, 32),
          random(),
          currentlyPlayingSegmentIndex(-1), currentlyPlayingSequenceIndex(-1),
          isSaving(false), isReloading(false)
    {
        // We use the camera for our lights.
        this->camera.AddComponent<GTEngine::CameraComponent>()->Set3DProjection(90.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        this->camera.AddComponent<GTEngine::DirectionalLightComponent>()->SetColour(0.25f, 0.25f, 0.25f);
        this->camera.AddComponent<GTEngine::AmbientLightComponent>()->SetColour(0.6f, 0.6f, 0.6f);

        // Viewport and Renderer.
        this->scene.SetDefaultViewportCamera(this->camera);
        this->scene.GetRenderer().EnableBackgroundColourClearing(0.5f, 0.5f, 0.5f);


        // HDR should be disabled for this.
        static_cast<DefaultSceneRenderer &>(this->scene.GetRenderer()).DisableHDR();

        // Show the grid.
        this->grid.Show(this->scene.GetRenderer());


        // Load the model. If it needs serialization, we'll also do that. This mimicks the behaviour of the model library.
        bool needsSerialize;
        if (this->modelDefinition.LoadFromFile(absolutePath, relativePath, needsSerialize))
        {
            if (needsSerialize)
            {
                ModelLibrary::WriteToFile(this->modelDefinition);
            }
        }

        // Refresh the model to have it show the current state of the model definition.
        m_model.OnDefinitionChanged();

        // Add a model component to the scene node.
        this->modelNode.AddComponent<GTEngine::ModelComponent>()->SetModel(m_model);


        auto &gui    = this->GetGUI();
        auto &script = this->GetScript();

        this->mainElement = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='model-editor-main' />");
        assert(this->mainElement != nullptr);
        {
            // The main element is the ModelEditor element. We need to pass 'this' as the '_internalPtr' argument.
            script.Get(GTLib::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("ModelEditor");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);       // 'self'
                    script.Push(this);          // '_internalPtr'
                    script.Call(2, 0);

                    // At this point we will have the scene editor initialized in the scripting environment, and is sitting at the top of the stack. So now we
                    // need to retrieve pointers to some of it's objects.
                    script.Push("Viewport");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.Push("GetID");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);   // <-- 'self'
                            script.Call(1, 1);
                            assert(script.IsString(-1));
                            {
                                this->viewportElement = gui.GetElementByID(script.ToString(-1));

                                this->viewportElement->AttachEventHandler(this->viewportEventHandler);
                                this->viewportElement->OnSize();
                            }
                            script.Pop(1);          // <-- return value from GetID()
                        }
                    }
                    script.Pop(1);

                    // Timeline Element
                    script.Push("Timeline");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.Push("GetID");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);   // <-- 'self'
                            script.Call(1, 1);
                            assert(script.IsString(-1));
                            {
                                this->timelineElement = gui.GetElementByID(script.ToString(-1));
                            }
                            script.Pop(1);          // <-- return value from GetID()
                        }
                    }
                    script.Pop(1);
                }
            }
            script.Pop(1);
        }






        // Convex hull.
        this->convexHullParentNode.Hide();
        this->scene.AddSceneNode(this->convexHullParentNode);


        // Now we add the scene nodes to the scene.
        this->scene.AddSceneNode(this->camera);
        this->scene.AddSceneNode(this->modelNode);


        // We'll position the camera properly by just resetting it.
        this->ResetCamera();
    }

    ModelEditor::~ModelEditor()
    {
        this->GetGUI().DeleteElement(this->mainElement);

        // Convex hulls need to be deleted.
        this->DeleteConvexHulls();
    }



    void ModelEditor::ResetCamera()
    {
        auto &script = this->GetScript();

        script.Get(GTLib::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("ResetCamera");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }


    ModelDefinition & ModelEditor::GetModelDefinition()
    {
        return this->modelDefinition;
    }


    void ModelEditor::GetModelAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax)
    {
        auto modelComponent = this->modelNode.GetComponent<ModelComponent>();
        assert(modelComponent != nullptr);
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                model->GetAABB(aabbMin, aabbMax);
            }
        }
    }


    void ModelEditor::RefreshViewport()
    {
        m_model.OnDefinitionChanged();
    }


    void ModelEditor::GetMaterials(GTLib::Vector<GTLib::String> &materialsOut)
    {
        auto modelComponent = this->modelNode.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                auto &definition = model->GetDefinition();

                for (size_t i = 0; i < definition.meshes.count; ++i)
                {
                    if (definition.meshes[i].material != nullptr)
                    {
                        materialsOut.PushBack(definition.meshes[i].material->GetDefinition().relativePath);
                    }
                    else
                    {
                        materialsOut.PushBack("");
                    }
                }
            }
        }
    }


    void ModelEditor::AddNewAnimationSegment(const char* name, size_t startIndex, size_t endIndex)
    {
        this->modelDefinition.m_animation.AddNamedSegment(name, startIndex, endIndex);
        this->MarkAsModified();
    }

    void ModelEditor::RemoveAnimationSegmentByIndex(size_t index)
    {
        this->modelDefinition.m_animation.RemoveSegmentByIndex(index);
        this->MarkAsModified();
    }

    void ModelEditor::SetAnimationSegmentName(size_t index, const char* newName)
    {
        auto segment = this->modelDefinition.m_animation.GetNamedSegmentByIndex(index);
        if (segment != nullptr)
        {
            segment->name = newName;
            this->MarkAsModified();
        }
    }

    const char* ModelEditor::GetAnimationSegmentName(size_t index) const
    {
        auto segment = this->modelDefinition.m_animation.GetNamedSegmentByIndex(index);
        if (segment != nullptr)
        {
            return segment->name.c_str();
        }

        return nullptr;
    }

    void ModelEditor::SetAnimationSegmentFrameIndices(size_t segmentIndex, size_t startIndex, size_t endIndex)
    {
        auto segment = this->modelDefinition.m_animation.GetNamedSegmentByIndex(segmentIndex);
        if (segment != nullptr)
        {
            segment->startKeyFrame = startIndex;
            segment->endKeyFrame   = endIndex;

            this->MarkAsModified();
        }
    }

    void ModelEditor::GetAnimationSegmentFrameIndices(size_t segmentIndex, size_t &startIndex, size_t &endIndex) const
    {
        auto segment = this->modelDefinition.m_animation.GetNamedSegmentByIndex(segmentIndex);
        if (segment != nullptr)
        {
            startIndex = segment->startKeyFrame;
            endIndex   = segment->endKeyFrame;
        }
    }


    void ModelEditor::PlayAnimationSegmentByIndex(size_t segmentIndex)
    {
        auto segment = this->modelDefinition.m_animation.GetNamedSegmentByIndex(segmentIndex);
        if (segment != nullptr)
        {
            AnimationSequence sequence;
            sequence.AddFrame(segment->startKeyFrame, segment->endKeyFrame, 0.0, true);

            m_model.PlayAnimation(sequence);
            this->UpdateAnimationPlaybackControls();
        }
    }

    void ModelEditor::PlayAnimation()
    {
        AnimationSequence sequence;
        sequence.AddFrame(0, m_model.animation.GetKeyFrameCount(), 0.0f, true);      // '0.0f' is the transition time. 'true' says to loop.

        m_model.PlayAnimation(sequence);
    }

    void ModelEditor::ResumeAnimation()
    {
        m_model.ResumeAnimation();
        this->UpdateAnimationPlaybackControls();
    }

    void ModelEditor::StopAnimation()
    {
        m_model.StopAnimation();
        this->UpdateAnimationPlaybackControls();
    }

    void ModelEditor::PauseAnimation()
    {
        m_model.PauseAnimation();
        this->UpdateAnimationPlaybackControls();
    }

    bool ModelEditor::IsAnimationPlaying() const
    {
        return m_model.IsAnimating();
    }

    bool ModelEditor::IsAnimationPaused() const
    {
        return m_model.IsAnimationPaused();
    }


    bool ModelEditor::SetMaterial(size_t index, const char* relativePath)
    {
        auto newMaterial = MaterialLibrary::Create(relativePath);
        if (newMaterial)
        {
            MaterialLibrary::Delete(this->modelDefinition.meshes[index].material);
            this->modelDefinition.meshes[index].material = newMaterial;
            this->modelDefinition.meshes[index].defaultUniforms.Clear();

            this->RefreshViewport();

            return true;
        }

        return false;
    }


    void ModelEditor::GetConvexHullBuildSettings(ConvexHullBuildSettings &settingsOut)
    {
        auto modelComponent = this->modelNode.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                settingsOut = model->GetDefinition().convexHullBuildSettings;
            }
        }
    }

    void ModelEditor::ShowConvexDecomposition()
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
                        hullModel->meshes[0]->GetMaterial()->SetParameter("DiffuseColour", this->random.Next<float>(0.0f, 1.0f), this->random.Next<float>(0.0f, 1.0f), this->random.Next<float>(0.0f, 1.0f));

                        auto node = new SceneNode;
                        node->AddComponent<GTEngine::ModelComponent>()->SetModel(hullModel);

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

    void ModelEditor::HideConvexDecomposition()
    {
        this->convexHullParentNode.Hide();
        this->modelNode.Show();
    }

    void ModelEditor::BuildConvexDecomposition(ConvexHullBuildSettings &settings)
    {
        // The old convex hulls need to be deleted.
        this->DeleteConvexHulls();

        // We build the convex decomposition on the definition directly.
        this->modelDefinition.BuildConvexDecomposition(settings);


        if (this->convexHullParentNode.IsVisible())
        {
            this->ShowConvexDecomposition();
        }
    }


    ///////////////////////////////////////////////////
    // Virtual Methods.

    void ModelEditor::Show()
    {
        this->mainElement->Show();
    }

    void ModelEditor::Hide()
    {
        this->mainElement->Hide();
    }

    bool ModelEditor::Save()
    {
        bool wasSaved = false;

        this->isSaving = true;
        {
            wasSaved = ModelLibrary::WriteToFile(this->modelDefinition);

            if (wasSaved)
            {
                this->UnmarkAsModified();
            }


            // We want to immediatly force the game to check for changes so that the model is immediately reloaded.
            auto &dataFilesWatcher = this->GetOwnerEditor().GetGame().GetDataFilesWatcher();
            dataFilesWatcher.CheckForChanges(false);
            dataFilesWatcher.DispatchEvents();
        }
        this->isSaving = false;

        return wasSaved;
    }

    void ModelEditor::OnUpdate(double deltaTimeInSeconds)
    {
        if (this->viewportElement->IsVisible())
        {
            this->scene.Update(deltaTimeInSeconds);
        }
    }


    void ModelEditor::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
        if (!this->isSaving)
        {
            if (item.info.absolutePath == this->GetAbsolutePath())
            {
                this->Reload();
            }
            else
            {
                if (GTLib::Path::ExtensionEqual(item.info.absolutePath, "gtmodel"))
                {
                    char absolutePathNoExt[EASYVFS_MAX_PATH];
                    easypath_copyandremoveextension(absolutePathNoExt, sizeof(absolutePathNoExt), item.info.absolutePath);

                    if (strcmp(absolutePathNoExt, this->GetAbsolutePath()) == 0) {
                        this->Reload();
                    }
                }
            }
        }
    }





    ///////////////////////////////////////////////////
    // Private Methods.

    void ModelEditor::DeleteConvexHulls()
    {
        // We need to delete any existing convex hull nodes so that they are re-built to account for changes.
        for (size_t i = 0; i < this->convexHullNodes.count; ++i)
        {
            auto node = this->convexHullNodes[i];
            assert(node != nullptr);
            {
                auto modelComponent = node->GetComponent<ModelComponent>();
                assert(modelComponent != nullptr);
                {
                    auto model = modelComponent->GetModel();
                    assert(model != nullptr);
                    {
                        ModelLibrary::Delete(model);
                    }
                }

                delete node;
            }
        }
        this->convexHullNodes.Clear();
    }

    void ModelEditor::Refresh()
    {
        this->RefreshViewport();

        // We need to let the scene know that the model has changed. We just call OnChanged() on the component for this. If we don't do this, the
        // scene will not know that it needs to update the frustum culling volume.
        auto modelComponent = this->modelNode.GetComponent<ModelComponent>();
        assert(modelComponent != nullptr);
        {
            modelComponent->OnChanged();
        }



        auto &script = this->GetScript();

        script.Get(GTLib::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("Refresh");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);       // 'self'
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }

    void ModelEditor::Reload()
    {
        this->isReloading = true;
        {
            bool needsSerialize;
            if (this->modelDefinition.LoadFromFile(this->modelDefinition.absolutePath.c_str(), this->modelDefinition.relativePath.c_str(), needsSerialize))
            {
                if (needsSerialize && !this->IsMarkedAsModified())
                {
                    ModelLibrary::WriteToFile(this->modelDefinition);
                }
            }


            this->Refresh();
        }
        this->isReloading = false;
    }

    void ModelEditor::UpdateAnimationPlaybackControls()
    {
        auto &script = this->GetScript();

        script.Get(GTLib::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("UpdateAnimationPlaybackControls");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);       // 'self'
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
