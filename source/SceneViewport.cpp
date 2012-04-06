
#include <GTEngine/SceneViewport.hpp>
#include <GTEngine/ViewportRenderer.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    SceneViewport::SceneViewport(unsigned int width, unsigned int height)
        : scene(nullptr), cameraNode(nullptr), renderer(nullptr), width(width), height(height),
          modelNodes(), ambientLightNodes(), directionalLightNodes(), pointLightNodes()
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


    GTCore::Vector<SceneNode*> & SceneViewport::GetModelNodes()
    {
        return this->modelNodes;
    }

    GTCore::Vector<SceneNode*> & SceneViewport::GetAmbientLightNodes()
    {
        return this->ambientLightNodes;
    }

    GTCore::Vector<SceneNode*> & SceneViewport::GetDirectionalLightNodes()
    {
        return this->directionalLightNodes;
    }

    GTCore::Vector<SceneNode*> & SceneViewport::GetPointLightNodes()
    {
        return this->pointLightNodes;
    }


    void SceneViewport::ClearVisibleNodes()
    {
        //this->visibleNodes.Clear();
        this->modelNodes.Clear();
        this->ambientLightNodes.Clear();
        this->directionalLightNodes.Clear();
        this->pointLightNodes.Clear();
    }

    void SceneViewport::AddVisibleNode(SceneNode &node)
    {
        // Note here how we're not usine 'else' statements. It entirely acceptable for a light to be a combination or
        // ambient, directional and point. For example, it makes sense for a Sun light to have both a directional and
        // ambient component.
        if (node.HasComponent<AmbientLightComponent>())
        {
            this->ambientLightNodes.PushBack(&node);
        }

        if (node.HasComponent<DirectionalLightComponent>())
        {
            this->directionalLightNodes.PushBack(&node);
        }

        if (node.HasComponent<PointLightComponent>())
        {
            this->pointLightNodes.PushBack(&node);
        }


        if (node.HasComponent<ModelComponent>())
        {
            this->modelNodes.PushBack(&node);
        }
    }


    void SceneViewport::Render()
    {
        if (this->renderer != nullptr)
        {
            this->renderer->Render();
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
}
