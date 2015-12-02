// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/PrefabLinker.hpp>

namespace GT
{
    PrefabLinker::PrefabLinker()
    {
    }

    PrefabLinker::~PrefabLinker()
    {
    }


    bool PrefabLinker::LinkSceneNodeToPrefab(SceneNode &baseSceneNode, const char* prefabRelativePath, bool isSourceSceneNode)
    {
        auto prefab = PrefabLibrary::Acquire(prefabRelativePath);
        if (prefab != nullptr)
        {
            // If the scene node is already linked to another prefab, but it is not the root, we need to unlink it first.
            auto prefabComponent = baseSceneNode.GetComponent<PrefabComponent>();
            if (prefabComponent != nullptr)
            {
                if (prefabComponent->GetLocalHierarchyID() != 1)
                {
                    // If we get here, it means 'baseSceneNode' is already attached to a prefab, but it is not a root one. What we want to do if traverse
                    // upwards and find the scene node that roots it's prefab and unlink it, but without deleting scene nodes.
                    auto rootSceneNode = this->FindRootSceneNode(baseSceneNode, prefabComponent->GetPrefabRelativePath());
                    assert(rootSceneNode != nullptr);
                    {
                        this->UnlinkSceneNodeFromPrefab(*rootSceneNode, false);
                    }
                }
                else
                {
                    if (!Strings::Equal(prefabComponent->GetPrefabRelativePath(), prefabRelativePath))
                    {
                        this->UnlinkSceneNodeFromPrefab(baseSceneNode, true);
                    }
                }
            }


            // Now we need to deserialize the root node, which will be baseSceneNode. If the scene node is the source scene node (the scene node that
            // was used to create the prefab), we don't actually want to deserialize. Instead, we just recursively iterate over the children and just
            // link them via prefab components.
            if (isSourceSceneNode)
            {
                this->LinkSceneNodeToPrefab(baseSceneNode, prefab->GetRootID(), *prefab);
            }
            else
            {
                this->DeserializeSceneNode(baseSceneNode, prefab->GetRootID(), *prefab);
            }


            return true;
        }

        return false;
    }

    void PrefabLinker::UnlinkSceneNodeFromPrefab(SceneNode &baseSceneNode, bool deleteChildrenLinkedToSamePrefab)
    {
        auto prefabComponent = baseSceneNode.GetComponent<PrefabComponent>();
        if (prefabComponent != nullptr)
        {
            Vector<SceneNode*> childrenToDelete;

            for (auto child = baseSceneNode.GetFirstChild(); child != nullptr; child = child->GetNextSibling())
            {
                if (this->IsSceneNodeLinkedToPrefab(*child, prefabComponent->GetPrefabRelativePath()))
                {
                    this->UnlinkSceneNodeFromPrefab(*child, deleteChildrenLinkedToSamePrefab);

                    if (deleteChildrenLinkedToSamePrefab)
                    {
                        childrenToDelete.PushBack(child);
                    }
                }
            }

            // Delete children.
            for (size_t iChild = 0; iChild < childrenToDelete.count; ++iChild)
            {
                auto child = childrenToDelete[iChild];
                assert(child != nullptr);
                {
                    this->DeleteSceneNode(*child);
                }
            }

            baseSceneNode.RemoveComponent<PrefabComponent>();
        }
    }

    bool PrefabLinker::IsSceneNodeLinkedToPrefab(SceneNode &sceneNode, const char* prefabRelativePath)
    {
        auto prefabComponent = sceneNode.GetComponent<PrefabComponent>();
        if (prefabComponent != nullptr)
        {
            return Strings::Equal(prefabComponent->GetPrefabRelativePath(), prefabRelativePath);
        }

        return false;
    }


    bool PrefabLinker::IsRootSceneNode(SceneNode &sceneNode) const
    {
        auto prefabComponent = sceneNode.GetComponent<PrefabComponent>();
        if (prefabComponent != nullptr)
        {
            if (prefabComponent->GetLocalHierarchyID() == 1)
            {
                return true;
            }
        }

        return false;
    }

    bool PrefabLinker::IsRootSceneNode(SceneNode &sceneNode, const char* prefabRelativePath) const
    {
        auto prefabComponent = sceneNode.GetComponent<PrefabComponent>();
        if (prefabComponent != nullptr)
        {
            if (prefabComponent->GetLocalHierarchyID() == 1)
            {
                if (Strings::Equal(prefabComponent->GetPrefabRelativePath(), prefabRelativePath))
                {
                    return true;
                }
            }
        }

        return false;
    }

