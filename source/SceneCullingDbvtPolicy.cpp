// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/SceneCullingDbvtPolicy.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/CollisionGroups.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)
#endif

namespace GTEngine
{
    struct WriteOCL
    {
        SIMD_FORCE_INLINE static bool Process(btScalar &q, btScalar v)
        {
            if (q < v)
            {
                q = v;
            }
            
            return false;
        }
    };

    struct QueryOCL
    {
        SIMD_FORCE_INLINE static bool Process(btScalar &q, btScalar v)
        {
            return q <= v;
        }
    };
}


namespace GTEngine
{
    SceneCullingDbvtPolicy::SceneCullingDbvtPolicy(const glm::mat4 &mvpIn, size_t bufferWidthIn, size_t bufferHeightIn)
        : mvp(),
          bufferWidth(bufferWidthIn), bufferHeight(bufferHeightIn),
          buffer()
    {
        /*
        for (int i = 0; i < 16; ++i)
        {
            this->mvp[i] = (&mvpIn[0][0])[i];
        }
        */

        
        this->mvp[0]  = mvpIn[0][0];
        this->mvp[1]  = mvpIn[0][1];
        this->mvp[2]  = mvpIn[0][2];
        this->mvp[3]  = mvpIn[0][3];

        this->mvp[4]  = mvpIn[1][0];
        this->mvp[5]  = mvpIn[1][1];
        this->mvp[6]  = mvpIn[1][2];
        this->mvp[7]  = mvpIn[1][3];

        this->mvp[ 8] = mvpIn[2][0];
        this->mvp[ 9] = mvpIn[2][1];
        this->mvp[10] = mvpIn[2][2];
        this->mvp[11] = mvpIn[2][3];

        this->mvp[12] = mvpIn[3][0];
        this->mvp[13] = mvpIn[3][1];
        this->mvp[14] = mvpIn[3][2];
        this->mvp[15] = mvpIn[3][3];
        

        /*
        this->mvp[0]  = mvpIn[0][0];
        this->mvp[1]  = mvpIn[1][0];
        this->mvp[2]  = mvpIn[2][0];
        this->mvp[3]  = mvpIn[3][0];

        this->mvp[4]  = mvpIn[0][1];
        this->mvp[5]  = mvpIn[1][1];
        this->mvp[6]  = mvpIn[2][1];
        this->mvp[7]  = mvpIn[3][1];

        this->mvp[ 8] = mvpIn[0][2];
        this->mvp[ 9] = mvpIn[1][2];
        this->mvp[10] = mvpIn[2][2];
        this->mvp[11] = mvpIn[3][2];

        this->mvp[12] = mvpIn[0][3];
        this->mvp[13] = mvpIn[1][3];
        this->mvp[14] = mvpIn[2][3];
        this->mvp[15] = mvpIn[3][3];
        */

	    buffer.resize(bufferWidth * bufferHeight, 0);
    }

    SceneCullingDbvtPolicy::~SceneCullingDbvtPolicy()
    {
    }


    bool SceneCullingDbvtPolicy::Descent(const btDbvtNode* node)
    {
        return this->QueryOccluder(node->volume.Center(), node->volume.Extents());
    }

    void SceneCullingDbvtPolicy::Process(const btDbvtNode* leaf)
    {
        auto proxy           = static_cast<btBroadphaseProxy*>(leaf->data);
        auto collisionObject = static_cast<btCollisionObject*>(proxy->m_clientObject);

        assert(proxy           != nullptr);
        assert(collisionObject != nullptr);


        auto sceneNode = static_cast<SceneNode*>(collisionObject->getUserPointer());
        if (sceneNode != nullptr && sceneNode->IsVisible())
        {
            // We check the objects collision group to determine the type of the component we're retrieving.
            if (proxy->m_collisionFilterGroup & CollisionGroups::Occluder)
            {
                // We need to append the occluder. We're only supporting box shapes here, and we'll be upcasting with that assumption.
                auto shape = static_cast<btBoxShape*>(collisionObject->getCollisionShape());
                if (shape != nullptr)
                {
                    this->AppendOccluder(shape->getHalfExtentsWithoutMargin(), collisionObject->getWorldTransform());
                }
            }
            else if (proxy->m_collisionFilterGroup & CollisionGroups::Model)
            {
                auto modelComponent = sceneNode->GetComponent<ModelComponent>();
                assert(modelComponent != nullptr);

                this->ProcessModel(*modelComponent);
            }
            else if (proxy->m_collisionFilterGroup & CollisionGroups::PointLight)
            {
                auto pointLightComponent = sceneNode->GetComponent<PointLightComponent>();
                assert(pointLightComponent != nullptr);

                this->ProcessPointLight(*pointLightComponent);
            }
            else if (proxy->m_collisionFilterGroup & CollisionGroups::SpotLight)
            {
                auto spotLightComponent = sceneNode->GetComponent<SpotLightComponent>();
                assert(spotLightComponent != nullptr);

                this->ProcessSpotLight(*spotLightComponent);
            }
        }
    }


