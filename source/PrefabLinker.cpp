// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/PrefabLinker.hpp>

namespace GTEngine
{
    PrefabLinker::PrefabLinker()
    {
    }

    PrefabLinker::~PrefabLinker()
    {
    }


    bool PrefabLinker::LinkSceneNodeToPrefab(SceneNode &baseSceneNode, const char* prefabRelativePath)
    {
        auto prefab = PrefabLibrary::Acquire(prefabRelativePath);
        if (prefab != nullptr)
        {
            // If the scene node is already linked to another prefab, but it is not the root, we need to fail.
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
                    if (!GTCore::Strings::Equal(prefabComponent->GetPrefabRelativePath(), prefabRelativePath))
                    {
                        this->UnlinkSceneNodeFromPrefab(baseSceneNode, true);
                    }
                }
            }


            // Now we need to deserialize the root node, which will be baseSceneNode.
            this->DeserializeSceneNode(baseSceneNode, prefab->GetRootID(), *prefab);


            return true;
        }

        return false;
    }

    void PrefabLinker::UnlinkSceneNodeFromPrefab(SceneNode &baseSceneNode, bool deleteChildrenLinkedToSamePrefab)
    {
        auto prefabComponent = baseSceneNode.GetComponent<PrefabComponent>();
        if (prefabComponent != nullptr)
        {
            for (auto child = baseSceneNode.GetFirstChild(); child != nullptr; child = child->GetNextSibling())
            {
                if (this->IsSceneNodeLinkedToPrefab(*child, prefabComponent->GetPrefabRelativePath()))
                {
                    this->UnlinkSceneNodeFromPrefab(*child, deleteChildrenLinkedToSamePrefab);

                    if (deleteChildrenLinkedToSamePrefab)
                    {
                        this->DeleteSceneNode(child);
                    }
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
            return GTCore::Strings::Equal(prefabComponent->GetPrefabRelativePath(), prefabRelativePath);
        }

        return false;
    }


    ////////////////////////////////////////////////
    // Private

    SceneNode* PrefabLinker::FindRootSceneNode(SceneNode &sceneNode, const char* prefabRelativePath) const
    {
        auto parent = sceneNode.GetParent();
        if (parent != nullptr)
        {
            auto parentPrefabComponent = parent->GetComponent<PrefabComponent>();
            if (parentPrefabComponent != nullptr)
            {
                if (GTCore::Strings::Equal(parentPrefabComponent->GetPrefabRelativePath(), prefabRelativePath))
                {
                    if (parentPrefabComponent->GetLocalHierarchyID() == 1)
                    {
                        return parent;
                    }
                }
            }

            return this->FindRootSceneNode(*parent, prefabRelativePath);
        }

        return nullptr;
    }

    bool PrefabLinker::DeserializeSceneNode(SceneNode &sceneNode, uint64_t localID, const Prefab &prefab)
    {
        auto serializer = prefab.GetSerializerByID(localID);
        if (serializer != nullptr)
        {
            // Before deserializing, we need to let the derived class know about it.
            this->OnSceneNodeDeserializeStart(sceneNode);

            GTCore::BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());
            sceneNode.Deserialize(deserializer, SceneNode::NoID | SceneNode::NoScriptPublicVariableOverride);       // <-- Super important! We need the ID to be maintained!

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
            GTCore::Vector<uint64_t> childIDs;
            prefab.GetChildIDs(localID, childIDs);

            for (auto child = sceneNode.GetFirstChild(); child != nullptr; child = child->GetNextSibling())
            {
                auto childPrefabComponent = child->GetComponent<PrefabComponent>();
                if (childPrefabComponent != nullptr)
                {
                    if (GTCore::Strings::Equal(childPrefabComponent->GetPrefabRelativePath(), prefab.GetRelativePath()))
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
                    this->DeserializeSceneNode(*newSceneNode, childIDs[iChild], prefab);

                    // The new scene node needs to be attached to it's parent.
                    sceneNode.AttachChild(*newSceneNode);
                }
            }

            return true;
        }

        return false;
    }
}