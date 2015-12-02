// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/DefaultPrefabLinker.hpp>
#include <GTGE/Scene.hpp>

namespace GT
{
    DefaultPrefabLinker::DefaultPrefabLinker(Scene &sceneIn)
        : scene(sceneIn), deserializingSceneNodeData(), lastCreatedSceneNode(nullptr)
    {
    }

    DefaultPrefabLinker::~DefaultPrefabLinker()
    {
    }



    SceneNode* DefaultPrefabLinker::CreateSceneNode()
    {
        this->lastCreatedSceneNode = this->scene.CreateNewSceneNode();

        return this->lastCreatedSceneNode;
    }

    void DefaultPrefabLinker::DeleteSceneNode(SceneNode &sceneNode)
    {
        this->scene.RemoveSceneNode(sceneNode);
    }

    void DefaultPrefabLinker::OnSceneNodeDeserializeStart(SceneNode &sceneNode)
    {
        this->deserializingSceneNodeData.sceneNode = &sceneNode;
        this->deserializingSceneNodeData.name      = sceneNode.GetName();

        sceneNode.GetWorldTransformComponents(this->deserializingSceneNodeData.worldPosition, this->deserializingSceneNodeData.worldOrientation, this->deserializingSceneNodeData.worldScale);
    }

    void DefaultPrefabLinker::OnSceneNodeDeserializeEnd(SceneNode &sceneNode)
    {
        assert(this->deserializingSceneNodeData.sceneNode != nullptr);
        {
            // The name needs to be restored, but only if there actually was a name. If the name was empty, we'll just leave it be, which
            // will cause it to be set as defined by the prefab.
            if (!this->deserializingSceneNodeData.name.IsEmpty())
            {
                sceneNode.SetName(this->deserializingSceneNodeData.name.c_str());
            }

            // The world transform needs to be restored, but only if the scene node is the root and not a newly created one.
            bool isNewlyCreatedSceneNode = this->lastCreatedSceneNode == &sceneNode;
            if (!isNewlyCreatedSceneNode)
            {
                if (this->IsRootSceneNode(sceneNode))
                {
                    sceneNode.SetWorldTransformComponents(this->deserializingSceneNodeData.worldPosition, this->deserializingSceneNodeData.worldOrientation, this->deserializingSceneNodeData.worldScale);
                }
            }
        }

        this->deserializingSceneNodeData.sceneNode = nullptr;
    }
}