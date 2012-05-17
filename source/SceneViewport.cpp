
#include <GTEngine/SceneViewport.hpp>
#include <GTEngine/ViewportRenderer.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    SceneViewport::SceneViewport(unsigned int width, unsigned int height)
        : scene(nullptr), cameraNode(nullptr), renderer(nullptr), width(width), height(height),
          modelComponents(), ambientLightComponents(), directionalLightComponents(), pointLightComponents()
    {
    }

    SceneViewport::~SceneViewport()
    {
    }


    void SceneViewport::SetScene(Scene* scene)
    {
        this->scene = scene;
    }

    Scene* SceneViewport::GetScene()
    {
        return this->scene;
    }


    void SceneViewport::SetCameraNode(SceneNode* cameraNode)
    {
        this->cameraNode = cameraNode;
    }

    SceneNode* SceneViewport::GetCameraNode()
    {
        return this->cameraNode;
    }
    const SceneNode* SceneViewport::GetCameraNode() const
    {
        return this->cameraNode;
    }


    void SceneViewport::SetRenderer(ViewportRenderer* renderer)
    {
        // First we need to detach the old renderer...
        if (this->renderer != nullptr)
        {
            this->renderer->SetOwnerViewport(nullptr);
        }

        // And now we set the new renderer. The new renderer needs to be resized correctly.
        this->renderer = renderer;

        if (this->renderer != nullptr)
        {
            this->renderer->SetOwnerViewport(this);
            this->renderer->ResizeFramebuffer(this->width, this->height);
        }
    }

    ViewportRenderer* SceneViewport::GetRenderer()
    {
        return this->renderer;
    }


    unsigned int SceneViewport::GetWidth() const
    {
        return this->width;
    }

    unsigned int SceneViewport::GetHeight() const
    {
        return this->height;
    }

    void SceneViewport::Resize(unsigned int newWidth, unsigned int newHeight)
    {
        this->width  = GTCore::Max(newWidth, 1U);
        this->height = GTCore::Max(newHeight, 1U);

        if (this->renderer != nullptr)
        {
            this->renderer->ResizeFramebuffer(this->width, this->height);
        }
    }



    void SceneViewport::AddModelComponent(ModelComponent &component)
    {
        this->modelComponents.PushBack(&component);
    }

    void SceneViewport::AddAmbientLightComponent(AmbientLightComponent &component)
    {
        this->ambientLightComponents.PushBack(&component);
    }

    void SceneViewport::AddDirectionalLightComponent(DirectionalLightComponent &component)
    {
        this->directionalLightComponents.PushBack(&component);
    }

    void SceneViewport::AddPointLightComponent(PointLightComponent &component)
    {
        this->pointLightComponents.PushBack(&component);
    }


    void SceneViewport::Render()
    {
        if (this->renderer != nullptr)
        {
            // We first need to update the rendering data. This will retrieve all of the components that are required for drawing the scene from
            // this viewport's perspective.
            this->UpdateRenderingData();

            // Now that we have the rendered components cached, we can render the viewport.
            this->renderer->Render();


            // TESTING
            //printf("Point Light Count: %d\n", this->pointLightComponents.count);
            //printf("Model Count: %d\n", this->modelComponents.count);
        }
    }

    Framebuffer* SceneViewport::GetFramebuffer()
    {
        if (this->renderer != nullptr)
        {
            return &this->renderer->GetFramebuffer();
        }

        return nullptr;
    }

    Texture2D* SceneViewport::GetColourOutputBuffer()
    {
        if (this->renderer != nullptr)
        {
            return this->renderer->GetFinalColourOutputBuffer();
        }

        return nullptr;
    }

    Texture2D* SceneViewport::GetDepthStencilOutputBuffer()
    {
        if (this->renderer != nullptr)
        {
            return this->renderer->GetFinalDepthStencilOutputBuffer();
        }

        return nullptr;
    }


    /*** Picking ***/
    void SceneViewport::CalculatePickingRay(int x, int y, glm::vec3 &rayNear, glm::vec3 &rayFar)
    {
        auto viewportCamera = this->cameraNode->GetComponent<GTEngine::CameraComponent>();
        assert(viewportCamera != nullptr);

        const glm::mat4 &projection = viewportCamera->GetProjectionMatrix();
              glm::mat4 view        = viewportCamera->GetViewMatrix();

        glm::vec3 winCoords;
        winCoords.x = static_cast<float>(x);
        winCoords.y = static_cast<float>(this->height) - static_cast<float>(y);

        glm::uvec4 viewportCoords(0, 0, this->width, this->height);

        winCoords.z = 0.0f;
        rayNear = glm::unProject(winCoords, view, projection, viewportCoords);

        winCoords.z = 1.0f;
        rayFar  = glm::unProject(winCoords, view, projection, viewportCoords);
    }

    SceneNode* SceneViewport::PickSceneNode(int x, int y)
    {
        glm::vec3 rayNear, rayFar;
        this->CalculatePickingRay(x, y, rayNear, rayFar);

        if (this->scene != nullptr)
        {
            return this->scene->PickSceneNode(rayNear, rayFar);
        }

        return nullptr;
    }


    /*** Misc Stuff ***/
    glm::vec3 SceneViewport::Project(const glm::vec3 &position)
    {
        auto viewportCamera = this->cameraNode->GetComponent<GTEngine::CameraComponent>();
        assert(viewportCamera != nullptr);

        return glm::project(position, viewportCamera->GetViewMatrix(), viewportCamera->GetProjectionMatrix(), glm::uvec4(0, 0, this->width, this->height));
    }

    glm::mat4 SceneViewport::Get2DProjectionMatrix(bool yDown)
    {
        if (!yDown)
        {
            return glm::ortho(0.0f, static_cast<float>(this->width), 0.0f, static_cast<float>(this->height));
        }
        else
        {
            return glm::ortho(0.0f, static_cast<float>(this->width), static_cast<float>(this->height), 0.0f);
        }
    }
}

// Private
namespace GTEngine
{
    void SceneViewport::UpdateRenderingData()
    {
        this->ClearRenderingData();
        
        if (this->scene != nullptr)
        {
            this->scene->AddVisibleComponents(*this);
        }
    }

    void SceneViewport::ClearRenderingData()
    {
        this->modelComponents.Clear();
        this->ambientLightComponents.Clear();
        this->directionalLightComponents.Clear();
        this->pointLightComponents.Clear();
    }
}
