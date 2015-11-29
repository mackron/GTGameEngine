// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/PivotPoint.hpp>

namespace GT
{
    PivotPoint::PivotPoint()
        : m_position(),
          m_orientation(),
          m_scale(1.0f, 1.0f, 1.0f),
          m_imaginaryPosition(m_position),
          m_imaginaryOrientation(m_orientation),
          m_imaginaryScale(m_scale),
          m_translationSnappingInterval(0.25f),
          m_rotationSnappingInterval(27.5f),
          m_scaleSnappingInterval(0.25f),
          m_isTranslationSnappingEnabled(false),
          m_isRotationSnappingEnabled(false),
          m_isScaleSnappingEnabled(false),
          m_snapToGrid(true)
    {
    }

    PivotPoint::PivotPoint(const PivotPoint &other)
        : m_position(other.GetPosition()),
          m_orientation(other.GetOrientation()),
          m_scale(other.GetScale()),
          m_imaginaryPosition(other.m_imaginaryPosition),
          m_imaginaryOrientation(other.m_imaginaryOrientation),
          m_imaginaryScale(other.m_imaginaryScale),
          m_translationSnappingInterval(other.m_translationSnappingInterval),
          m_rotationSnappingInterval(other.m_rotationSnappingInterval),
          m_scaleSnappingInterval(other.m_scaleSnappingInterval),
          m_isTranslationSnappingEnabled(other.m_isTranslationSnappingEnabled),
          m_isRotationSnappingEnabled(other.m_isRotationSnappingEnabled),
          m_isScaleSnappingEnabled(other.m_isScaleSnappingEnabled),
          m_snapToGrid(other.m_snapToGrid)
    {
    }

    PivotPoint::~PivotPoint()
    {
    }



    /////////////////////////////////////////////////
    // Transforming

    void PivotPoint::SetPosition(const glm::vec3 &position)
    {
        m_position          = position;
        m_imaginaryPosition = position;
    }

    const glm::vec3 & PivotPoint::GetPosition() const
    {
        return m_position;
    }


    void PivotPoint::SetOrientation(const glm::quat &orientation)
    {
        m_orientation          = orientation;
        m_imaginaryOrientation = orientation;
    }

    const glm::quat & PivotPoint::GetOrientation() const
    {
        return m_orientation;
    }


    void PivotPoint::SetScale(const glm::vec3 &scale)
    {
        m_scale          = scale;
        m_imaginaryScale = scale;
    }

    const glm::vec3 & PivotPoint::GetScale() const
    {
        return m_scale;
    }



    void PivotPoint::Translate(const glm::vec3 &translation)
    {
        // If snapping is enabled, we actually just change the imaginary position. We only update the actual position if
        // it hits a snapping interval.
        if (this->IsTranslationSnappingEnabled())
        {
            m_imaginaryPosition += translation;

            // We need to check if we've passed the snapping threshold. If so, we need to update the real position. Snapping to the grid in the
            // context of this pivot point simply means that our position will always be easily divisible by the snapping interval.
            if (m_snapToGrid)
            {
                // It's snapping to the grid. We will do this on an axis-by-axis basis. We need to get the closest grid position to the axis and
                // set the position to that.

                if (glm::distance(m_imaginaryPosition.x, m_position.x) >= m_translationSnappingInterval)
                {
                    float closestGridPosition = m_translationSnappingInterval * (glm::round(m_imaginaryPosition.x / m_translationSnappingInterval));

                    m_position.x          = closestGridPosition;
                    m_imaginaryPosition.x = m_position.x;
                }

                if (glm::distance(m_imaginaryPosition.y, m_position.y) >= m_translationSnappingInterval)
                {
                    float closestGridPosition = m_translationSnappingInterval * (glm::round(m_imaginaryPosition.y / m_translationSnappingInterval));

                    m_position.y          = closestGridPosition;
                    m_imaginaryPosition.y = m_position.y;
                }

                if (glm::distance(m_imaginaryPosition.z, m_position.z) >= m_translationSnappingInterval)
                {
                    float closestGridPosition = m_translationSnappingInterval * (glm::round(m_imaginaryPosition.z / m_translationSnappingInterval));

                    m_position.z          = closestGridPosition;
                    m_imaginaryPosition.z = m_position.z;
                }
            }
            else
            {
                // It's not snapping to the grid, so we will base this off the distance to the real position.
                const float distanceToRealPosition = glm::distance(m_imaginaryPosition, m_position);
                if (distanceToRealPosition >= m_translationSnappingInterval)
                {
                    const float snappingIntervals = glm::round(distanceToRealPosition / m_translationSnappingInterval);

                    m_position          = m_position + glm::normalize(translation) * m_translationSnappingInterval * snappingIntervals;
                    m_imaginaryPosition = m_position;
                }
            }
        }
        else
        {
            m_position += translation;
            m_imaginaryPosition = m_position;
        }
    }

