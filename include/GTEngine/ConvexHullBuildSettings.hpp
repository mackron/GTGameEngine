
#ifndef __GTEngine_ConvexHullBuildSettings_hpp_
#define __GTEngine_ConvexHullBuildSettings_hpp_

namespace GTEngine
{
    /// Structure containing settings to use when building convex hulls.
    struct ConvexHullBuildSettings
    {
        ConvexHullBuildSettings()
            : compacityWeight(0.1f),
              volumeWeight(0.0f),
              minClusters(100),
              verticesPerCH(100),
              concavity(1000.0f),
              smallClusterThreshold(0.1f),
              connectedComponentsDist(30.0f),
              simplifiedTriangleCountTarget(0),
              addExtraDistPoints(true),
              addFacesPoints(true)
        {
        }


        float        compacityWeight;
        float        volumeWeight;
        unsigned int minClusters;
        unsigned int verticesPerCH;
        float        concavity;
        float        smallClusterThreshold;
        float        connectedComponentsDist;
        unsigned int simplifiedTriangleCountTarget;
        bool         addExtraDistPoints;
        bool         addFacesPoints;
    };
}

#endif
