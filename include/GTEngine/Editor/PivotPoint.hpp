// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "../Math.hpp"

namespace GTEngine
{
    /// A pivot point is used for doing translation, rotation and scaling of objects.
    ///
    /// A pivot point is always in world space.
    class PivotPoint
    {
    public:

        /// Constructor.
        PivotPoint();

        /// Copy constructor.
        PivotPoint(const PivotPoint &other);

        /// Destructor.
        ~PivotPoint();


        /////////////////////////////////////////////
        // Transforming

        /// Sets the position of the pivot point.
        ///
        /// @param position [in] The new position of the pivot point, in world space.
        void SetPosition(const glm::vec3 &position);
        void SetPosition(const float x, const float y, const float z) { this->SetPosition(glm::vec3(x, y, z)); }

        // Retrieves the position of the pivot point.
        const glm::vec3 & GetPosition() const;


        /// Sets the orientation of the pivot point.
        ///
        /// @param orientation [in] The new orientation of the pivot point.
        void SetOrientation(const glm::quat &orientation);

        /// Retrieves the orientation of the pivot point.
        const glm::quat & GetOrientation() const;


        /// Sets the scale of the pivot point.
        ///
        /// @param scale [in] The new scale of the pivot point.
        void SetScale(const glm::vec3 &scale);
        void SetScale(const float x, const float y, const float z) { this->SetScale(glm::vec3(x, y, z)); }

        /// Retrieves the scale of the pivot point.
        const glm::vec3 & GetScale() const;



        /// Translates the pivot point.
        ///
        /// @param translation [in] The amount of translation to apply.
        ///
        /// @remarks
        ///     This is done in global space.
        void Translate(const glm::vec3 &translation);



        /////////////////////////////////////////////
        // Snapping Control

        /// Enables translation snapping.
        void EnableTranslationSnapping();

        /// Disables translation snapping.
        void DisableTranslationSnapping();

        /// Determines whether or not translation snapping is enabled.
        bool IsTranslationSnappingEnabled() const;


        /// Enables rotation snapping.
        void EnableRotationSnapping();

        /// Disables rotation snapping.
        void DisableRotationSnapping();

        /// Determines whether or not rotation snapping is enabled.
        bool IsRotationSnappingEnabled() const;


        /// Enables scale snapping.
        void EnableScaleSnapping();

        /// Disables scale snapping.
        void DisableScaleSnapping();

        /// Determines whether or not scale snapping is enabled.
        bool IsScaleSnappingEnabled() const;


        /// Enables snapping on all transformation forms.
        void EnableAllSnapping();

        /// Disables snapping on all transformation forms.
        void DisableAllSnapping();



        /// Sets the translation snapping interval.
        ///
        /// @param interval [in] The new snapping interval for translations.
        void SetTranslationSnappingInterval(float interval);

        /// Retrieves the translation snapping interval.
        float GetTranslationSnappingInterval() const;


        /// Sets the rotation snapping interval.
        ///
        /// @param interval [in] The new snapping interval for rotations, in degrees.
        void SetRotationSnappingInterval(float interval);

        /// Retrieves the rotation snapping interval.
        float GetRotationSnappingInterval() const;


        /// Sets the scale snapping interval.
        ///
        /// @param interval [in] The new snapping interval for scales.
        void SetScaleSnappingInterval(float interval);

        /// Retrieves the scale snapping interval.
        float GetScaleSnappingInterval() const;



    private:
        
        /// The position of the pivot point in world space.
        glm::vec3 m_position;

        /// The orientation of the pivot point.
        glm::quat m_orientation;

        /// The scale of the pivot point.
        glm::vec3 m_scale;


        /// The imaginary position used for translation snapping.
        glm::vec3 m_imaginaryPosition;

        /// The imaginary rotation used for rotation snapping.
        glm::quat m_imaginaryOrientation;

        /// The imaginary scale used for scale snapping.
        glm::vec3 m_imaginaryScale;


        /// The snapping interval for translations.
        float m_translationSnappingInterval;

        /// The snapping interval for rotations.
        float m_rotationSnappingInterval;

        /// The snapping interval for scaling.
        float m_scaleSnappingInterval;


        /// Keeps track of whether or not snapping is enable for translations.
        bool m_isTranslationSnappingEnabled;

        /// Keeps track of whether or not snapping is enabled for orientations.
        bool m_isRotationSnappingEnabled;

        /// Keeps track of whether or not snapping is enabled for scaling.
        bool m_isScaleSnappingEnabled;

        /// Keeps track of whether or not to snap to the grid.
        bool m_snapToGrid;
    };
}