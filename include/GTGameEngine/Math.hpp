// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.
//
// Much of the code is based on the GLM project: http://glm.g-truc.net/0.9.6/index.html

#ifndef __GT_Engine_Math_hpp_
#define __GT_Engine_Math_hpp_

#include "Config.hpp"
#include <GTLib/Math.hpp>
#include <math.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4201)   // nonstandard extension used : nameless struct/union
#endif


// Technically we should be checking 
#define GT_USE_SSE2

#if defined(GT_USE_SSE2)

#if defined(__MINGW32__)
#include <intrin.h>
#endif

#include <emmintrin.h>
#endif

namespace GT
{
    /// Converts the given value in degrees to radians.
    ///
    /// @param degrees [in] The degrees to convert to radians.
    inline float Radians(float degrees)
    {
        return degrees * static_cast<float>(0.01745329251994329576923690768489);
    }

    /// Converts the given value in radians to degrees.
    ///
    /// @param radians [in] The radians to convert to degrees.
    inline float Degrees(float radians)
    {
        return radians * static_cast<float>(57.295779513082320876798154814105);
    }



    //////////////////////////////////
    // vec4

    struct vec4;

    inline vec4 operator+(const vec4 &v0, const vec4 &v1);
    inline vec4 operator+(const vec4 &v0, float v1);
    inline vec4 operator+(float v0, const vec4 &v1);

    inline vec4 operator-(const vec4 &v0, const vec4 &v1);
    inline vec4 operator-(const vec4 &v0, float v1);
    inline vec4 operator-(float v0, const vec4 &v1);

    inline vec4 operator*(const vec4 &v0, const vec4 &v1);
    inline vec4 operator*(const vec4 &v0, float v1);
    inline vec4 operator*(float v0, const vec4 &v1);

    inline vec4 operator/(const vec4 &v0, const vec4 &v1);
    inline vec4 operator/(const vec4 &v0, float v1);
    inline vec4 operator/(float v0, const vec4 &v1);

    /// Structure representing a vec4/float4.
    ALIGNED_TYPE(struct, 16) vec4
    {
        union
        {
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };

#if defined(GT_USE_SSE2)
            __m128 xyzw;
#endif
        };


        vec4()
        {
        }

        vec4(const vec4 &other)
            : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        vec4(float _x, float _y, float _z, float _w)
            : x(_x), y(_y), z(_z), w(_w)
        {
        }


        vec4 & operator=(const vec4 &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;

            return *this;
        }

        float & operator[](int index)
        {
            return *(&x + index);
        }

        float operator[](int index) const
        {
            return *(&x + index);
        }


        vec4 & operator+=(const vec4 &other)
        {
            return (*this) = (*this) + other;
        }
        vec4 & operator+=(float other)
        {
            return (*this) = (*this) + other;
        }
        
        vec4 & operator-=(const vec4 &other)
        {
            return (*this) = (*this) - other;
        }
        vec4 & operator-=(float other)
        {
            return (*this) = (*this) - other;
        }
        
        vec4 & operator*=(const vec4 &other)
        {
            return (*this) = (*this) * other;
        }
        vec4 & operator*=(float other)
        {
            return (*this) = (*this) * other;
        }

        vec4 & operator/=(const vec4 &other)
        {
            return (*this) = (*this) / other;
        }
        vec4 & operator/=(float other)
        {
            return (*this) = (*this) / other;
        }


        ///////////////////////////////////
        // Static Members

