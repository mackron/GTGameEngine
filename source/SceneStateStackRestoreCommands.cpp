// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneStateStackRestoreCommands.hpp>
#include <GTEngine/SceneStateStackBranch.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    SceneStateStackRestoreCommands::SceneStateStackRestoreCommands()
        : inserts(), deletes(), updates(), hierarchy()
    {
    }

    SceneStateStackRestoreCommands::~SceneStateStackRestoreCommands()
    {
    }


    void SceneStateStackRestoreCommands::AddInsert(uint64_t sceneNodeID, GTCore::BasicSerializer* sceneNodeSerializer)
    {
        // If a delete command with the scene node is already staged, all we want to do is remove it from the deletes and just
        // ignore everything.
        auto iDelete = this->deletes.Find(sceneNodeID);
        if (iDelete != nullptr)
        {
            this->deletes.RemoveByIndex(iDelete->index);
        }
        else
        {
            // If the scene node is in the updates list we need to remove it.
            this->updates.RemoveByKey(sceneNodeID);

            if (!this->inserts.Exists(sceneNodeID))
            {
                this->inserts.Add(sceneNodeID, sceneNodeSerializer);
            }
        }
    }

    void SceneStateStackRestoreCommands::AddDelete(uint64_t sceneNodeID, GTCore::BasicSerializer* sceneNodeSerializer)
    {
        // If an insert command with the scene node is already staged, all we want to do is remove it from the inserts and just
        // ignore everything.
        auto iInsert = this->inserts.Find(sceneNodeID);
        if (iInsert != nullptr)
        {
            this->inserts.RemoveByIndex(iInsert->index);
        }
        else
        {
            this->updates.RemoveByKey(sceneNodeID);

            if (!this->deletes.Exists(sceneNodeID))
            {
                this->deletes.Add(sceneNodeID, sceneNodeSerializer);
            }
        }
    }

    void SceneStateStackRestoreCommands::AddUpdate(uint64_t sceneNodeID, GTCore::BasicSerializer* sceneNodeSerializer)
    {
        // We ignore update commands if an insert or delete command is already present.
        if (!this->inserts.Exists(sceneNodeID) &&
            !this->deletes.Exists(sceneNodeID) &&
            !this->updates.Exists(sceneNodeID))
        {
            this->updates.Add(sceneNodeID, sceneNodeSerializer);
        }
    }


    void SceneStateStackRestoreCommands::UpdateToMostRecentSerializers(SceneStateStackBranch &branch, uint32_t startFrameIndex)
    {
        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            auto iCommand = this->inserts.buffer[i];
            assert(iCommand != nullptr);
            {
                auto  sceneNodeID         = iCommand->key;
                auto &sceneNodeSerializer = iCommand->value;

                sceneNodeSerializer = branch.FindMostRecentSerializer(sceneNodeID, startFrameIndex);
            }
        }

        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto iCommand = this->deletes.buffer[i];
            assert(iCommand != nullptr);
            {
                auto  sceneNodeID         = iCommand->key;
                auto &sceneNodeSerializer = iCommand->value;

                sceneNodeSerializer = branch.FindMostRecentSerializer(sceneNodeID, startFrameIndex);
            }
        }

        for (size_t i = 0; i < this->updates.count; ++i)
        {
            auto iCommand = this->updates.buffer[i];
            assert(iCommand != nullptr);
            {
                auto  sceneNodeID         = iCommand->key;
                auto &sceneNodeSerializer = iCommand->value;

                sceneNodeSerializer = branch.FindMostRecentSerializer(sceneNodeID, startFrameIndex);
            }
        }
    }

    void SceneStateStackRestoreCommands::UpdateToMostRecentHierarchy(SceneStateStackBranch &branch, uint32_t startFrameIndex)
    {
        for (size_t i = 0; i < this->hierarchy.count; )
        {
            auto iParentID = this->hierarchy.buffer[i];
            assert(iParentID != nullptr);
            {
                auto sceneNodeID = iParentID->key;

                // If the scene node ID is in the deletes commands, we don't want it to be in the heirarchy.
                if (this->deletes.Exists(sceneNodeID))
                {
                    this->hierarchy.RemoveByKey(sceneNodeID);
                }
                else
                {
                    iParentID->value = branch.FindMostRecentParentSceneNodeID(sceneNodeID, startFrameIndex);
                    ++i;
                }
            }
        }
    }


    void SceneStateStackRestoreCommands::Clear()
    {
        this->inserts.Clear();
        this->deletes.Clear();
        this->updates.Clear();

        this->hierarchy.Clear();
    }

    void SceneStateStackRestoreCommands::Execute(Scene &scene)
    {
        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            auto serializer = this->inserts.buffer[i]->value;
            assert(serializer != nullptr);
            {
                GTCore::BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());
                scene.CreateNewSceneNode(deserializer);
            }
        }

        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto sceneNodeID = this->deletes.buffer[i]->key;
            assert(sceneNodeID != 0);
            {
                scene.RemoveSceneNodeByID(sceneNodeID);
            }
        }

        for (size_t i = 0; i < this->updates.count; ++i)
        {
            auto sceneNodeID         = this->updates.buffer[i]->key;
            auto sceneNodeSerializer = this->updates.buffer[i]->value;

            assert(sceneNodeID         != 0);
            assert(sceneNodeSerializer != nullptr);
            {
                auto sceneNode = scene.GetSceneNodeByID(sceneNodeID);
                assert(sceneNode != nullptr);
                {
                    GTCore::BasicDeserializer deserializer(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
                    sceneNode->Deserialize(deserializer);
                }
            }
        }


        // Hierarchy re-construction.
        for (size_t i = 0; i < this->hierarchy.count; ++i)
        {
            auto sceneNodeID       = this->hierarchy.buffer[i]->key;
            auto parentSceneNodeID = this->hierarchy.buffer[i]->value;

            assert(sceneNodeID != 0);
            {
                auto sceneNode = scene.GetSceneNodeByID(sceneNodeID);
                assert(sceneNode != nullptr);
                {
                    if (parentSceneNodeID != 0)
                    {
                        auto parentSceneNode = scene.GetSceneNodeByID(parentSceneNodeID);
                        assert(parentSceneNode != nullptr);
                        {
                            parentSceneNode->AttachChild(*sceneNode);
                        }
                    }
                    else
                    {
                        sceneNode->DetachFromParent(false);
                    }
                }
            }
        }
    }
}
