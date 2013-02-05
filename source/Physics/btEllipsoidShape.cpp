// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Physics/btEllipsoidShape.hpp>

btEllipsoidShape::btEllipsoidShape(const btVector3 &semiAxes)
    : btConvexInternalShape()
{
    m_shapeType               = CUSTOM_CONVEX_SHAPE_TYPE;
    m_implicitShapeDimensions = semiAxes - btVector3(getMargin(), getMargin(), getMargin());
}

void btEllipsoidShape::calculateLocalInertia(btScalar mass, btVector3 &inertia) const
{
    btVector3 abc2 = m_implicitShapeDimensions + btVector3(getMargin(), getMargin(), getMargin());
    abc2 *= abc2;

    btScalar a2 = abc2[0];
    btScalar b2 = abc2[1];
    btScalar c2 = abc2[2];

    inertia.setValue(
        btScalar(0.2) * (mass * (b2 + c2)),
        btScalar(0.2) * (mass * (a2 + c2)),
        btScalar(0.2) * (mass * (a2 + b2)));
}

btVector3 btEllipsoidShape::localGetSupportingVertexWithoutMargin(const btVector3 &vec0) const
{
	btVector3 vec    = vec0;
	btScalar  lenSqr = vec.length2();

	if (lenSqr < (SIMD_EPSILON*SIMD_EPSILON))
	{
		vec.setValue(1,0,0);
	}
    else
	{
        vec.normalize();
	}

    vec *= m_implicitShapeDimensions;
    vec  = vec * (m_implicitShapeDimensions / vec.length());

    return vec;
}

void btEllipsoidShape::batchedUnitVectorGetSupportingVertexWithoutMargin(const btVector3* vectors, btVector3* supportVerticesOut, int numVectors) const
{
    for (int j = 0; j < numVectors; j++)
    {
        btVector3 vec = vectors[j] * m_implicitShapeDimensions;

        supportVerticesOut[j] = vec * (m_implicitShapeDimensions / vec.length());
    }
}


void btEllipsoidShape::setMargin(btScalar collisionMargin)
{
    //correct the m_implicitShapeDimensions for the margin
    btVector3 oldMargin(getMargin(), getMargin(), getMargin());
    btVector3 implicitShapeDimensionsWithMargin = m_implicitShapeDimensions + oldMargin;
		
    btConvexInternalShape::setMargin(collisionMargin);
    btVector3 newMargin(getMargin(), getMargin(), getMargin());
    m_implicitShapeDimensions = implicitShapeDimensionsWithMargin - newMargin;
}

void btEllipsoidShape::setLocalScaling(const btVector3 &scaling)
{
	btVector3 oldMargin(getMargin(), getMargin(), getMargin());
	btVector3 implicitShapeDimensionsWithMargin         = m_implicitShapeDimensions + oldMargin;
	btVector3 unScaledImplicitShapeDimensionsWithMargin = implicitShapeDimensionsWithMargin / m_localScaling;

	btConvexInternalShape::setLocalScaling(scaling);

	m_implicitShapeDimensions = (unScaledImplicitShapeDimensionsWithMargin * m_localScaling) - oldMargin;
}

void btEllipsoidShape::getAabb(const btTransform& t, btVector3& aabbMin, btVector3& aabbMax) const
{
    btVector3 halfExtents = m_implicitShapeDimensions;
    halfExtents += btVector3(getMargin(), getMargin(), getMargin());

    btMatrix3x3 abs_b  = t.getBasis().absolute();  
    btVector3   center = t.getOrigin();
    btVector3   extent = btVector3(abs_b[0].dot(halfExtents), abs_b[1].dot(halfExtents), abs_b[2].dot(halfExtents));		  
			
    aabbMin = center - extent;
    aabbMax = center + extent;
}
