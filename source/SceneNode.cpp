
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    void SceneNodeEventHandler::OnAttach(SceneNode &, SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnDetach(SceneNode &, SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnDestroy(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnTransform(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnScale(SceneNode &, const glm::vec3 &)
    {
    }

    void SceneNodeEventHandler::OnLayerChanged(SceneNode &, unsigned int)
    {
    }

    void SceneNodeEventHandler::OnSceneChanged(SceneNode &, Scene *)
    {
    }

    void SceneNodeEventHandler::OnStaticChanged(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnVisibleChanged(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnComponentAttached(SceneNode &, Component &)
    {
    }

    void SceneNodeEventHandler::OnComponentDetached(SceneNode &, Component &)
    {
    }

    void SceneNodeEventHandler::OnUpdate(SceneNode &, double)
    {
    }
}


// *** SceneNode ***
namespace GTEngine
{
    SceneNode::SceneNode()
        : name(),
          parent(nullptr),
          firstChild(nullptr), lastChild(nullptr), prevSibling(nullptr), nextSibling(nullptr),
          eventHandlers(), components(), dataPointers(),
          layer(0),
          scene(nullptr),
          isStatic(false), isVisible(true),
          inheritPosition(true), inheritOrientation(true), inheritScale(true),
          position(), orientation(), scale(1.0f, 1.0f, 1.0f),
          eventLockCounter(0)
    {
    }

    SceneNode::~SceneNode()
    {
        this->SetScene(nullptr);

        this->DetachFromParent();
        this->OnDestroy();
        
        this->DetachAllChildren();
        this->RemoveAllComponents();
    }

    const char * SceneNode::GetName() const
    {
        return this->name.c_str();
    }

    void SceneNode::SetName(const char *newName)
    {
        this->name = newName;
    }

    SceneNode * SceneNode::GetParent()
    {
        return this->parent;
    }


    SceneNode * SceneNode::GetFirstChild()
    {
        return this->firstChild;
    }

    SceneNode * SceneNode::GetLastChild()
    {
        return this->lastChild;
    }

    SceneNode * SceneNode::GetPrevSibling()
    {
        return this->prevSibling;
    }

    SceneNode * SceneNode::GetNextSibling()
    {
        return this->nextSibling;
    }


    void SceneNode::AttachEventHandler(SceneNodeEventHandler &handler)
    {
        // If an event handler already exists, we can't attach it.
        if (this->eventHandlers.Find(&handler) == nullptr)
        {
            this->eventHandlers.Append(&handler);
        }
    }

    void SceneNode::DetachEventHandler(SceneNodeEventHandler &handler)
    {
        this->eventHandlers.Remove(this->eventHandlers.Find(&handler));
    }


    void SceneNode::AttachChild(SceneNode& child)
    {
        // If the childs parent is equal to this, it is already attached and so we don't need to do anything.
        if (child.GetParent() != this)
        {
            // If the child already has a parent, it needs to be detached.
            child.DetachFromParent();

            // If we don't have children, this will be the first one...
            if (this->firstChild == nullptr)
            {
                this->firstChild = &child;
            }
            
            // If we already have children, this child needs to be appended to the end of the children list.
            if (this->lastChild != nullptr)
            {
                auto secondLastChild = this->lastChild;

                secondLastChild->_SetNextSibling(&child);
                child._SetPrevSibling(secondLastChild);
            }
            
            // The new item is always the last one.
            child._SetNextSibling(nullptr);
            this->lastChild = &child;
            

            // The child needs to know that this is it's new parent.
            child._SetParent(this);
            
            // The event handlers need to know about the new child.
            if (!this->EventsLocked())
            {
                this->OnAttach(child);
            }

            // If we have a scene, it needs to know about the new attachment.
            child.SetScene(this->scene);
        }
    }

    void SceneNode::AttachTo(SceneNode& parent)
    {
        parent.AttachChild(*this);
    }

    void SceneNode::DetachChild(SceneNode& child)
    {
        assert(child.GetParent() == this);

        auto childPrevSibling = child.GetPrevSibling();
        auto childNextSibling = child.GetNextSibling();

        if (childPrevSibling)
        {
            childPrevSibling->_SetNextSibling(childNextSibling);
        }
        else
        {
            // The child is the root.
            this->firstChild = childNextSibling;
        }
            
        if (childNextSibling)
        {
            childNextSibling->_SetPrevSibling(childPrevSibling);
        }
        else
        {
            // The child is the end.
            this->lastChild = childPrevSibling;
        }


        // The child needs to be orphaned.
        child._SetParent(nullptr);
        child._SetPrevSibling(nullptr);
        child._SetNextSibling(nullptr);

        if (!this->EventsLocked())
        {
            this->OnDetach(child);
        }

        // It makes more sense to leave the scene as-is, so don't be tempted to set it to nullptr like we do the parent.
    }

    void SceneNode::DetachAllChildren()
    {
        while (this->firstChild != nullptr)
        {
            this->DetachChild(*this->firstChild);
        }
    }

    void SceneNode::DetachFromParent()
    {
        auto parent = this->GetParent();
        if (parent != nullptr)
        {
            parent->DetachChild(*this);
        }
    }

    SceneNode * SceneNode::FindFirstChild(const char *name, bool recursive)
    {
        for (auto iChild = this->firstChild; iChild != nullptr; iChild = iChild->GetNextSibling())
        {
            if (GTCore::Strings::Equal(iChild->GetName(), name))
            {
                return iChild;
            }
            else
            {
                if (recursive)
                {
                    iChild = iChild->FindFirstChild(name, recursive);
                    if (iChild != nullptr)
                    {
                        return iChild;
                    }
                }
            }
        }

        return nullptr;
    }

    SceneNode * SceneNode::FindFirstChildWithComponent(const char *componentName, bool recursive)
    {
        for (auto iChild = this->firstChild; iChild != nullptr; iChild = iChild->GetNextSibling())
        {
            if (iChild->HasComponent(componentName))
            {
                return iChild;
            }
            else
            {
                if (recursive)
                {
                    iChild = FindFirstChildWithComponent(componentName, recursive);
                    if (iChild != nullptr)
                    {
                        return iChild;
                    }
                }
            }
        }

        return nullptr;
    }

    void SceneNode::_SetParent(SceneNode *parent)
    {
        this->parent = parent;
    }

    void SceneNode::_SetPrevSibling(SceneNode* newPrevSibling)
    {
        this->prevSibling = newPrevSibling;
    }

    void SceneNode::_SetNextSibling(SceneNode* newNextSibling)
    {
        this->nextSibling = newNextSibling;
    }


    const glm::vec3 & SceneNode::GetPosition() const
    {
        return this->position;
    }

    void SceneNode::SetPosition(const glm::vec3 &position)
    {
        if (this->position.x != position.x || this->position.y != position.y || this->position.z != position.z)
        {
            glm::vec3 prevPosition = this->position;
            this->position = position;
            
            if (!this->EventsLocked())
            {
                this->OnTransform();
            }
        }
    }

    glm::vec3 SceneNode::GetWorldPosition() const
    {
        if (this->parent != nullptr && this->inheritPosition)
        {
            glm::quat orientation;
            if (this->inheritOrientation)
            {
                orientation = this->parent->GetWorldOrientation();
            }

            glm::vec3 scale(1.0f, 1.0f, 1.0f);
            if (this->inheritScale)
            {
                scale = this->parent->GetWorldScale();
            }

            return this->parent->GetWorldPosition() + (orientation * (scale * this->position));
        }
        
        return this->position;
    }

    void SceneNode::SetWorldPosition(const glm::vec3 &worldPosition)
    {
        if (this->parent != nullptr && this->inheritPosition)
        {
            glm::vec3 Pp = this->parent->GetWorldPosition();
            glm::vec3 Ps = this->parent->GetWorldScale();
            glm::quat Po = this->parent->GetOrientation();

            this->SetPosition(((worldPosition - Pp) * Po) / Ps);
        }
        else
        {
            this->SetPosition(worldPosition);
        }
    }

    const glm::quat & SceneNode::GetOrientation() const
    {
        return this->orientation;
    }

    void SceneNode::SetOrientation(const glm::quat &orientation)
    {
        if (this->orientation[0] != orientation[0] ||
            this->orientation[1] != orientation[1] ||
            this->orientation[2] != orientation[2] ||
            this->orientation[3] != orientation[3])
        {
            glm::quat prevOrientation = this->orientation;
            this->orientation = orientation;
            
            if (!this->EventsLocked())
            {
                this->OnTransform();
            }
        }
    }

    glm::quat SceneNode::GetWorldOrientation() const
    {
        if (this->parent != nullptr && this->inheritOrientation)
        {
            return this->parent->GetWorldOrientation() * this->orientation;
        }
        
        return this->orientation;
    }

    void SceneNode::SetWorldOrientation(const glm::quat &worldOrientation)
    {
        if (this->parent != nullptr && this->inheritOrientation)
        {
            this->SetOrientation(worldOrientation * glm::inverse(this->parent->GetWorldOrientation()));
        }
        else
        {
            this->SetOrientation(worldOrientation);
        }
    }


    const glm::vec3 & SceneNode::GetScale() const
    {
        return this->scale;
    }

    void SceneNode::SetScale(const glm::vec3 &scale)
    {
        if (this->scale.x != scale.x || this->scale.y != scale.y || this->scale.z != scale.z)
        {
            glm::vec3 prevScale = this->scale;
            this->scale = scale;

            // If we have a dynamics component, we need to apply scaling there, too.
            auto dynamicsComponent = this->GetComponent<DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                dynamicsComponent->ApplyScaling(scale.x, scale.y, scale.z);
            }
            
            if (!this->EventsLocked())
            {
                this->OnScale(prevScale);
            }
        }
    }

    glm::vec3 SceneNode::GetWorldScale() const
    {
        if (this->parent != nullptr && this->inheritScale)
        {
            return this->parent->GetWorldScale() * this->scale;
        }
        
        return this->scale;
    }

    void SceneNode::SetWorldScale(const glm::vec3 &worldScale)
    {
        if (this->parent != nullptr && this->inheritScale)
        {
            this->SetScale(worldScale / this->parent->GetWorldScale());
        }
        else
        {
            this->SetScale(worldScale);
        }
    }


    void SceneNode::LookAt(const glm::vec3 &target, const glm::vec3 &up)
    {
        glm::vec3 f = glm::normalize(target - this->GetWorldPosition());
        glm::vec3 s = glm::normalize(glm::cross(f, up));
		glm::vec3 u = glm::cross(s, f);

        glm::mat3 orientation(glm::mat3::null);
        orientation[0][0] =  s.x;
		orientation[0][1] =  s.y;
		orientation[0][2] =  s.z;
		orientation[1][0] =  u.x;
		orientation[1][1] =  u.y;
		orientation[1][2] =  u.z;
		orientation[2][0] = -f.x;
		orientation[2][1] = -f.y;
		orientation[2][2] = -f.z;

        this->SetWorldOrientation(glm::quat_cast(orientation));
    }

    void SceneNode::LookAt(const SceneNode &target, const glm::vec3 &up)
    {
        this->LookAt(target.GetWorldPosition(), up);
    }


    glm::vec3 SceneNode::GetForwardVector() const
    {
        return this->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 SceneNode::GetRightVector() const
    {
        return this->orientation * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 SceneNode::GetUpVector() const
    {
        return this->orientation * glm::vec3(0.0f, 1.0f, 0.0f);
    }


    glm::vec3 SceneNode::GetWorldForwardVector() const
    {
        return this->GetWorldOrientation() * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 SceneNode::GetWorldRightVector() const
    {
        return this->GetWorldOrientation() * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 SceneNode::GetWorldUpVector() const
    {
        return this->GetWorldOrientation() * glm::vec3(0.0f, 1.0f, 0.0f);
    }


    glm::mat4 SceneNode::GetTransformMatrix() const
    {
        return glm::translate(this->position) * glm::mat4_cast(this->orientation) * glm::scale(this->scale);
    }

    glm::mat4 SceneNode::GetWorldTransformMatrix() const
    {
        glm::vec3 position    = this->GetWorldPosition();
        glm::quat orientation = this->GetWorldOrientation();
        glm::vec3 scale       = this->GetWorldScale();
        
        return glm::translate(position) * glm::mat4_cast(orientation) * glm::scale(scale);
    }

    void SceneNode::GetWorldTransform(btTransform &worldTransform) const
    {
        glm::vec3 position    = this->GetWorldPosition();
        glm::quat orientation = this->GetWorldOrientation();

        worldTransform.setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));
        worldTransform.setOrigin(btVector3(position.x, position.y, position.z));
    }

    void SceneNode::SetWorldTransform(const btTransform &worldTransform)
    {
        // We need to lock transformation event posting because we want to do it in a single event for the sake of efficiency.
        this->LockEvents();
        {
            this->SetWorldPosition(GTEngine::ToGLMVector3(worldTransform.getOrigin()));
            this->SetWorldOrientation(GTEngine::ToGLMQuaternion(worldTransform.getRotation()));
        }
        this->UnlockEvents();

        // Now is where we post the transformation event.
        this->OnTransform();
    }


    void SceneNode::Translate(const glm::vec3 &offset)
    {
        this->SetPosition(this->position + (this->orientation * offset));
    }

    void SceneNode::Rotate(float angle, const glm::vec3 &axis)
    {
        this->SetOrientation(this->orientation * glm::angleAxis(angle, axis));
    }

    void SceneNode::Scale(const glm::vec3 &scale)
    {
        this->SetScale(this->scale * scale);
    }


    void SceneNode::TranslateWorld(const glm::vec3 &offset)
    {
        this->SetPosition(this->position + offset);
    }




    void SceneNode::SetLayer(unsigned int layer)
    {
        if (this->layer != layer)
        {
            unsigned int oldLayer = this->layer;
            this->layer = layer;

            if (!this->EventsLocked())
            {
                this->OnLayerChanged(oldLayer);
            }
        }
    }

    void SceneNode::SetScene(Scene *scene)
    {
        Scene *prevScene = this->scene;
        this->scene = scene;

        // The scene needs to be changed for the children, too.
        for (auto iChild = this->firstChild; iChild != nullptr; iChild = iChild->GetNextSibling())
        {
            iChild->SetScene(scene);
        }

        // Only if the scenes are different will we fire this event.
        if (this->scene != prevScene && !this->EventsLocked())
        {
            this->OnSceneChanged(prevScene);
        }
    }
    
    Scene * SceneNode::GetScene()
    {
        // If we don't have a scene, we will check the parents.
        if (this->scene == nullptr)
        {
            if (this->parent != nullptr)
            {
                return this->parent->GetScene();
            }
        }

        return this->scene;
    }

    void SceneNode::RemoveFromScene()
    {
        this->SetScene(nullptr);
    }

    void SceneNode::SetStatic(bool isStatic)
    {
        if (this->isStatic != isStatic)
        {
            this->isStatic = isStatic;

            if (!this->EventsLocked())
            {
                this->OnStaticChanged();
            }
        }
    }

    bool SceneNode::IsStatic() const
    {
        return this->isStatic;
    }

    bool SceneNode::IsVisible(bool recursive) const
    {
        if (!this->isVisible)
        {
            return false;
        }
        else
        {
            if (recursive && this->parent != nullptr)
            {
                return this->parent->IsVisible();
            }
            else
            {
                return true;
            }
        }
    }

    void SceneNode::SetVisible(bool isVisible)
    {
        if (this->isVisible != isVisible)
        {
            this->isVisible = isVisible;

            if (!this->EventsLocked())
            {
                this->OnVisibleChanged();
            }
        }
    }


    void SceneNode::DisablePositionInheritance()
    {
        auto worldPosition = this->GetWorldPosition();

        this->inheritPosition = false;
        this->SetWorldPosition(worldPosition);     // This will ensure the position is correct.
    }
    void SceneNode::EnablePositionInheritance()
    {
        auto worldPosition = this->GetWorldPosition();

        this->inheritPosition = true;
        this->SetWorldPosition(worldPosition);     // This will ensure the position is correct.
    }

    void SceneNode::DisableOrientationInheritance()
    {
        auto worldOrientation = this->GetWorldOrientation();
        
        this->inheritOrientation = false;
        this->SetWorldOrientation(worldOrientation);
    }
    void SceneNode::EnableOrientationInheritance()
    {
        auto worldOrientation = this->GetWorldOrientation();
        
        this->inheritOrientation = true;
        this->SetWorldOrientation(worldOrientation);
    }

    void SceneNode::DisableScaleInheritance()
    {
        auto worldScale = this->GetWorldScale();
        
        this->inheritScale = false;
        this->SetWorldScale(worldScale);
    }
    void SceneNode::EnableScaleInheritance()
    {
        auto worldScale = this->GetWorldScale();
        
        this->inheritScale = true;
        this->SetWorldScale(worldScale);
    }


    ModelComponent* SceneNode::AddModelComponent(Model * model)
    {
        auto component = this->AddComponent<ModelComponent>();
        assert(component != nullptr);

        component->SetModel(model);

        return component;
    }

    CameraComponent* SceneNode::Add3DCameraComponent(float fov, float aspect, float zNear, float zFar)
    {
        auto component = this->AddComponent<CameraComponent>();
        assert(component != nullptr);

        component->Set3DProjection(fov, aspect, zNear, zFar);

        return component;
    }

    AmbientLightComponent* SceneNode::AddAmbientLightComponent(float r, float g, float b)
    {
        return this->AddAmbientLightComponent(glm::vec3(r, g, b));
    }

    AmbientLightComponent* SceneNode::AddAmbientLightComponent(const glm::vec3 &colour)
    {
        auto component = this->AddComponent<AmbientLightComponent>();
        assert(component != nullptr);

        component->SetColour(colour);

        return component;
    }

    DirectionalLightComponent* SceneNode::AddDirectionalLightComponent(float r, float g, float b)
    {
        return this->AddDirectionalLightComponent(glm::vec3(r, g, b));
    }

    DirectionalLightComponent* SceneNode::AddDirectionalLightComponent(const glm::vec3 &colour)
    {
        auto component = this->AddComponent<DirectionalLightComponent>();
        assert(component != nullptr);

        component->SetColour(colour);

        return component;
    }

    PointLightComponent* SceneNode::AddPointLightComponent(float r, float g, float b)
    {
        return this->AddPointLightComponent(glm::vec3(r, g, b));
    }

    PointLightComponent* SceneNode::AddPointLightComponent(const glm::vec3 &colour)
    {
        auto component = this->AddComponent<PointLightComponent>();
        assert(component != nullptr);

        component->SetColour(colour);

        return component;
    }



    void SceneNode::OnAttach(SceneNode &childNode)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnAttach(*this, childNode);
        }
    }

    void SceneNode::OnDetach(SceneNode &childNode)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnDetach(*this, childNode);
        }
    }

    void SceneNode::OnDestroy()
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnDestroy(*this);
        }
    }

    void SceneNode::OnTransform()
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnTransform(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeTransform(*this);
        }
    }

    void SceneNode::OnScale(const glm::vec3 &prevScale)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnScale(*this, prevScale);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeScale(*this, prevScale);
        }
    }

    void SceneNode::OnLayerChanged(unsigned int prevLayer)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnLayerChanged(*this, prevLayer);
        }
    }

    void SceneNode::OnSceneChanged(Scene *prevScene)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnSceneChanged(*this, prevScene);
        }

        if (prevScene != nullptr)
        {
            prevScene->OnSceneNodeRemoved(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeAdded(*this);
        }
    }

    void SceneNode::OnStaticChanged()
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnStaticChanged(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeStaticChanged(*this);
        }
    }

    void SceneNode::OnVisibleChanged()
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnVisibleChanged(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeVisibleChanged(*this);
        }
    }

    void SceneNode::OnComponentAttached(Component& component)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnComponentAttached(*this, component);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeComponentAttached(*this, component);
        }
    }

    void SceneNode::OnComponentDetached(Component& component)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnComponentDetached(*this, component);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeComponentDetached(*this, component);
        }
    }

    void SceneNode::OnUpdate(double deltaTimeInSeconds)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnUpdate(*this, deltaTimeInSeconds);
        }
    }
}

// Private
namespace GTEngine
{
    void SceneNode::LockEvents()
    {
        ++this->eventLockCounter;
    }

    void SceneNode::UnlockEvents()
    {
        --this->eventLockCounter;
    }

    bool SceneNode::EventsLocked() const
    {
        return this->eventLockCounter > 0;
    }
}