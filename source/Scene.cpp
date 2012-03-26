
#include <GTEngine/Scene.hpp>
#include <GTEngine/Components/CameraComponent.hpp>
#include <GTEngine/Components/SpriteComponent.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    Scene::Scene()
        : currentRCBuffer(nullptr), paused(false)
    {
    }

    Scene::~Scene()
    {
    }

    void Scene::SetCurrentRCBuffer(RenderCommandBuffer *rcBuffer)
    {
        // Default to the renderer's back buffer if we passed null.
        if (rcBuffer == nullptr)
        {
            rcBuffer = Renderer::BackBuffer;
        }

        this->currentRCBuffer = rcBuffer;
    }

    RenderCommandBuffer * Scene::GetCurrentRCBuffer()
    {
        return this->currentRCBuffer;
    }

    void Scene::AppendRC(RenderCommand *cmd)
    {
        this->currentRCBuffer->Append(cmd);
    }


    void Scene::Pause()
    {
        if (!this->paused)
        {
            this->paused = true;
            this->OnPause();
        }
    }

    void Scene::Resume()
    {
        if (this->paused)
        {
            this->paused = false;
            this->OnResume();
        }
    }


    void Scene::OnSceneNodeAdded(SceneNode &)
    {
    }

    void Scene::OnSceneNodeRemoved(SceneNode &)
    {
    }

    void Scene::OnSceneNodeTransform(SceneNode &)
    {
    }

    void Scene::OnSceneNodeMove(SceneNode &, const glm::vec3 &)
    {
    }

    void Scene::OnSceneNodeRotate(SceneNode &, const glm::quat &)
    {
    }

    void Scene::OnSceneNodeScale(SceneNode &, const glm::vec3 &)
    {
    }

    void Scene::OnSceneNodeStaticChanged(SceneNode &)
    {
    }

    void Scene::OnSceneNodeVisibleChanged(SceneNode &)
    {
    }

    void Scene::OnSceneNodeComponentAttached(SceneNode &, Component &)
    {
    }

    void Scene::OnSceneNodeComponentDetached(SceneNode &, Component &)
    {
    }

    void Scene::OnPause()
    {
    }

    void Scene::OnResume()
    {
    }
}