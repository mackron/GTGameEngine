
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

    void Scene::OnSceneNodeScale(SceneNode &)
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

    void Scene::OnSceneNodeComponentChanged(SceneNode &, Component &)
    {
    }

    void Scene::OnPause()
    {
    }

    void Scene::OnResume()
    {
    }
}