// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_SceneCullingDbvtPolicy_hpp_
#define __GTEngine_SceneCullingDbvtPolicy_hpp_

#include "Physics.hpp"
#include <GTLib/Vector.hpp>

namespace GTEngine
{
    class SceneNode;
    class ModelComponent;
    class PointLightComponent;
    class SpotLightComponent;

    /// The Dbvt collision policy to use for scene node culling.
    ///
    /// Basically, this is used for finding visible scene nodes during occlusion and frustum culling.
    ///
    /// This will only include scene nodes that are actually visible.
    ///
    /// This assumes the user data of the collision objects are a SceneNode object.
    ///
    /// Much of the code here is taken directly from the Bullet samples. Anything uncommented means I've got no idea what the hell is going on...
    struct SceneCullingDbvtPolicy : btDbvt::ICollide
    {
        /// Constructor.
        SceneCullingDbvtPolicy(const glm::mat4 &mvp, size_t bufferWidth = 128, size_t bufferHeight = 128);

        /// Destructor.
        virtual ~SceneCullingDbvtPolicy();


        /// Virtual function that will be called for every visible model component.
        virtual void ProcessModel(ModelComponent &) {}

        /// Virtual function that will be called for every visible point light component.
        virtual void ProcessPointLight(PointLightComponent &) {}

        /// Virtual function taht will be called for every visible spot light component.
        virtual void ProcessSpotLight(SpotLightComponent &) {}



        /// btDbvt::ICollide::Descent()
        bool Descent(const btDbvtNode* node);

        /// btDbvt::ICollide::Process()
        void Process(const btDbvtNode* node, btScalar) { this->Process(node); }
        void Process(const btDbvtNode* leaf);



    private:

        /// Appends an occluder using the half extents and transformation.
        ///
        /// @param halfExtents [in] The half extents of the occluder.
        /// @param transform   [in] The transformation to apply to the extents.
        void AppendOccluder(const btVector3 &halfExtents, const btTransform &worldTransform);

        /// Determines whether or not the given AABB is occluded.
        ///
        /// @param center  [in] The center point of the AABB.
        /// @param extents [in] The extents of the AABB.
        bool QueryOccluder(const btVector3 &center, const btVector3 &extents);

        
        /// Transforms the given point the model-view-projection matrix given during construction.
        ///
        /// @param v [in] A reference to the vector to transform.
        ///
        /// @return The transformed vector.
        btVector4 Transform(const btVector3 &v);


        static bool	Project(btVector4* p, int n)
        {
            for(int i = 0; i < n; ++i)
            {
                //btScalar iw = 1 / p[i][3];
                p[i][2]  = 1 / p[i][3];
                p[i][0] *=     p[i][2];
                p[i][1] *=     p[i][2];
            }

            return true;
        }


        /// Below are rasterizing methods.
        template <int NP>
        inline int Clip(const btVector4* pi, btVector4* po)
        {
            btScalar s[NP];
            int      m = 0;

            for(int i = 0; i < NP; ++i)
	        {
	            s[i] = pi[i][2] + pi[i][3];

	            if (s[i] < 0)
                {
                    m += 1 << i;
                }
	        }

            if (m == ((1 << NP) - 1))
            {
                return(0);
            }

            if(m != 0)
            {
                int n=0;
                for (int i = NP - 1, j = 0; j < NP; i = j++)
                {
                    const btVector4& a = pi[i];
                    const btVector4& b = pi[j];
                    const btScalar   t = s[i] / (a[3] + a[2] - b[3] - b[2]);

                    if ((t > 0) && (t < 1))
                    {
                        po[n][0] = a[0] + (b[0] - a[0]) * t;
                        po[n][1] = a[1] + (b[1] - a[1]) * t;
                        po[n][2] = a[2] + (b[2] - a[2]) * t;
                        po[n][3] = a[3] + (b[3] - a[3]) * t;
                        ++n;
                    }

                    if (s[j] > 0)
                    {
                        po[n++] = b;
                    }
                }

                return n;
            }

            for (int i = 0; i < NP; ++i)
            {
                po[i] = pi[i];
            }

            return NP;
        }