    bool SceneCullingDbvtPolicy::QueryOccluder(const btVector3 &c, const btVector3 &e)
    {
        btVector4 x[] =
        {
            this->Transform(btVector3(c[0] - e[0], c[1] - e[1], c[2] - e[2])),
            this->Transform(btVector3(c[0] + e[0], c[1] - e[1], c[2] - e[2])),
            this->Transform(btVector3(c[0] + e[0], c[1] + e[1], c[2] - e[2])),
            this->Transform(btVector3(c[0] - e[0], c[1] + e[1], c[2] - e[2])),
            this->Transform(btVector3(c[0] - e[0], c[1] - e[1], c[2] + e[2])),
            this->Transform(btVector3(c[0] + e[0], c[1] - e[1], c[2] + e[2])),
            this->Transform(btVector3(c[0] + e[0], c[1] + e[1], c[2] + e[2])),
            this->Transform(btVector3(c[0] - e[0], c[1] + e[1], c[2] + e[2]))
        };

        for(int i = 0; i < 8; ++i)
        {
            if ((x[i][2] + x[i][3]) <= 0)
            {
                return true;
            }
        }

        const int d[] = 
        {
            1, 0, 3, 2,
            4, 5, 6, 7,
            4, 7, 3, 0,
            6, 5, 1, 2,
            7, 6, 2, 3,
            5, 4, 0, 1
        };

        for(unsigned int i = 0; i < (sizeof(d) / sizeof(d[0])); )
        {
            btVector4 p[] = 
            {
                x[d[i++]],
                x[d[i++]],
                x[d[i++]],
                x[d[i++]]
            };

            if (this->ClipDraw<4, QueryOCL>(p, 0.0f))
            {
                return true;
            }
        }

        return false;
    }

    void SceneCullingDbvtPolicy::AppendOccluder(const btVector3 &halfExtents, const btTransform &transform)
    {
        auto  c     = transform.getOrigin();
        auto &basis = transform.getBasis();
        auto &e     = halfExtents;

        btVector4 x[] = 
        {
            this->Transform(c+basis*btVector3(-e[0], -e[1], -e[2])),
            this->Transform(c+basis*btVector3(+e[0], -e[1], -e[2])),
            this->Transform(c+basis*btVector3(+e[0], +e[1], -e[2])),
            this->Transform(c+basis*btVector3(-e[0], +e[1], -e[2])),
            this->Transform(c+basis*btVector3(-e[0], -e[1], +e[2])),
            this->Transform(c+basis*btVector3(+e[0], -e[1], +e[2])),
            this->Transform(c+basis*btVector3(+e[0], +e[1], +e[2])),
            this->Transform(c+basis*btVector3(-e[0], +e[1], +e[2]))
        };

        const int d[] = 
        {
            1,0,3,2,
            4,5,6,7,
            4,7,3,0,
            6,5,1,2,
            7,6,2,3,
            5,4,0,1
        };

        for (unsigned int i = 0; i < (sizeof(d) / sizeof(d[0])); )
        {
            btVector4 p[] = 
            {
                x[d[i++]],
                x[d[i++]],
                x[d[i++]],
                x[d[i++]]
            };

            this->ClipDraw<4, WriteOCL>(p, 0.0f);
        }
    }

    btVector4 SceneCullingDbvtPolicy::Transform(const btVector3 &v)
    {
        btVector4 t;
        t[0] = v[0]*mvp[0] + v[1]*mvp[4] + v[2]*mvp[ 8] + mvp[12];
        t[1] = v[0]*mvp[1] + v[1]*mvp[5] + v[2]*mvp[ 9] + mvp[13];
        t[2] = v[0]*mvp[2] + v[1]*mvp[6] + v[2]*mvp[10] + mvp[14];
        t[3] = v[0]*mvp[3] + v[1]*mvp[7] + v[2]*mvp[11] + mvp[15];

        return t;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
