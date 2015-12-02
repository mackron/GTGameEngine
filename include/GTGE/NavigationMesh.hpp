// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_NavigationMesh
#define GT_NavigationMesh

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-conversion"
    #pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <GTGE/Recast/Recast.h>
#include <GTGE/Recast/DetourNavMesh.h>
#include <GTGE/Recast/DetourNavMeshBuilder.h>
#include <GTGE/Recast/DetourNavMeshQuery.h>

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif


#include <GTGE/Core/Vector.hpp>
#include "Math.hpp"
#include "Serialization.hpp"
#include "MeshBuilder.hpp"

#include "Rendering/VertexArray.hpp"    // <-- Remove this later.


namespace GT
{
    class Scene;

    /// Class representing a navigation mesh.
    ///
    /// A navigation mesh is used for allowing an actor to know where it is able to move while navigating between two points.
    ///
    /// Each scene will have a navigation mesh object.
    ///
    /// The current implementation of this will require a complete rebuild of the navigation data using a scene object. In this future this
    /// will probably become more adaptive for more dynamic readjustments.
    class NavigationMesh
    {
    public:

        /// Constructor.
        NavigationMesh();

        /// Destructor.
        ~NavigationMesh();


        /// Builds the navigation mesh from a scene.
        ///
        /// @remarks
        ///     This is build from the static dynamic objects.
        ///     @par
        ///     This will need to be called when the navigation mesh needs to be updated.
        bool Build(const Scene &scene);


        /// Sets the cell size that will be used when building the navigation mesh.
        ///
        /// @param size [in] The width, height and depth of each cell.
        ///
        /// @remarks
        ///     A smaller cell size means a higher resolution mesh will be generated at the expense of performance. Defaults to 0.5 (half a meter).
        void SetCellSize(float size);

        void SetWalkableHeight(float height);
        void SetWalkableRadius(float radius);
        void SetWalkableSlope(float angle);
        void SetWalkableClimb(float height);

        float GetWalkableHeight() const;
        float GetWalkableRadius() const;
        float GetWalkableSlope() const;
        float GetWalkableClimb() const;


        /// Retrieves the points of a path between the given start and end positions.
        ///
        /// @param start  [in] The start position.
        /// @param end    [in] The end position.
        /// @param output [in] The vector that will receive the positions of the points on the path.
        ///
        /// @return True if the path is generated successfully; false otherwise.
        bool FindPath(const glm::vec3 &start, const glm::vec3 &end, Vector<glm::vec3> &output);


        /// Builds the visualization of the navigation mesh into a mesh builder.
        ///
        /// @param mainMesh      [in] A reference to the mesh builder that will receive the geometric data for the main mesh.
        /// @param innerEdgeMesh [in] A reference to the mesh builder that will receive the wireframe geometric data of the polygons in the main mesh.
        /// @param outerEdgeMesh [in] A reference to the mesh builder that will receive the wireframe geometric data of the outer edges of the outer polygons.
        void BuildMeshVisualization(MeshBuilderP3 &mainMesh, MeshBuilderP3 &innerEdgeMesh, MeshBuilderP3 &outerEdgeMesh) const;


        /////////////////////////////////////////////
        // Serialization/Deserialization

        /// Serializes the navigation mesh.
        void Serialize(Serializer &serializer) const;

        /// Deserializes the navigation mesh.
        bool Deserialize(Deserializer &deserializer);


    private:

        /// Rebuilds the vertex array for doing the visual representation of the nav mesh.
        void RebuildVisualVA();



    public:

        /// The configuration for building the Recast navmesh.
        rcConfig config;

        /// A pointer to the main navigation mesh.
        rcPolyMesh* m_mesh;

        /// A pointer to the detail mesh containing height information.
        rcPolyMeshDetail* detailMesh;

        /// A pointer to the detour nav mesh.
        dtNavMesh* detourNavMesh;

        /// A pointer to the detour nav mesh query object for doing pathfinding queries.
        dtNavMeshQuery* navMeshQuery;


        /// The walkable height in units. Defaults to 2.0.
        float walkableHeight;

        /// The walkable radius in units. Defaults to 0.5.
        float walkableRadius;

        /// The walkable slope. Defaults to 27.5.
        float walkableSlope;

        /// The size of a step an actor can step over. Defaults to 0.25.
        float walkableClimb;


        /// The vertex array containing geometric data for the visual representation.
        VertexArray* visualVA;
        
        
    private:
        NavigationMesh(const NavigationMesh &);
        NavigationMesh & operator=(const NavigationMesh&);
    };
}

#endif
