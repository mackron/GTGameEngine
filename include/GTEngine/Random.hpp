
#ifndef __GTEngine_Random_hpp_
#define __GTEngine_Random_hpp_

#include "Math.hpp"
#include <cstdlib>		// For srand() and rand().
#include <ctime>		// For time().

namespace GTEngine
{
    /// Determines if the random number generator has been seeded.
    extern bool g_haveSeeded;

    /// Seeds the random number generator.
    template <typename T>
    void SeedRandom(T seed)
    {
        srand(static_cast<unsigned int>(seed));
        g_haveSeeded = true;
    }

    /**
    *   \brief           Generates a random number between two values.
    *   \param	min [in] The minimum value.
    *   \param	max [in] The maximum value. This is inclusive.
    *   \return          A random number between \c min and \c max.
    *
    *   \remarks
    *       This generalised template is only good for integral numbers.
    *       \par
    *       The \c max parameter is inclusive. That is, the value of max
    *       can potentially be the number returned.
    */
    template <typename T>
    T GenerateRandomNumber(T min, T max)
    {
        // If we havn't seeded the random number generator, we'll do it now.
        // This only needs to be done once.
        if (!g_haveSeeded)
        {
            SeedRandom(time(0));
        }

        // Generate the actual random number. The "max - min" part ensures
        // the the number does not go above the maximum, while the "+ min"
        // part ensure that the number will never be below the minimum.
        return static_cast<T>((((float)rand() / RAND_MAX) * ((max - min) + 1))) + min;
    }

    /**
    *	\brief           Generates a random number between two values.
    *	\param	min [in] The minimum value.
    *	\param	max	[in] The maximum value. This is inclusive.
    *	\return		     A random number between \c min and \c max.
    *
    *	\remarks
    *		This will produce a float32 to 4 decimal places.
    *		\par
    *		The \c max parameter is inclusive. That is, the value of max
    *		can potentially be the number returned.
    */
    template <> float GenerateRandomNumber(float min, float max);

    /**
    *   \brief           Generates a random number between two values.
    *   \param	min [in] The minimum value.
    *   \param	max [in] The maximum value. This is inclusive.
    *   \return          A random number between \c min and \c max.
    *
    *   \remarks
    *       This will produce a float64 to 4 decimal places.
    *       \par
    *       The \c max parameter is inclusive. That is, the value of max
    *       can potentially be the number returned.
    */
    template <> double GenerateRandomNumber(double min, double max);

    /**
    *   \brief              Generates a random \c vec2 inside the bounds of a circle.
    *   \param	radius [in] The radius of the circle.
    *   \return             A vec2 that is inside the bounds of the specified circle.
    */
    glm::vec2 GenerateRandomNumberCircle(float radius);

    /**
    *   \brief              Generates a random \c vec3 inside the bounds of a sphere.
    *   \param  radius [in] The radius of the sphere.
    *   \return             A vec3 that is inside the bounds of the specified sphere.
    */
    glm::vec3 GenerateRandomNumberSphere(float radius);

    /**
    *   \brief              Generates a random \c vec2 inside the bounds of a 2D ellipse.
    *   \param  width  [in] The width of the ellipse.
    *   \param  height [in] The height of the ellipse.
    *   \return             A vec2 that is inside the bounds of the specified ellipse.
    */
    glm::vec2 GenerateRandomNumberEllipse(float width, float height);

    /**
    *   \brief              Generates a random \c vec3 inside the bounds of a 3D ellipse.
    *   \param  width  [in] The width of the ellipse.
    *   \param  height [in] The height of the ellipse.
    *   \param  depth  [in] The depth of the ellipse.
    *   \return             A vec3 that is inside the bounds of the specified ellipse.
    */
    glm::vec3 GenerateRandomNumberEllipse(float width, float height, float depth);
}

#endif	// __SOUL_CORE_RANDOM_H_
