
#include <GTEngine/Scene.hpp>
#include <GTEngine/DefaultSceneUpdateManager.hpp>
#include <GTEngine/DefaultScenePhysicsManager.hpp>
#include <GTEngine/DefaultSceneCullingManager.hpp>


namespace GTEngine
{
    Scene::Scene()
        : updateManager(*new DefaultSceneUpdateManager), physicsManager(*new DefaultScenePhysicsManager), cullingManager(*new DefaultSceneCullingManager),
          deleteUpdateManager(true), deletePhysicsManager(true), deleteCullingManager(true),
          paused(false)
    {
    }

    Scene::Scene(SceneUpdateManager &updateManagerIn, ScenePhysicsManager &physicsManagerIn, SceneCullingManager &cullingManagerIn)
        : updateManager(updateManagerIn), physicsManager(physicsManagerIn), cullingManager(cullingManagerIn),
          deleteUpdateManager(false), deletePhysicsManager(false), deleteCullingManager(false),
          paused(false)
    {
    }

    Scene::~Scene()
    {
        if (deleteUpdateManager)
        {
            delete &this->updateManager;
        }

        if (deletePhysicsManager)
        {
            delete &this->physicsManager;
        }

        if (deleteCullingManager)
        {
            delete &this->cullingManager;
        }
    }

    void Scene::AddObject(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            this->AddSceneNode(static_cast<SceneNode &>(object));
        }
    }

    void Scene::RemoveObject(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            this->RemoveSceneNode(static_cast<SceneNode &>(object));
        }
    }


    void Scene::RefreshObject(SceneObject &object)
    {
        this->RemoveObject(object);
        this->AddObject(object);
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