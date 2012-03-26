
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/ColladaLoader.hpp>
#include <GTEngine/Components/ModelComponent.hpp>

namespace GTEngine
{
    Model* ModelLibrary::LoadFromCollada(const char* filename)
    {
        Model* model = nullptr;

        // We use the collada loader to load the file. This will construct a scene node containing all of the nodes in
        // the collada file. All we care about is the first model component.
        GTEngine::SceneNode* colladaNode = GTEngine::ColladaLoader::Load(filename);
        if (colladaNode != nullptr)
        {
            // We just find the first node with a model component and strip it from the collada scene node, which we'll be discarding. We
            // may need to tweak this is the player mode gets more complex than a single model.
            GTEngine::SceneNode* modelNode = colladaNode->FindFirstChildWithComponent<GTEngine::ModelComponent>(true);
            if (modelNode != nullptr)
            {
                // Now that we have the model node, we will steal the model from the model component.
                model = modelNode->GetComponent<GTEngine::ModelComponent>()->GetModel();
                model->ApplyTransformation(modelNode->GetTransformMatrix());
            }
        }

        delete colladaNode;
        return model;
    }

    Model* ModelLibrary::CreatePlaneXZ(float width, float height, VertexFormat &format)
    {
        Model* model = new Model;

        model->AttachMesh(VertexArrayFactory::CreatePlaneXZ(width, height, format), nullptr);

        return model;
    }


    void ModelLibrary::Delete(Model* model)
    {
        delete model;
    }
}