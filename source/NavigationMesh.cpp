// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/NavigationMesh.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Recast/RecastAlloc.h>

namespace GTEngine
{
    NavigationMesh::NavigationMesh()
        : config(),
          mesh(nullptr), detailMesh(nullptr),
          detourNavMesh(nullptr), navMeshQuery(nullptr),
          walkableHeight(2.0f), walkableRadius(0.85f), walkableSlope(27.5f), walkableClimb(0.25f),
          visualVA(Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3T2N3))
    {
        memset(&this->config, 0, sizeof(this->config));
        this->SetCellSize(0.25f);

        this->config.tileSize               = 32;
        this->config.maxSimplificationError = 2.0f;
        this->config.maxVertsPerPoly        = 3;        // Triangles. Good for rendering.
        this->config.detailSampleDist       = 1.0f;
        this->config.detailSampleMaxError   = 1.0f;
        this->config.minRegionArea          = static_cast<int>(rcSqr(8.0f));
        this->config.mergeRegionArea        = static_cast<int>(rcSqr(20.0f));
    }

    NavigationMesh::~NavigationMesh()
    {
        rcFreePolyMesh(this->mesh);
        rcFreePolyMeshDetail(this->detailMesh);
        dtFreeNavMesh(this->detourNavMesh);
        dtFreeNavMeshQuery(this->navMeshQuery);

        Renderer::DeleteVertexArray(this->visualVA);
    }


    bool NavigationMesh::Build(const Scene &scene)
    {
        bool successful = false;

        glm::vec3 aabbMin;
        glm::vec3 aabbMax;
        scene.GetAABB(aabbMin, aabbMax);

        this->config.walkableHeight     = static_cast<int>(ceilf( this->walkableHeight / this->config.ch));
        this->config.walkableRadius     = static_cast<int>(floorf(this->walkableRadius / this->config.cs));
        this->config.walkableSlopeAngle = this->walkableSlope;
        this->config.walkableClimb      = static_cast<int>(ceilf(this->walkableClimb / this->config.ch));
        //this->config.borderSize         = this->config.walkableRadius + 3;
        //this->config.width              = this->config.tileSize + this->config.borderSize * 2;
        //this->config.height             = this->config.tileSize + this->config.borderSize * 2;
        
	    rcVcopy(this->config.bmin, &aabbMin[0]);
	    rcVcopy(this->config.bmax, &aabbMax[0]);
        rcCalcGridSize(this->config.bmin, this->config.bmax, this->config.cs, &this->config.width, &this->config.height);

        rcFreePolyMesh(this->mesh);
        this->mesh = nullptr;

        rcFreePolyMeshDetail(this->detailMesh);
        this->detailMesh = nullptr;


        // The context. This only exists so we can pass it around to the rc* functions. We get a failed assertion if we pass null, unfortunately.
        rcContext context;

        // NOTE: WE WILL CRASH IF THERE IS A STATIC PLANE IN THE SCENE. NEED TO FIX.

        // We need a heightfield...
        auto heightfield = rcAllocHeightfield();
        assert(heightfield != nullptr);

        if (rcCreateHeightfield(&context, *heightfield, this->config.width, this->config.height, this->config.bmin, this->config.bmax, this->config.cs, this->config.ch))
        {
            // Here is where we create the geometric data for the applicable scene nodes. For now we will look only at static meshes, but we will consider
            // obstacles later on.
            auto &nodes = scene.GetSceneNodes();
            for (size_t i = 0; i < nodes.GetCount(); ++i)
            {
                auto node = nodes.GetSceneNodeAtIndex(i);
                assert(node != nullptr);
                {
                    auto dynamics = node->GetComponent<GTEngine::DynamicsComponent>();
                    if (dynamics != nullptr && dynamics->IsNavigationMeshGenerationEnabled())
                    {
                        auto mesh = dynamics->CreateCollisionShapeMesh(true);   // <-- 'true' means to apply the scene node's transformation.
                        if (mesh != nullptr)
                        {
                            // With the mesh information retrieved we can now rasterize the mesh on the heightfield.
                            auto vertices      = mesh->GetVertexDataPtr();
                            auto vertexCount   = mesh->GetVertexCount();
                            auto indices       = reinterpret_cast<const int*>(mesh->GetIndexDataPtr());
                            auto indexCount    = mesh->GetIndexCount();
                            auto triangleCount = indexCount / 3;

                            auto walkableAreas = new unsigned char[triangleCount];
                            memset(walkableAreas, 0, triangleCount * sizeof(unsigned char));

                            rcMarkWalkableTriangles(&context, this->config.walkableSlopeAngle, vertices, vertexCount, indices, triangleCount, walkableAreas);
                            rcRasterizeTriangles(&context, vertices, vertexCount, indices, walkableAreas, triangleCount, *heightfield, this->config.walkableClimb);

                            delete [] walkableAreas;
                            delete mesh;
                        }
                    }
                }
            }


            // By this point we will have the triangles rasterized. Now we filter a bunch of stuff.
            rcFilterLowHangingWalkableObstacles(&context, this->config.walkableClimb, *heightfield);
	        rcFilterLedgeSpans(&context, this->config.walkableHeight, this->config.walkableClimb, *heightfield);
	        rcFilterWalkableLowHeightSpans(&context, this->config.walkableHeight, *heightfield);

            // Here is where we create the compact heightfield. After this is done we won't need the original heightfield anymore.
            auto compactHeightfield = rcAllocCompactHeightfield();
            assert(compactHeightfield != nullptr);

            if (rcBuildCompactHeightfield(&context, this->config.walkableHeight, this->config.walkableClimb, *heightfield, *compactHeightfield))
            {
                rcErodeWalkableArea(&context, this->config.walkableRadius, *compactHeightfield);
                rcBuildDistanceField(&context, *compactHeightfield);
                rcBuildRegions(&context, *compactHeightfield, this->config.borderSize, this->config.minRegionArea, this->config.mergeRegionArea);

                // In order to build the polygond mesh we'll need contours.
                auto contours = rcAllocContourSet();
                assert(contours != nullptr);

                if (rcBuildContours(&context, *compactHeightfield, this->config.maxSimplificationError, this->config.maxEdgeLen, *contours))
                {
                    this->mesh = rcAllocPolyMesh();
                    assert(this->mesh != nullptr);

                    if (rcBuildPolyMesh(&context, *contours, this->config.maxVertsPerPoly, *this->mesh))
                    {
                        // Now we create the detail mesh.
                        this->detailMesh = rcAllocPolyMeshDetail();
                        assert(this->detailMesh != nullptr);

                        if (rcBuildPolyMeshDetail(&context, *this->mesh, *compactHeightfield, this->config.detailSampleDist, this->config.detailSampleMaxError, *this->detailMesh))
                        {
                            // Update poly flags from areas.
		                    for (int i = 0; i < this->mesh->npolys; ++i)
		                    {
			                    if (this->mesh->areas[i] == RC_WALKABLE_AREA)
                                {
				                    this->mesh->areas[i] = 1;
                                    this->mesh->flags[i] = 1;
                                }
		                    }

                            // If we've made it here we can now create the detour navigation mesh.
                            dtNavMeshCreateParams params;
		                    memset(&params, 0, sizeof(params));
                            params.cs               = this->config.cs;
		                    params.ch               = this->config.ch;
		                    params.buildBvTree      = true;
		                    params.verts            = this->mesh->verts;
		                    params.vertCount        = this->mesh->nverts;
		                    params.polys            = this->mesh->polys;
		                    params.polyAreas        = this->mesh->areas;
		                    params.polyFlags        = this->mesh->flags;
		                    params.polyCount        = this->mesh->npolys;
		                    params.nvp              = this->mesh->nvp;
                            params.detailMeshes     = this->detailMesh->meshes;
		                    params.detailVerts      = this->detailMesh->verts;
		                    params.detailVertsCount = this->detailMesh->nverts;
		                    params.detailTris       = this->detailMesh->tris;
		                    params.detailTriCount   = this->detailMesh->ntris;
                            params.walkableHeight   = this->walkableHeight;
                            params.walkableRadius   = this->walkableRadius;
                            params.walkableClimb    = this->walkableClimb;
		                    rcVcopy(params.bmin, this->mesh->bmin);
		                    rcVcopy(params.bmax, this->mesh->bmax);
		                    

                            unsigned char* navData     = nullptr;
		                    int            navDataSize = 0;
                            if (dtCreateNavMeshData(&params, &navData, &navDataSize))
                            {
                                dtFreeNavMesh(this->detourNavMesh);
                                this->detourNavMesh = dtAllocNavMesh();
                                assert(this->detourNavMesh != nullptr);

                                dtStatus status = this->detourNavMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
                                if (!dtStatusFailed(status))
                                {
                                    // Finally we're going to create the query object so we can do pathfinding queries.
                                    dtFreeNavMeshQuery(this->navMeshQuery);
                                    this->navMeshQuery = dtAllocNavMeshQuery();
                                    assert(this->navMeshQuery);

                                    if (this->navMeshQuery->init(this->detourNavMesh, 512))
                                    {
                                        successful = true;
                                    }
                                    else
                                    {
                                        GTEngine::PostError("NavigationMesh: Failed to init nav mesh query object.");
                                    }
                                }
                                else
                                {
                                    dtFree(navData);
                                    GTEngine::PostError("NavigationMesh: Failed to init detail nav mesh.");
                                }
                            }
                            else
                            {
                                dtFree(navData);
                                GTEngine::PostError("NavigationMesh: Error creating Detour nav mesh data.");
                            }
                        }
                        else
                        {
                            GTEngine::PostError("NavigationMesh: Error creating detail mesh.");
                        }
                    }
                    else
                    {
                        GTEngine::PostError("NavigationMesh: Error creating main navigation mesh.");
                    }
                }
                else
                {
                    GTEngine::PostError("NavigationMesh: Error creating contours.");
                }


                rcFreeContourSet(contours);
            }
            else
            {
                GTEngine::PostError("NavigationMesh: Error creating compact heightfield.");
            }

            rcFreeCompactHeightfield(compactHeightfield);
        }
        else
        {
            GTEngine::PostError("NavigationMesh: Error creating heightfield.");
        }

        rcFreeHeightField(heightfield);

        // Here we will rebuild the visual vertex array.
        if (successful)
        {
            this->RebuildVisualVA();
        }

        return successful;
    }



    void NavigationMesh::SetCellSize(float size)
    {
        this->config.cs = size;
        this->config.ch = size;
    }


    void NavigationMesh::SetWalkableHeight(float height)
    {
        this->walkableHeight = height;
    }

    void NavigationMesh::SetWalkableRadius(float radius)
    {
        this->walkableRadius = radius;
    }

    void NavigationMesh::SetWalkableSlope(float angle)
    {
        this->walkableSlope = angle;
    }

    void NavigationMesh::SetWalkableClimb(float height)
    {
        this->walkableClimb = height;
    }


    float NavigationMesh::GetWalkableHeight() const
    {
        return this->walkableHeight;
    }

    float NavigationMesh::GetWalkableRadius() const
    {
        return this->walkableRadius;
    }

    float NavigationMesh::GetWalkableSlope() const
    {
        return this->walkableSlope;
    }

    float NavigationMesh::GetWalkableClimb() const
    {
        return this->walkableClimb;
    }



    bool NavigationMesh::FindPath(const glm::vec3 &start, const glm::vec3 &end, GTCore::Vector<glm::vec3> &output)
    {
        bool success = false;

        if (this->navMeshQuery != nullptr)
        {
            dtQueryFilter filter;

            glm::vec3 extents(this->walkableRadius * 2.0f, this->walkableHeight * 2.0f, this->walkableRadius * 2.0f);

            // We first need to find the polygons we should start and end on.
            dtPolyRef nearestPolyStart;
            dtPolyRef nearestPolyEnd;
            float     nearestPointStart[3];
            float     nearestPointEnd[3];
            if (!dtStatusFailed(this->navMeshQuery->findNearestPoly(&start[0], &extents[0], &filter, &nearestPolyStart, nearestPointStart)))
            {
                if (!dtStatusFailed(this->navMeshQuery->findNearestPoly(&end[0], &extents[0], &filter, &nearestPolyEnd, nearestPointEnd)))
                {
                    // We have our start and end points, so now we need to find the polys making up a path between the two points.
                    dtPolyRef path[128];
                    int       pathCount;
                    if (!dtStatusFailed(this->navMeshQuery->findPath(nearestPolyStart, nearestPolyEnd, nearestPointStart, nearestPointEnd, &filter, path, &pathCount, 128)))
                    {
                        // We have the path, so now we find a straight line between the path. This will return the points that we can use for output.
                        float* straightPath = new float[pathCount * 3];
                        int straightPathCount;
                        if (!dtStatusFailed(this->navMeshQuery->findStraightPath(nearestPointStart, nearestPointEnd, path, pathCount, straightPath, nullptr, nullptr, &straightPathCount, pathCount)))
                        {
                            // Now we need to append our results to the output list.
                            for (int i = 0; i < straightPathCount; ++i)
                            {
                                auto sourcePoint = straightPath + (i * 3);
                                output.PushBack(glm::vec3(sourcePoint[0], sourcePoint[1], sourcePoint[2]));
                            }

                            // If nearestPolyStart and nearestPolyEnd are both the same, it means we're navigating over the same poly. In this case, straightPathCount will
                            // be set to 1 which means the end point will not be copied over by default. To fix, we just do the appropriate checks here and do it manually.
                            if (nearestPolyStart == nearestPolyEnd && straightPathCount == 1)
                            {
                                output.PushBack(glm::vec3(nearestPointEnd[0], nearestPointEnd[1], nearestPointEnd[2]));
                            }

                            success = true;
                        }

                        delete [] straightPath;
                    }
                }
            }
        }

        return success;
    }


    /////////////////////////////////////////////
    // Serialization/Deserialization

    void NavigationMesh::Serialize(GTCore::Serializer &serializer) const
    {
        GTCore::BasicSerializer intermediarySerializer;
        Serialization::ChunkHeader header;


        // General variables.
        {
            intermediarySerializer.Clear();
            intermediarySerializer.Write(this->config);
            intermediarySerializer.Write(this->walkableHeight);
            intermediarySerializer.Write(this->walkableRadius);
            intermediarySerializer.Write(this->walkableSlope);
            intermediarySerializer.Write(this->walkableClimb);


            header.id          = Serialization::ChunkID_NavigationMesh_Main;
            header.version     = 1;
            header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
        }


        // The recast poly-mesh.
        if (this->mesh != nullptr)
        {
            intermediarySerializer.Clear();

            intermediarySerializer.Write(static_cast<int32_t>(this->mesh->nverts));
            intermediarySerializer.Write(static_cast<int32_t>(this->mesh->npolys));
            intermediarySerializer.Write(static_cast<int32_t>(this->mesh->maxpolys));
            intermediarySerializer.Write(static_cast<int32_t>(this->mesh->nvp));
            intermediarySerializer.Write(static_cast<float>(this->mesh->bmin[0]));
            intermediarySerializer.Write(static_cast<float>(this->mesh->bmin[1]));
            intermediarySerializer.Write(static_cast<float>(this->mesh->bmin[2]));
            intermediarySerializer.Write(static_cast<float>(this->mesh->bmax[0]));
            intermediarySerializer.Write(static_cast<float>(this->mesh->bmax[1]));
            intermediarySerializer.Write(static_cast<float>(this->mesh->bmax[2]));
            intermediarySerializer.Write(static_cast<float>(this->mesh->cs));
            intermediarySerializer.Write(static_cast<float>(this->mesh->ch));
            intermediarySerializer.Write(static_cast<int32_t>(this->mesh->borderSize));

            intermediarySerializer.Write(this->mesh->verts, sizeof(uint16_t) * this->mesh->nverts * 3);     // <-- 3 components for each vertex (x, y, z).
            intermediarySerializer.Write(this->mesh->polys, sizeof(uint16_t) * this->mesh->maxpolys * 2 * this->mesh->nvp);
            intermediarySerializer.Write(this->mesh->regs,  sizeof(uint16_t) * this->mesh->maxpolys);
            intermediarySerializer.Write(this->mesh->flags, sizeof(uint16_t) * this->mesh->maxpolys);
            intermediarySerializer.Write(this->mesh->areas, sizeof(uint8_t)  * this->mesh->maxpolys);



            header.id          = Serialization::ChunkID_NavigationMesh_RecastPolyMesh;
            header.version     = 1;
            header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
        }


        // The detour nav-mesh
        if (this->detourNavMesh != nullptr)
        {
            intermediarySerializer.Clear();

            int32_t tileCount = 0;
            for (int iTile = 0; iTile < this->detourNavMesh->getMaxTiles(); ++iTile)
            {
                auto tile = const_cast<const dtNavMesh*>(this->detourNavMesh)->getTile(iTile);
                if (tile != nullptr && tile->header != nullptr && tile->dataSize > 0)
                {
                    tileCount += 1;
                }
            }

            auto params = this->detourNavMesh->getParams();
            assert(params != nullptr);
            {
                intermediarySerializer.Write(params->orig[0]);
                intermediarySerializer.Write(params->orig[1]);
                intermediarySerializer.Write(params->orig[2]);
                intermediarySerializer.Write(params->tileWidth);
                intermediarySerializer.Write(params->tileHeight);
                intermediarySerializer.Write(static_cast<int32_t>(params->maxTiles));
                intermediarySerializer.Write(static_cast<int32_t>(params->maxPolys));
            }

            intermediarySerializer.Write(tileCount);
            
            for (int iTile = 0; iTile < this->detourNavMesh->getMaxTiles(); ++iTile)
            {
                auto tile = const_cast<const dtNavMesh*>(this->detourNavMesh)->getTile(iTile);
                if (tile != nullptr && tile->header != nullptr && tile->dataSize > 0)
                {
                    intermediarySerializer.Write(static_cast<uint32_t>(this->detourNavMesh->getTileRef(tile)));
                    intermediarySerializer.Write(static_cast<uint32_t>(tile->dataSize));
                    intermediarySerializer.Write(tile->data, static_cast<size_t>(tile->dataSize));
                }
            }


            header.id          = Serialization::ChunkID_NavigationMesh_DetourNavMesh;
            header.version     = 1;
            header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
        }


        // Null-terminating chunk.
        {
            header.id          = Serialization::ChunkID_Null;
            header.version     = 1;
            header.sizeInBytes = 0;
            serializer.Write(header);
        }
    }

    bool NavigationMesh::Deserialize(GTCore::Deserializer &deserializer)
    {
        bool successful = true;

        // We keep looping until we hit the null-terminating chunk.
        Serialization::ChunkHeader header;
        while (deserializer.Peek(&header, sizeof(header)) == sizeof(header) && header.id != Serialization::ChunkID_Null)
        {
            deserializer.Seek(sizeof(header));

            switch (header.id)
            {
            case Serialization::ChunkID_NavigationMesh_Main:
                {
                    if (header.version == 1)
                    {
                        deserializer.Read(this->config);
                        deserializer.Read(this->walkableHeight);
                        deserializer.Read(this->walkableRadius);
                        deserializer.Read(this->walkableSlope);
                        deserializer.Read(this->walkableClimb);
                    }
                    else
                    {
                        GTEngine::Log("Error deserializing main chunk of navigation mesh. Unsupported version (%d).", header.version);
                        successful = false;
                    }

                    break;
                }

            case Serialization::ChunkID_NavigationMesh_RecastPolyMesh:
                {
                    if (header.version == 1)
                    {
                        //deserializer.Seek(header.sizeInBytes);

                        // Old mesh must be deleted.
                        if (this->mesh != nullptr)
                        {
                            rcFreePolyMesh(this->mesh);
                            this->mesh = rcAllocPolyMesh();
                        }


                        int32_t nverts;
                        int32_t npolys;
                        int32_t maxpolys;
                        int32_t nvp;
                        deserializer.Read(nverts);
                        deserializer.Read(npolys);
                        deserializer.Read(maxpolys);
                        deserializer.Read(nvp);

                        float bmin[3];
                        float bmax[3];
                        deserializer.Read(bmin[0]);
                        deserializer.Read(bmin[1]);
                        deserializer.Read(bmin[2]);
                        deserializer.Read(bmax[0]);
                        deserializer.Read(bmax[1]);
                        deserializer.Read(bmax[2]);

                        float cs;
                        float ch;
                        deserializer.Read(cs);
                        deserializer.Read(ch);

                        int32_t borderSize;
                        deserializer.Read(borderSize);


                        GTCore::Vector<uint16_t> verts;
                        GTCore::Vector<uint16_t> polys;
                        GTCore::Vector<uint16_t> regs;
                        GTCore::Vector<uint16_t> flags;
                        GTCore::Vector<uint8_t> areas;

                        verts.Resize(nverts * 3);
                        deserializer.Read(verts.buffer, sizeof(uint16_t) * verts.count);

                        polys.Resize(npolys * 2 * nvp);
                        deserializer.Read(polys.buffer, sizeof(uint16_t) * polys.count);

                        regs.Resize(npolys);
                        deserializer.Read(regs.buffer, sizeof(uint16_t) * regs.count);

                        flags.Resize(npolys);
                        deserializer.Read(flags.buffer, sizeof(uint16_t) * flags.count);

                        areas.Resize(npolys);
                        deserializer.Read(areas.buffer, sizeof(uint16_t) * areas.count);


                        // I'm unaware of a public API for creating a mesh from raw data like this, so we're going to copy the implementation of
                        // rcCopyPolyMesh(). We use the same memory allocation routines as that function.

                        this->mesh->nverts     = static_cast<int>(nverts);
                        this->mesh->npolys     = static_cast<int>(npolys);
                        this->mesh->maxpolys   = static_cast<int>(maxpolys);
                        this->mesh->nvp        = static_cast<int>(nvp);
                        this->mesh->bmin[0]    = bmin[0];
                        this->mesh->bmin[1]    = bmin[1];
                        this->mesh->bmin[2]    = bmin[2];
                        this->mesh->bmax[0]    = bmax[0];
                        this->mesh->bmax[1]    = bmax[1];
                        this->mesh->bmax[2]    = bmax[2];
                        this->mesh->cs         = cs;
                        this->mesh->ch         = ch;
                        this->mesh->borderSize = static_cast<int>(borderSize);

                        this->mesh->verts = static_cast<unsigned short*>(rcAlloc(sizeof(unsigned short) * verts.count, RC_ALLOC_PERM));
                        this->mesh->polys = static_cast<unsigned short*>(rcAlloc(sizeof(unsigned short) * polys.count, RC_ALLOC_PERM));
                        this->mesh->regs  = static_cast<unsigned short*>(rcAlloc(sizeof(unsigned short) * regs.count,  RC_ALLOC_PERM));
                        this->mesh->flags = static_cast<unsigned short*>(rcAlloc(sizeof(unsigned short) * flags.count, RC_ALLOC_PERM));
                        this->mesh->areas = static_cast<unsigned char* >(rcAlloc(sizeof(unsigned char)  * areas.count, RC_ALLOC_PERM));

                        memcpy(this->mesh->verts, verts.buffer, sizeof(unsigned short) * verts.count);
                        memcpy(this->mesh->polys, polys.buffer, sizeof(unsigned short) * polys.count);
                        memcpy(this->mesh->regs,  regs.buffer,  sizeof(unsigned short) * regs.count);
                        memcpy(this->mesh->flags, flags.buffer, sizeof(unsigned short) * flags.count);
                        memcpy(this->mesh->areas, areas.buffer, sizeof(unsigned short) * areas.count);
                    }
                    else
                    {
                        GTEngine::Log("Error deserializing Recast Poly Mesh chunk of navigation mesh. Unsupported version (%d).", header.version);
                        successful = false;
                    }

                    break;
                }

            case Serialization::ChunkID_NavigationMesh_DetourNavMesh:
                {
                    if (header.version == 1)
                    {
                        //deserializer.Seek(header.sizeInBytes);

                        // Old mesh must be deleted.
                        if (this->detourNavMesh != nullptr)
                        {
                            dtFreeNavMesh(this->detourNavMesh);
                            this->detourNavMesh = dtAllocNavMesh();
                        }


                        //dtNavMeshParams params;

                    }
                    else
                    {
                        GTEngine::Log("Error deserializing Detour Nav Mesh chunk of navigation mesh. Unsupported version (%d).", header.version);
                        successful = false;
                    }

                    break;
                }


            default:
                {
                    break;
                }
            }
        }

        return successful;
    }



    /////////////////////////////////////////////
    // Private

    void NavigationMesh::RebuildVisualVA()
    {
        // We use a mesh builder here.
        MeshBuilderP3T2N3 builder;

        for (int i = 0; i < this->detourNavMesh->getMaxTiles(); ++i)
        {
            auto tile = const_cast<const dtNavMesh*>(this->detourNavMesh)->getTile(i);
            if (tile->header != nullptr)
            {
                for (int j = 0; j < tile->header->polyCount; ++j)
                {
                    auto &poly = tile->polys[j];

                    if (poly.getType() != DT_POLYTYPE_OFFMESH_CONNECTION)
                    {
		                auto &pd = tile->detailMeshes[j];

                        for (int k = 0; k < pd.triCount; ++k)
		                {
			                const unsigned char* t = &tile->detailTris[(pd.triBase+k)*4];
			                for (int l = 0; l < 3; ++l)
			                {
                                float* vertex = nullptr;

				                if (t[l] < poly.vertCount)
                                {
                                    vertex = &tile->verts[poly.verts[t[l]] * 3];
                                }
				                else
                                {
                                    vertex = &tile->detailVerts[(pd.vertBase + t[l] - poly.vertCount) * 3];
                                }

                                builder.EmitVertex(glm::vec3(vertex[0], vertex[1], vertex[2]), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			                }
		                }
                    }
                }
            }
        }


        // Now we can set the VA data.
        this->visualVA->SetData(builder.GetVertexData(), builder.GetVertexCount(), builder.GetIndexData(), builder.GetIndexCount());
    }
}

