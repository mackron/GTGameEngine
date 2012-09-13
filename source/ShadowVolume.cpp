
#include <GTEngine/ShadowVolume.hpp>
#include <GTEngine/MeshBuilder.hpp>

namespace GTEngine
{
    ShadowVolume::ShadowVolume()
        : vertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3)
    {
    }

    ShadowVolume::~ShadowVolume()
    {
    }

    void ShadowVolume::BuildFromPointLight(const glm::vec3 &position, float lightRadius, const Model &model, const Math::Plane &nearPlane, const Math::Plane &farPlane)
    {
        (void)position;
        (void)lightRadius;
        (void)model;
        (void)nearPlane;
        (void)farPlane;

        // We will use a MeshBuilder for generating the vertex array.
        MeshBuilder builder(3);         // <-- '3' is the size of a vertex in floats. Always in P3 format here.

        // We build the shadow volume from the models shadow generation data. This data is unique in that it is arranged in such a way that
        // shadow generation is a bit easier.
    }
}
