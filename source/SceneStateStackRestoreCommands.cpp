
#include <GTEngine/SceneStateStackRestoreCommands.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    SceneStateStackRestoreCommands::SceneStateStackRestoreCommands()
        : inserts(), deletes(), updates()
    {
    }

    SceneStateStackRestoreCommands::~SceneStateStackRestoreCommands()
    {
    }

    void SceneStateStackRestoreCommands::Clear()
    {
        this->inserts.Clear();
        this->deletes.Clear();
        this->updates.Clear();
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
    }
}
