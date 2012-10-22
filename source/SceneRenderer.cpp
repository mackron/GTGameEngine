
#include <GTEngine/SceneRenderer.hpp>

namespace GTEngine
{
    SceneRenderer::SceneRenderer()
        : isColourClearingEnabled(true), clearColour(0.0f, 0.0f, 0.0f)
    {
    }

    SceneRenderer::~SceneRenderer()
    {
    }


    void SceneRenderer::EnableBackgroundColourClearing(float r, float g, float b)
    {
        this->isColourClearingEnabled = true;
        this->clearColour             = glm::vec3(r, g, b);
    }

    void SceneRenderer::DisableBackgroundColourClearing()
    {
        this->isColourClearingEnabled = false;
    }

    bool SceneRenderer::IsBackgroundColourClearingEnabled() const
    {
        return this->isColourClearingEnabled;
    }

    const glm::vec3 & SceneRenderer::GetBackgroundClearColour() const
    {
        return this->clearColour;
    }
}