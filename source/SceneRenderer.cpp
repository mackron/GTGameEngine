// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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



    void SceneRenderer::AddExternalMesh(const SceneRendererMesh &)
    {
    }

    void SceneRenderer::RemoveExternalMesh(const SceneRendererMesh &)
    {
    }



    void SceneRenderer::SetProperty(const char*, const char*)
    {
    }

    void SceneRenderer::SetProperty(const char*, int)
    {
    }

    void SceneRenderer::SetProperty(const char*, float)
    {
    }

    void SceneRenderer::SetProperty(const char*, bool)
    {
    }

    void SceneRenderer::SetProperty(const char*, const glm::vec2 &)
    {
    }

    void SceneRenderer::SetProperty(const char*, const glm::vec3 &)
    {
    }

    void SceneRenderer::SetProperty(const char*, const glm::vec4 &)
    {
    }


    GTCore::String SceneRenderer::GetStringProperty(const char*) const
    {
        return "";
    }

    int SceneRenderer::GetIntegerProperty(const char*) const
    {
        return 0;
    }

    float SceneRenderer::GetFloatProperty(const char*) const
    {
        return 0.0f;
    }

    bool SceneRenderer::GetBooleanProperty(const char*) const
    {
        return false;
    }

    glm::vec2 SceneRenderer::GetVector2Property(const char*) const
    {
        return glm::vec2();
    }

    glm::vec3 SceneRenderer::GetVector3Property(const char*) const
    {
        return glm::vec3();
    }

    glm::vec4 SceneRenderer::GetVector4Property(const char*) const
    {
        return glm::vec4();
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