        template <typename POLICY>
        inline bool	Draw(const btVector4 &a, const btVector4 &b, const btVector4 &c, const btScalar minarea)
        {
            btScalar a2 = ((b -a).cross(c - a))[2];
            if (a2 > 0)
            {
                if (a2 < minarea)
                {
                    return true;
                }

                int sizes[2] =
                {
                    static_cast<int>(this->bufferWidth),
                    static_cast<int>(this->bufferHeight)
                };

                btScalar scales[2] =
                {
                    sizes[0] * 0.5f,
                    sizes[1] * 0.5f
                };

                btScalar offsets[2] =
                {
                    scales[0] + 0.5f,
                    scales[1] + 0.5f
                };

                int x[] =
                {
                    static_cast<int>(a.x() * scales[0] + offsets[0]),
                    static_cast<int>(b.x() * scales[0] + offsets[0]),
                    static_cast<int>(c.x() * scales[0] + offsets[0])
                };

                int y[] =
                {
                    static_cast<int>(a.y() * scales[1] + offsets[1]),
                    static_cast<int>(b.y() * scales[1] + offsets[1]),
                    static_cast<int>(c.y() * scales[1] + offsets[1])
                };

                btScalar z[] =
                {
                    a.z(),
                    b.z(),
                    c.z()
                };

                int mix    = btMax(0,            btMin(x[0], btMin(x[1], x[2])));
                int mxx    = btMin(sizes[0], 1 + btMax(x[0], btMax(x[1], x[2])));
                int miy    = btMax(0,            btMin(y[0], btMin(y[1], y[2])));
                int mxy    = btMin(sizes[1], 1 + btMax(y[0], btMax(y[1], y[2])));
                int width  = mxx - mix;
                int height = mxy - miy;

                if ((width * height) > 0)
                {
                    int dx[] =
                    {
                        y[0] - y[1],
                        y[1] - y[2],
                        y[2] - y[0]
                    };

                    int dy[] =
                    {
                        x[1] - x[0] - dx[0]*width,
                        x[2] - x[1] - dx[1]*width,
                        x[0] - x[2] - dx[2]*width
                    };

                    int      a   = x[2]*y[0] + x[0]*y[1] - x[2]*y[1] - x[0]*y[2] + x[1]*y[2] - x[1]*y[0];
                    btScalar ia  = 1.0f / static_cast<btScalar>(a);
                    btScalar dzx = ia * (y[2]*(z[1] - z[0]) + y[1]*(z[0] - z[2]) + y[0]*(z[2] - z[1]));
                    btScalar dzy = ia * (x[2]*(z[0] - z[1]) + x[0]*(z[1] - z[2]) + x[1]*(z[2] - z[0])) - (dzx * width);		

                    int c[] =
                    {
                        miy*x[1] + mix*y[0] - x[1]*y[0] - mix*y[1] + x[0]*y[1] - miy*x[0],
                        miy*x[2] + mix*y[1] - x[2]*y[1] - mix*y[2] + x[1]*y[2] - miy*x[1],
                        miy*x[0] + mix*y[2] - x[0]*y[2] - mix*y[0] + x[2]*y[0] - miy*x[2]
                    };

                    btScalar  v    = ia * ((z[2]*c[0]) + (z[0]*c[1]) + (z[1]*c[2]));
                    btScalar* scan = &buffer[miy * sizes[1]];

                    for (int iy = miy; iy < mxy; ++iy)
                    {
                        for (int ix = mix; ix < mxx; ++ix)
                        {
                            if ((c[0] >= 0) && (c[1] >=0 ) && (c[2] >= 0))
                            {
                                if(POLICY::Process(scan[ix], v))
                                {
                                    return true;
                                }
                            }

                            c[0]+=dx[0];
                            c[1]+=dx[1];
                            c[2]+=dx[2];

                            v   +=dzx;
                        }

                        c[0] += dy[0];
                        c[1] += dy[1];
                        c[2] += dy[2];

                        v    += dzy;
                        scan += sizes[0];
                    }
                }
            }

            return false;
        }



        /// Not sure what this does yet...
        template <int NP, typename POLICY>
        inline bool	ClipDraw(const btVector4* p, btScalar minarea)
        {
            btVector4 o[NP*2];
            int  n         = this->Clip<NP>(p, o);
            bool earlyexit = false;

            this->Project(o, n);

            for(int i = 2; i < n; ++i)
	        {
	            earlyexit |= Draw<POLICY>(o[0], o[i-1], o[i], minarea);
	        }

            return earlyexit;
        }


    private:

        /// The model-view-projection matrix to use with the culling.
        btScalar mvp[16];

        /// The width of the occlusion buffer.
        size_t bufferWidth;

        /// The height of the occlusion buffer.
        size_t bufferHeight;

        /// The buffer containing "pixel" data from rasterizing.
        btAlignedObjectArray<btScalar> buffer;
    };
}

#endif