    SceneNode* PrefabLinker::FindRootSceneNode(SceneNode &sceneNode, const char* prefabRelativePath) const
    {
        if (this->IsRootSceneNode(sceneNode, prefabRelativePath))
        {
            return &sceneNode;
        }
        else
        {
            auto parent = sceneNode.GetParent();
            if (parent != nullptr)
            {
                return this->FindRootSceneNode(*parent, prefabRelativePath);
            }
        }

        return nullptr;
    }

    SceneNode* PrefabLinker::FindRootSceneNode(SceneNode &sceneNode) const
    {
        if (this->IsRootSceneNode(sceneNode))
        {
            return &sceneNode;
        }
        else
        {
            auto parent = sceneNode.GetParent();
            if (parent != nullptr)
            {
                return this->FindRootSceneNode(*parent);
            }
        }

        return nullptr;
    }


    ////////////////////////////////////////////////
    // Private

    void PrefabLinker::LinkSceneNodeToPrefab(SceneNode &sceneNode, uint64_t localID, const Prefab &prefab)
    {
        auto prefabComponent = sceneNode.AddComponent<PrefabComponent>();
        assert(prefabComponent != nullptr);
        {
            prefabComponent->SetPrefabRelativePath(prefab.GetRelativePath());
            prefabComponent->SetLocalHierarchyID(localID);


            // Children.
            Vector<uint64_t> childIDs;
            prefab.GetChildIDs(localID, childIDs);

            size_t iChild = 0;
            for (auto child = sceneNode.GetFirstChild(); child != nullptr; child = child->GetNextSibling())
            {
                assert(iChild < childIDs.count);        // <-- If this assert fails, it means DeserializeSceneNode() should have been called instead. This method (LinkSceneNodeToPrefab) should only be called when the scene node is the source scene node that created the prefab.
                {
                    this->LinkSceneNodeToPrefab(*child, childIDs[iChild], prefab);
                    ++iChild;
                }
            }
        }
    }

    bool PrefabLinker::DeserializeSceneNode(SceneNode &sceneNode, uint64_t localID, const Prefab &prefab)
    {
        auto serializer = prefab.GetSerializerByID(localID);
        if (serializer != nullptr)
        {
            // Before deserializing, we need to let the derived class know about it.
            this->OnSceneNodeDeserializeStart(sceneNode);

            BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());
            sceneNode.Deserialize(deserializer, SceneNode::NoID | SceneNode::NoScriptPublicVariableOverride | SceneNode::NoScriptOnDeserialize);       // <-- Super important! We need the ID to be maintained!

            // The link to the prefab will have been broken with deserialization, so that will need to be restored.
            auto prefabComponent = sceneNode.AddComponent<PrefabComponent>();
            assert(prefabComponent != nullptr);
            {
                prefabComponent->SetPrefabRelativePath(prefab.GetRelativePath());
                prefabComponent->SetLocalHierarchyID(localID);
            }

            // We need to let derived classes know when a scene node has finished deserializing.
            this->OnSceneNodeDeserializeEnd(sceneNode);


            // Now we want to do the same with the children. Missing children need to be created. What we do, is we grab the list
            // of child IDs from the prefab and remove those that exist. Any left overs need to be created.
            Vector<uint64_t> childIDs;
            prefab.GetChildIDs(localID, childIDs);

            for (auto child = sceneNode.GetFirstChild(); child != nullptr; child = child->GetNextSibling())
            {
                auto childPrefabComponent = child->GetComponent<PrefabComponent>();
                if (childPrefabComponent != nullptr)
                {
                    if (Strings::Equal(childPrefabComponent->GetPrefabRelativePath(), prefab.GetRelativePath()))
                    {
                        // If the child has a local ID that is not the the child ID list, it will be unlinked. Otherwise, it will be deserialized.
                        uint64_t childID = childPrefabComponent->GetLocalHierarchyID();
                        if (childIDs.Exists(childID))
                        {
                            this->DeserializeSceneNode(*child, childID, prefab);
                            childIDs.RemoveFirstOccuranceOf(childID);
                        }
                        else
                        {
                            child->RemoveComponent<PrefabComponent>();  // <-- Unlink, but don't delete.
                        }
                    }
                }
            }

            // Some children need to be created and in turn deserialized.
            for (size_t iChild = 0; iChild < childIDs.count; ++iChild)
            {
                auto newSceneNode = this->CreateSceneNode();
                assert(newSceneNode != nullptr);
                {
                    // The new scene node needs to be attached to it's parent.
                    sceneNode.AttachChild(*newSceneNode);
                    
                    // After the scene node is attached, we can just deserialize.
                    this->DeserializeSceneNode(*newSceneNode, childIDs[iChild], prefab);
                }
            }

            return true;
        }

        return false;
    }
}
