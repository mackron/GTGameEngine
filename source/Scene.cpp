
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    Scene::Scene()
        : paused(false)
    {
    }

    Scene::~Scene()
    {
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