        static const vec4 zero;
        static const vec4 one;
        static const vec4 posx;
        static const vec4 posy;
        static const vec4 posz;
        static const vec4 posw;
        static const vec4 negx;
        static const vec4 negy;
        static const vec4 negz;
        static const vec4 negw;
    };

    /// Performs an addition on the given vector.
    inline vec4 add(const vec4 &v0, const vec4 &v1)
    {
        return vec4(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w);
    }
    inline vec4 add(const vec4 &v0, float v1)
    {
        return vec4(v0.x + v1, v0.y + v1, v0.z + v1, v0.w + v1);
    }


    /// Performs a subtraction on the given vector.
    inline vec4 sub(const vec4 &v0, const vec4 &v1)
    {
        return vec4(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w);
    }
    inline vec4 sub(const vec4 &v0, float v1)
    {
        return vec4(v0.x - v1, v0.y - v1, v0.z - v1, v0.w - v1);
    }
    inline vec4 sub(float v0, const vec4 &v1)
    {
        return vec4(v0 - v1.x, v0 - v1.y, v0 - v1.z, v0 - v1.w);
    }


    /// Performs a multiplication on the given vector.
    inline vec4 mul(const vec4 &v0, const vec4 &v1)
    {
        return vec4(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z, v0.w * v1.w);
    }
    inline vec4 mul(const vec4 &v0, float v1)
    {
        return vec4(v0.x * v1, v0.y * v1, v0.z * v1, v0.w * v1);
    }


    /// Performs a division on the given vector.
    inline vec4 div(const vec4 &v0, const vec4 &v1)
    {
        return vec4(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z, v0.w / v1.w);
    }
    inline vec4 div(const vec4 &v0, float v1)
    {
        return vec4(v0.x / v1, v0.y / v1, v0.z / v1, v0.w / v1);
    }
    inline vec4 div(float v0, const vec4 &v1)
    {
        return vec4(v0 / v1.x, v0 / v1.y, v0 / v1.z, v0 / v1.w);
    }


    /// Calculates the squared length of a vector.
    inline float length2(const vec4 &v)
    {
        return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
    }

    /// Calculates the length of a vector.
    inline float length(const vec4 &v)
    {
        return ::sqrtf(length2(v));
    }

    /// Returns the normalization of the given vector.
    inline vec4 normalize(const vec4 &v)
    {
        return div(v, length(v));
    }


    /// Performs a dot product between two vectors.
    inline float dot(const vec4 &v0, const vec4 &v1)
    {
        return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z + v0.w*v1.w;
    }

    /// Performs a cross product between two vectors.
    ///
    /// The W component is ignored and set to 0 in the returned vector.
    inline vec4 cross(const vec4 &v0, const vec4 &v1)
    {
        return vec4(
            v0.y*v1.z - v0.z*v1.y,
            v0.z*v1.x - v0.x*v1.z,
            v0.x*v1.y - v0.y*v1.x,
            0.0f);
    }


    //////////////////////////
    // Operator Overloads

    inline vec4 operator+(const vec4 &v0, const vec4 &v1)
    {
        return add(v0, v1);
    }
    inline vec4 operator+(const vec4 &v0, float v1)
    {
        return add(v0, v1);
    }
    inline vec4 operator+(float v0, const vec4 &v1)
    {
        return add(v1, v0);
    }

    inline vec4 operator-(const vec4 &v0, const vec4 &v1)
    {
        return sub(v0, v1);
    }
    inline vec4 operator-(const vec4 &v0, float v1)
    {
        return sub(v0, v1);
    }
    inline vec4 operator-(float v0, const vec4 &v1)
    {
        return sub(v0, v1);
    }

    inline vec4 operator*(const vec4 &v0, const vec4 &v1)
    {
        return mul(v0, v1);
    }
    inline vec4 operator*(const vec4 &v0, float v1)
    {
        return mul(v0, v1);
    }
    inline vec4 operator*(float v0, const vec4 &v1)
    {
        return mul(v1, v0);
    }

    inline vec4 operator/(const vec4 &v0, const vec4 &v1)
    {
        return div(v0, v1);
    }
    inline vec4 operator/(const vec4 &v0, float v1)
    {
        return div(v0, v1);
    }
    inline vec4 operator/(float v0, const vec4 &v1)
    {
        return div(v0, v1);
    }

    inline bool operator==(const vec4 &v0, const vec4 &v1)
    {
        return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z && v0.w == v1.w;
    }
    inline bool operator!=(const vec4 &v0, const vec4 &v1)
    {
        return !(v0 == v1);
    }



    //////////////////////////////////
    // quat

    /// Structure representing a quaternion.
    ALIGNED_TYPE(struct, 16) quat
    {
        union
        {
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };

#if defined(GT_USE_SSE2)
            __m128 xyzw;
#endif
        };


        quat()
        {
        }

        quat(const quat &other)
            : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        quat(float _x, float _y, float _z, float _w)
            : x(_x), y(_y), z(_z), w(_w)
        {
        }


        quat & operator=(const quat &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;

            return *this;
        }

        float & operator[](int index)
        {
            return *(&x + index);
        }

        float operator[](int index) const
        {
            return *(&x + index);
        }


        /// Creates a quaternion from an angle rotated around the given axis.
        static quat angle_axis(float angle, float axisX, float axisY, float axisZ)
        {
            const float a = angle;
		    const float s = sin(a * 0.5f);

            return quat(
                cos(a * 0.5f),
                axisX * s,
                axisY * s,
                axisZ * s
            );
        }



        ///////////////////////////////////
        // Static Members

        static const quat identity;
    };


    //////////////////////////
    // Operator Overloads

    inline bool operator==(const quat &v0, const quat &v1)
    {
        return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z && v0.w == v1.w;
    }
    inline bool operator!=(const quat &v0, const quat &v1)
    {
        return !(v0 == v1);
    }




    //////////////////////////////////
    // mat4

    /// Structure representing a column-major mat4
    ALIGNED_TYPE(struct, 16) mat4
    {
        vec4 col0;
        vec4 col1;
        vec4 col2;
        vec4 col3;

        mat4()
            : col0(), col1(), col2(), col3()
        {
        }

        mat4(const mat4 &other)
            : col0(other.col0), col1(other.col1), col2(other.col2), col3(other.col3)
        {
        }

        mat4(const vec4 &_col0, const vec4 &_col1, const vec4 &_col2, const vec4 &_col3)
            : col0(_col0), col1(_col1), col2(_col2), col3(_col3)
        {
        }


        vec4 & operator[](int index)
        {
            return *(&col0 + index);
        }

        vec4 operator[](int index) const
        {
            return *(&col0 + index);
        }


        /// Creates a perspective matrix.
        ///
        /// @param fovy   [in] The field of view.
        /// @param aspect [in] The aspect ratio.
        /// @param znear  [in] The near plane.
        /// @param zfar   [in] The far plane.
        ///
        /// @return The perspective projection matrix.
        static mat4 perspective(float fovy, float aspect, float znear, float zfar)
        {
            float const tanHalfFovy = tan(fovy * 0.5f);

		    mat4 result(mat4::zero);
            result[0][0] = 1.0f / (aspect * tanHalfFovy);
            result[1][1] = 1.0f / (tanHalfFovy);
            result[2][2] = -(zfar + znear) / (zfar - znear);
            result[2][3] = -1.0f;
            result[3][2] = -(2.0f * zfar * znear) / (zfar - znear);
            
            return result;
        }

        /// Creates an orthographic projection matrix.
        ///
        /// @param left   [in] The left vertical clipping plane.
        /// @param right  [in] The right vertical clipping plane.
        /// @param bottom [in] The bottom horizontal clipping plane.
        /// @param top    [in] The top horizontal clipping plane.
        /// @param znear  [in] The distance to the near depth clipping plane.
        /// @param zfar   [in] The distance to the far depth clipping plane.
        ///
        /// @return The orthographics projection matrix.
        static mat4 ortho(float left, float right, float bottom, float top, float znear, float zfar)
        {
            mat4 result(mat4::identity);
            result[0][0] =  2.0f / (right - left);
            result[1][1] =  2.0f / (top   - bottom);
            result[2][2] = -2.0f / (zfar  - znear);
            result[3][0] = -(right + left)   / (right - left);
            result[3][1] = -(top   + bottom) / (top   - bottom);
            result[3][2] = -(zfar  + znear)  / (zfar  - znear);

            return result;
        }


        /// Creates a copy of the given matrix, translated by the given amount.
        ///
        /// @param m [in] The matrix to translate.
        /// @param v [in] The translation vector.
        ///
        /// @remarks
        ///     The w component of the translation vector is ignored.
        static mat4 translate(const mat4 &m, const vec4 &v)
        {
            mat4 result(m);
            result[3] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2] + m[3];

            return result;
        }

        /// Creates a copy of the given matrix, rotated by the given angle and axis.
        ///
        /// @param m     [in] The matrix to rotate.
        /// @param angle [in] The angle to rotate, in radians.
        /// @param axis  [in] The axis to rotate around.
        ///
        /// @remarks
        ///     The w component of the rotation axis is ignored.
        ///     @par
        ///     \c axis should be normalized.
        static mat4 rotate(const mat4 &m, float angle, const vec4 &axis)
        {
            const float a = angle;
		    const float c = cos(a);
		    const float s = sin(a);

		    vec4 temp((1.0f - c) * axis);

		    mat4 rotation;
		    rotation[0][0] = c + temp[0] * axis[0];
		    rotation[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
		    rotation[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

		    rotation[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
		    rotation[1][1] = c + temp[1] * axis[1];
		    rotation[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

		    rotation[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
		    rotation[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
		    rotation[2][2] = c + temp[2] * axis[2];

		    mat4 result;
		    result[0] = m[0] * rotation[0][0] + m[1] * rotation[0][1] + m[2] * rotation[0][2];
		    result[1] = m[0] * rotation[1][0] + m[1] * rotation[1][1] + m[2] * rotation[1][2];
		    result[2] = m[0] * rotation[2][0] + m[1] * rotation[2][1] + m[2] * rotation[2][2];
		    result[3] = m[3];

            return result;
        }

        /// Creates a copy of the given matrix, scaled by the given vector.
        ///
        /// @param m [in] The matrix to scale.
        /// @param v [in] The scale vector.
        ///
        /// @remarks
        ///     The w component of the scaling vector is ignored.
        static mat4 scale(const mat4 &m, const vec4 &v)
        {
            mat4 result;
            result[0] = m[0] * v[0];
		    result[1] = m[1] * v[1];
		    result[2] = m[2] * v[2];
		    result[3] = m[3];

		    return result;
        }


        ///////////////////////////////////
        // Static Members

        static const mat4 zero;
        static const mat4 identity;
    };


    /// Converts a quaternion to a matrix.
    inline mat4 quat_to_mat4(const quat &q)
    {
        mat4 result;

		float qxx(q.x * q.x);
		float qyy(q.y * q.y);
		float qzz(q.z * q.z);
		float qxz(q.x * q.z);
		float qxy(q.x * q.y);
		float qyz(q.y * q.z);
		float qwx(q.w * q.x);
		float qwy(q.w * q.y);
		float qwz(q.w * q.z);

		result[0][0] = 1.0f - 2.0f * (qyy +  qzz);
		result[0][1] = 2.0f * (qxy + qwz);
		result[0][2] = 2.0f * (qxz - qwy);
        result[0][3] = 0.0f;

		result[1][0] = 2.0f * (qxy - qwz);
		result[1][1] = 1.0f - 2.0f * (qxx +  qzz);
		result[1][2] = 2.0f * (qyz + qwx);
        result[1][3] = 0.0f;

		result[2][0] = 2.0f * (qxz + qwy);
		result[2][1] = 2.0f * (qyz - qwx);
		result[2][2] = 1.0f - 2.0f * (qxx +  qyy);
        result[2][3] = 0.0f;

        result[3][0] = 0.0f;
        result[3][1] = 0.0f;
        result[3][2] = 0.0f;
        result[3][3] = 1.0f;

		return result;
    }

    /// Converts a matrix to a quaternion
    inline quat mat4_to_quat(const mat4 &m)
    {
        float fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
		float fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
		float fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
		float fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

		int biggestIndex = 0;
		float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}

		float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
		float mult = 0.25f / biggestVal;

		quat result;
		switch(biggestIndex)
		{
		case 0:
            {
                result.w = biggestVal;
			    result.x = (m[1][2] - m[2][1]) * mult;
			    result.y = (m[2][0] - m[0][2]) * mult;
			    result.z = (m[0][1] - m[1][0]) * mult;

			    break;
            }

		case 1:
            {
                result.w = (m[1][2] - m[2][1]) * mult;
			    result.x = biggestVal;
			    result.y = (m[0][1] + m[1][0]) * mult;
			    result.z = (m[2][0] + m[0][2]) * mult;

			    break;
            }
            
		case 2:
            {
                result.w = (m[2][0] - m[0][2]) * mult;
			    result.x = (m[0][1] + m[1][0]) * mult;
			    result.y = biggestVal;
			    result.z = (m[1][2] + m[2][1]) * mult;
			    break;
            }
			
		case 3:
            {
                result.w = (m[0][1] - m[1][0]) * mult;
			    result.x = (m[2][0] + m[0][2]) * mult;
			    result.y = (m[1][2] + m[2][1]) * mult;
			    result.z = biggestVal;

			    break;
            }
			
			
		default:					// Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
			assert(false);
			break;
		}

		return result;
    }




    ///////////////////////////////////////////
    // Helper Utilities

    inline mat4 CalculateTransformMatrix(const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        mat4 result;

        result = GT::quat_to_mat4(rotation);

        result[3][0] = position.x;
        result[3][1] = position.y;
        result[3][2] = position.z;

        result[0] *= scale.x;
        result[1] *= scale.y;
        result[2] *= scale.z;

        return result;
    }


    inline float Distance(float x1, float y1, float x2, float y2)
    {
        float diffX = x1 - x2;
        float diffY = y1 - y2;
        return abs(sqrt(diffX*diffX + diffY*diffY));
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
