// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ModelDefinition.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/GarbageCollector.hpp>

namespace GTEngine
{
    ModelDefinition::ModelDefinition(const char* fileNameIn)
        : fileName(fileNameIn), absolutePath(),
          meshGeometries(), meshMaterials(), meshSkinningVertexAttributes(),
          bones(),
          animation(), animationChannelBones(), animationKeyCache(),
          convexHulls(), convexHullBuildSettings()
    {
        if (this->fileName != "")
        {
            GTCore::IO::FindAbsolutePath(fileNameIn, this->absolutePath);
        }
    }

    ModelDefinition::~ModelDefinition()
    {
        this->ClearMeshGeometries();
        this->ClearMeshSkinningVertexAttributes();
        this->ClearBones();
        this->ClearAnimations();
        this->ClearMaterials();
        this->ClearConvexHulls();
    }

    void ModelDefinition::GenerateTangentsAndBitangents()
    {
        for (size_t i = 0; i < this->meshGeometries.count; ++i)
        {
            this->meshGeometries[i]->GenerateTangentsAndBitangents();
        }
    }

    TransformAnimationKey* ModelDefinition::CreateAnimationKey(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
    {
        auto newKey = new TransformAnimationKey(position, rotation, scale);
        this->animationKeyCache.PushBack(newKey);

        return newKey;
    }

    void ModelDefinition::MapAnimationChannelToBone(Bone &bone, AnimationChannel &channel)
    {
        this->animationChannelBones.Add(&bone, &channel);
    }

    void ModelDefinition::BuildConvexDecomposition(ConvexHullBuildSettings &settings)
    {
        // We need to delete the old convex hulls.
        for (size_t i = 0; i < this->convexHulls.count; ++i)
        {
            delete this->convexHulls[i];
        }
        this->convexHulls.Clear();


        // After deleting the old convex hulls we can build the new ones.
        for (size_t i = 0; i < this->meshGeometries.count; ++i)
        {
            auto va = this->meshGeometries[i];
            if (va != nullptr)
            {
                ConvexHull* convexHulls;
                size_t      count;
                ConvexHull::BuildConvexHulls(*va, convexHulls, count, settings);

                for (size_t iHull = 0; iHull < count; ++iHull)
                {
                    this->convexHulls.PushBack(new ConvexHull(convexHulls[iHull]));
                }

                ConvexHull::DeleteConvexHulls(convexHulls);
            }
        }

        // We're going to store the settings that were used to build the convex hulls. These will be stored as metadata in the .gtmodel file.
        this->convexHullBuildSettings = settings;
    }


    void ModelDefinition::ClearMeshGeometries()
    {
        for (size_t i = 0; i < this->meshGeometries.count; ++i)
        {
            // It's important that we garbage collect here.
            Renderer2::DeleteVertexArray(this->meshGeometries[i]);
        }
        this->meshGeometries.Clear();
    }

    void ModelDefinition::ClearMeshSkinningVertexAttributes()
    {
        for (size_t i = 0; i < this->meshSkinningVertexAttributes.count; ++i)
        {
            delete [] this->meshSkinningVertexAttributes[i];
        }
        this->meshSkinningVertexAttributes.Clear();
    }

    void ModelDefinition::ClearBones()
    {
        for (size_t i = 0; i < this->bones.count; ++i)
        {
            delete this->bones.buffer[i]->value;
        }
        this->bones.Clear();
    }

    void ModelDefinition::ClearAnimations(bool clearNamedSegments)
    {
        this->animation.Clear(clearNamedSegments);

        for (size_t i = 0; i < this->animationKeyCache.count; ++i)
        {
            delete this->animationKeyCache[i];
        }
        this->animationKeyCache.Clear();

        this->animationChannelBones.Clear();
    }

    void ModelDefinition::ClearNamedAnimationSegments()
    {
        this->animation.ClearNamedSegments();
    }

    void ModelDefinition::ClearMaterials()
    {
        for (size_t i = 0; i < this->meshMaterials.count; ++i)
        {
            MaterialLibrary::Delete(this->meshMaterials[i]);
        }
        this->meshMaterials.Clear();
    }

    void ModelDefinition::ClearConvexHulls()
    {
        for (size_t i = 0; i < this->convexHulls.count; ++i)
        {
            delete this->convexHulls[i];
        }
        this->convexHulls.Clear();
    }
}