    void PivotPoint::Rotate(const glm::quat &rotation)
    {
        // No support for snapping for the moment.
        m_orientation          = m_orientation * rotation;
        m_imaginaryOrientation = m_orientation;
    }

    void PivotPoint::AdditiveScale(const glm::vec3 &additiveScale)
    {
        m_scale += additiveScale;
        m_imaginaryScale = m_scale;
    }



    /////////////////////////////////////////////////
    // Snapping Control

    void PivotPoint::EnableTranslationSnapping()
    {
        m_isTranslationSnappingEnabled = true;
    }

    void PivotPoint::DisableTranslationSnapping()
    {
        m_isTranslationSnappingEnabled = false;
    }

    bool PivotPoint::IsTranslationSnappingEnabled() const
    {
        return m_isTranslationSnappingEnabled && m_translationSnappingInterval > 0.0f;
    }


    void PivotPoint::EnableRotationSnapping()
    {
        m_isRotationSnappingEnabled = true;
    }

    void PivotPoint::DisableRotationSnapping()
    {
        m_isRotationSnappingEnabled = false;
    }

    bool PivotPoint::IsRotationSnappingEnabled() const
    {
        return m_isRotationSnappingEnabled && m_rotationSnappingInterval > 0.0f;
    }


    void PivotPoint::EnableScaleSnapping()
    {
        m_isScaleSnappingEnabled = true;
    }

    void PivotPoint::DisableScaleSnapping()
    {
        m_isScaleSnappingEnabled = false;
    }

    bool PivotPoint::IsScaleSnappingEnabled() const
    {
        return m_isScaleSnappingEnabled && m_scaleSnappingInterval > 0.0f;
    }


    void PivotPoint::EnableAllSnapping()
    {
        this->EnableTranslationSnapping();
        this->EnableRotationSnapping();
        this->EnableScaleSnapping();
    }

    void PivotPoint::DisableAllSnapping()
    {
        this->DisableTranslationSnapping();
        this->DisableRotationSnapping();
        this->DisableScaleSnapping();
    }



    void PivotPoint::SetTranslationSnappingInterval(float interval)
    {
        m_translationSnappingInterval = interval;
    }

    float PivotPoint::GetTranslationSnappingInterval() const
    {
        return m_translationSnappingInterval;
    }


    void PivotPoint::SetRotationSnappingInterval(float interval)
    {
        m_rotationSnappingInterval = interval;
    }

    float PivotPoint::GetRotationSnappingInterval() const
    {
        return m_rotationSnappingInterval;
    }


    void PivotPoint::SetScaleSnappingInterval(float interval)
    {
        m_scaleSnappingInterval = interval;
    }

    float PivotPoint::GetScaleSnappingInterval() const
    {
        return m_scaleSnappingInterval;
    }



    void PivotPoint::EnableSnapToGrid()
    {
        m_snapToGrid = true;
    }

    void PivotPoint::DisableSnapToGrid()
    {
        m_snapToGrid = false;
    }

    bool PivotPoint::IsSnappingToGrid() const
    {
        return m_snapToGrid;
    }
}