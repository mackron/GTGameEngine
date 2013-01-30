
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Logging.hpp>

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

    void SceneNodeEventHandler::OnScale(SceneNode &)
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

    void SceneNodeEventHandler::OnUpdate(SceneNode &, double)
    {
    }

    void SceneNodeEventHandler::OnContact(SceneNode &, SceneNode &, const btManifoldPoint &)
    {
    }
}


// *** SceneNode ***
namespace GTEngine
{
    SceneNode::SceneNode()
        : SceneObject(SceneObjectType_SceneNode),
          name(), uniqueID(0),
          parent(nullptr),
          firstChild(nullptr), lastChild(nullptr), prevSibling(nullptr), nextSibling(nullptr),
          eventHandlers(), components(), dataPointers(),
          scene(nullptr),
          isVisible(true),
          inheritPosition(true), inheritOrientation(true), inheritScale(true),
          flags(0),
          eventLockCounter(0),
          typeID(0),
          modelComponent(nullptr), pointLightComponent(nullptr), spotLightComponent(nullptr), editorMetadataComponent(nullptr)
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

    const char* SceneNode::GetName() const
    {
        return this->name.c_str();
    }

    void SceneNode::SetName(const char* newName)
    {
        this->name = newName;
    }


    uint64_t SceneNode::GetID() const
    {
        return this->uniqueID;
    }

    void SceneNode::SetID(uint64_t newID)
    {
        this->uniqueID = newID;
    }


    SceneNode* SceneNode::GetParent()
    {
        return this->parent;
    }

    const SceneNode* SceneNode::GetParent() const
    {
        return this->parent;
    }


    SceneNode* SceneNode::GetFirstChild()
    {
        return this->firstChild;
    }

    const SceneNode* SceneNode::GetFirstChild() const
    {
        return this->firstChild;
    }


    SceneNode* SceneNode::GetLastChild()
    {
        return this->lastChild;
    }

    const SceneNode* SceneNode::GetLastChild() const
    {
        return this->lastChild;
    }


    SceneNode* SceneNode::GetPrevSibling()
    {
        return this->prevSibling;
    }

    const SceneNode* SceneNode::GetPrevSibling() const
    {
        return this->prevSibling;
    }


    SceneNode* SceneNode::GetNextSibling()
    {
        return this->nextSibling;
    }

    const SceneNode* SceneNode::GetNextSibling() const
    {
        return this->nextSibling;
    }


    SceneNode* SceneNode::GetTopAncestor()
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetTopAncestor();
        }

        return this;
    }

    const SceneNode* SceneNode::GetTopAncestor() const
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetTopAncestor();
        }

        return this;
    }


    size_t SceneNode::GetChildCount() const
    {
        size_t count = 0;

        for (auto i = this->GetFirstChild(); i != nullptr; i = i->GetNextSibling())
        {
            ++count;
        }

        return count;
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


    void SceneNode::AttachChild(SceneNode &child)
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

    void SceneNode::AttachTo(SceneNode &parent)
    {
        parent.AttachChild(*this);
    }

    void SceneNode::DetachChild(SceneNode &child)
    {
        if (child.GetParent() == this)
        {
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

    SceneNode* SceneNode::FindFirstChild(const char* name, bool recursive)
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

    SceneNode* SceneNode::FindFirstChildWithComponent(const char *componentName, bool recursive)
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

    bool SceneNode::IsAncestor(const SceneNode &other) const
    {
        if (this->parent != nullptr)
        {
            if (this->parent == &other)
            {
                return true;
            }

            return this->parent->IsAncestor(other);
        }

        return false;
    }

    bool SceneNode::IsDescendant(const SceneNode &other) const
    {
        for (auto iChild = this->firstChild; iChild != nullptr; iChild = iChild->nextSibling)
        {
            if (iChild == &other)
            {
                return true;
            }
            else
            {
                if (iChild->IsDescendant(other))
                {
                    return true;
                }
            }
        }

        return false;
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


    void SceneNode::SetPosition(const glm::vec3 &position, bool updateDynamicsObject)
    {
        if (this->position.x != position.x || this->position.y != position.y || this->position.z != position.z)
        {
            this->position = position;
            
            if (!this->EventsLocked())
            {
                this->OnTransform(updateDynamicsObject);
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

    void SceneNode::SetWorldPosition(const glm::vec3 &worldPosition, bool updateDynamicsObject)
    {
        if (this->parent != nullptr && this->inheritPosition)
        {
            glm::vec3 Pp = this->parent->GetWorldPosition();
            glm::vec3 Ps = this->parent->GetWorldScale();
            glm::quat Po = this->parent->GetOrientation();

            this->SetPosition(((worldPosition - Pp) * Po) / Ps, updateDynamicsObject);
        }
        else
        {
            this->SetPosition(worldPosition, updateDynamicsObject);
        }
    }


    void SceneNode::SetOrientation(const glm::quat &orientation, bool updateDynamicsObject)
    {
        if (this->orientation[0] != orientation[0] ||
            this->orientation[1] != orientation[1] ||
            this->orientation[2] != orientation[2] ||
            this->orientation[3] != orientation[3])
        {
            this->orientation = orientation;
            
            if (!this->EventsLocked())
            {
                this->OnTransform(updateDynamicsObject);
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

    void SceneNode::SetWorldOrientation(const glm::quat &worldOrientation, bool updateDynamicsObject)
    {
        if (this->parent != nullptr && this->inheritOrientation)
        {
            this->SetOrientation(glm::inverse(this->parent->GetWorldOrientation()) * worldOrientation, updateDynamicsObject);
        }
        else
        {
            this->SetOrientation(worldOrientation, updateDynamicsObject);
        }
    }


    void SceneNode::SetScale(const glm::vec3 &scale)
    {
        if (this->scale.x != scale.x || this->scale.y != scale.y || this->scale.z != scale.z)
        {
            this->scale = scale;

            if (!this->EventsLocked())
            {
                this->OnScale();
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
        this->SetWorldOrientation(glm::quat_cast(Math::CalculateLookAtMatrix(this->GetWorldPosition(), target, up)));
    }

    void SceneNode::LookAt(const SceneNode &target, const glm::vec3 &up)
    {
        this->LookAt(target.GetWorldPosition(), up);
    }


    void SceneNode::LookInDirection(const glm::vec3 &direction, const glm::vec3 &up)
    {
        this->LookAt(this->GetWorldPosition() + direction, up);
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


    void SceneNode::GetWorldTransformComponents(glm::vec3 &positionOut, glm::quat &orientationOut, glm::vec3 &scaleOut) const
    {
        if (this->parent != nullptr)
        {
            this->parent->GetWorldTransformComponents(positionOut, orientationOut, scaleOut);

            glm::vec3 temp;
            if (!this->inheritPosition)
            {
                positionOut = this->position;

                if (!this->inheritScale)
                {
                    scaleOut = this->scale;
                }
                else
                {
                    scaleOut = scaleOut * this->scale;
                }

                if (!this->inheritOrientation)
                {
                    orientationOut = this->orientation;
                }
                else
                {
                    orientationOut = orientationOut * this->orientation;
                }
            }
            else
            {
                glm::vec3 offset = this->position;
                
                if (!this->inheritScale)
                {
                    scaleOut = this->scale;
                }
                else
                {
                    offset   = scaleOut * offset;
                    scaleOut = scaleOut * this->scale;
                }

                if (!this->inheritOrientation)
                {
                    orientationOut = this->orientation;
                }
                else
                {
                    offset         = orientationOut * offset;
                    orientationOut = orientationOut * this->orientation;
                }

                positionOut += offset;
            }
        }
        else
        {
            positionOut    = this->position;
            orientationOut = this->orientation;
            scaleOut       = this->scale;
        }
    }


    glm::mat4 SceneNode::GetWorldTransform() const
    {
        glm::vec3 position;
        glm::quat orientation;
        glm::vec3 scale;
        this->GetWorldTransformComponents(position, orientation, scale);

        glm::mat4 result;
        Math::CalculateTransformMatrix(position, orientation, scale, result);

        return result;
    }

    glm::mat4 SceneNode::GetWorldTransformWithoutScale() const
    {
        glm::vec3 position;
        glm::quat orientation;
        glm::vec3 devnull;
        this->GetWorldTransformComponents(position, orientation, devnull);

        glm::mat4 result;
        Math::CalculateTransformMatrix(position, orientation, result);

        return result;
    }

    void SceneNode::GetWorldTransform(btTransform &worldTransform) const
    {
        glm::vec3 position;
        glm::quat orientation;
        glm::vec3 devnull;
        this->GetWorldTransformComponents(position, orientation, devnull);
        
        worldTransform.setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));
        worldTransform.setOrigin(btVector3(position.x, position.y, position.z));


        (void)devnull;  // <-- warning silencer.
    }

    void SceneNode::SetWorldTransform(const btTransform &worldTransform, bool updateDynamicsObject)
    {
        // We need to lock transformation event posting because we want to do it in a single event for the sake of efficiency.
        this->LockEvents();
        {
            this->SetWorldPosition(GTEngine::ToGLMVector3(worldTransform.getOrigin()), false);
            this->SetWorldOrientation(GTEngine::ToGLMQuaternion(worldTransform.getRotation()), false);
        }
        this->UnlockEvents();

        // Now is where we post the transformation event.
        this->OnTransform(updateDynamicsObject);
    }



    /////////////////////////////////////////
    // Components

    Component* SceneNode::GetComponentByName(const char* componentName)
    {
        if (GTCore::Strings::Equal(componentName, ModelComponent::Name))
        {
            return this->modelComponent;
        }

        if (GTCore::Strings::Equal(componentName, PointLightComponent::Name))
        {
            return this->pointLightComponent;
        }

        if (GTCore::Strings::Equal(componentName, SpotLightComponent::Name))
        {
            return this->spotLightComponent;
        }

        if (GTCore::Strings::Equal(componentName, EditorMetadataComponent::Name))
        {
            return this->editorMetadataComponent;
        }


        auto item = this->components.Find(componentName);
        if (item != nullptr)
        {
            return item->value;
        }

        return nullptr;
    }

    Component* SceneNode::AddComponentByName(const char* componentName)
    {
        Component* component = nullptr;

        if (GTCore::Strings::Equal(componentName, ModelComponent::Name))
        {
            if (this->modelComponent == nullptr)
            {
                this->modelComponent = new ModelComponent(*this);
            }
            component = this->modelComponent;
        }
        else if (GTCore::Strings::Equal(componentName, PointLightComponent::Name))
        {
            if (this->pointLightComponent == nullptr)
            {
                this->pointLightComponent = new PointLightComponent(*this);
            }
            component = this->pointLightComponent;
        }
        else if (GTCore::Strings::Equal(componentName, SpotLightComponent::Name))
        {
            if (this->spotLightComponent == nullptr)
            {
                this->spotLightComponent = new SpotLightComponent(*this);
            }
            component = this->spotLightComponent;
        }
        else if (GTCore::Strings::Equal(componentName, EditorMetadataComponent::Name))
        {
            if (this->editorMetadataComponent == nullptr)
            {
                this->editorMetadataComponent = new EditorMetadataComponent(*this);
            }
            component = this->editorMetadataComponent;
        }
        else
        {
            // A component of the same name can't already exist. If it doesn, we just return the existing one.
            component = this->GetComponentByName(componentName);
            if (component == nullptr)
            {
                component = GTEngine::CreateComponentByName(componentName, *this);
                this->components.Add(componentName, component);
            }
        }

        return component;
    }

    void SceneNode::RemoveComponentByName(const char* componentName)
    {
        auto component = this->GetComponentByName(componentName);
        if (component != nullptr)
        {
            if (component == this->modelComponent)
            {
                this->modelComponent = nullptr;
            }
            else if (component == this->pointLightComponent)
            {
                this->pointLightComponent = nullptr;
            }
            else if (component == this->spotLightComponent)
            {
                this->spotLightComponent = nullptr;
            }
            else if (component == this->editorMetadataComponent)
            {
                this->editorMetadataComponent = nullptr;
            }

            this->components.Remove(componentName);
            delete component;
        }
    }

    void SceneNode::RemoveAllComponents()
    {
        // I've had too many bugs where I've forgotten to handle the specialised cases properly, so here I'm going
        // to take it slow and do this in a way where we avoid at least one case of this kind of bug.
        //
        // What we'll do is just retrieve the names of all of the attached components, and then iterate over that
        // and remove by name.
        GTCore::Vector<GTCore::String> componentNames;
        this->GetAttachedComponentNames(componentNames);

        for (size_t i = 0; i < componentNames.count; ++i)
        {
            this->RemoveComponentByName(componentNames[i].c_str());
        }


        // And now at the end we'll assert that we have no components.
        assert(this->components.count == 0);
    }

    void SceneNode::GetAttachedComponentNames(GTCore::Vector<GTCore::String> &output) const
    {
        // First is our specialised cases.
        if (this->modelComponent          != nullptr) { output.PushBack(ModelComponent::Name);          }
        if (this->pointLightComponent     != nullptr) { output.PushBack(PointLightComponent::Name);     }
        if (this->spotLightComponent      != nullptr) { output.PushBack(SpotLightComponent::Name);      }
        if (this->editorMetadataComponent != nullptr) { output.PushBack(EditorMetadataComponent::Name); }


        // Now we can just read the rest from the map.
        for (size_t i = 0; i < this->components.count; ++i)
        {
            output.PushBack(this->components.buffer[i]->key);
        }
    }






    void SceneNode::SetScene(Scene *scene)
    {
        auto prevScene = this->scene;
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

    void SceneNode::Refresh()
    {
        if (this->scene != nullptr)
        {
            this->scene->RefreshObject(*this);
        }
    }


    void SceneNode::SetStatic(bool isStaticIn)
    {
        if (isStaticIn != this->IsStatic())
        {
            if (isStaticIn)
            {
                this->flags = this->flags | Static;
            }
            else
            {
                this->flags = this->flags &~Static;
            }


            if (!this->EventsLocked())
            {
                this->OnStaticChanged();
            }
        }
    }

    bool SceneNode::IsStatic() const
    {
        return (this->flags & Static) != 0;
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



    //////////////////////////////////////////////////
    // Serialization/Deserialization.

    void SceneNode::Serialize(GTCore::Serializer &serializer) const
    {
        // First, we serialize the SceneObject.
        SceneObject::Serialize(serializer);

        
        // The first scene node chunk, besides SceneObject, is the general attributes. We're going to use an intermediate serializer here
        // because we're writing a string.
        GTCore::BasicSerializer secondarySerializer;
        secondarySerializer.Write(this->uniqueID);
        secondarySerializer.Write(this->name);
        secondarySerializer.Write(this->isVisible);
        secondarySerializer.Write(this->inheritPosition);
        secondarySerializer.Write(this->inheritOrientation);
        secondarySerializer.Write(this->inheritScale);
        secondarySerializer.Write(static_cast<uint32_t>(this->flags));
        secondarySerializer.Write(static_cast<uint32_t>(this->typeID));


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SceneNode_General;
        header.version     = 1;
        header.sizeInBytes = secondarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(secondarySerializer.GetBuffer(), header.sizeInBytes);



        // The next chunk contains our components. What we want to in deserialization is the ability to skip over unknown components. To do this,
        // we need to employ a mechanism to allow the deserializer to skip over. The most intuitive way is to just supply a variable containing
        // the size of the component data, which is what we'll be doing.
        //
        // We need to use an intermediary serializer to do this so we can obtain an exact chunk size.
        GTCore::BasicSerializer componentSerializer;

        GTCore::Vector<GTCore::String> componentNames;
        this->GetAttachedComponentNames(componentNames);

        // We first write the number of components we are saving.
        componentSerializer.Write(static_cast<uint32_t>(componentNames.count));

        // Now we need to loop over every component. Because we are saving the size of the data before the actual data itself, we need to use
        // yet another intermediary serializer so we can retrieve the size.
        for (size_t iComponent = 0; iComponent < componentNames.count; ++iComponent)
        {
            auto component = this->GetComponentByName(componentNames[iComponent].c_str());
            assert(component != nullptr);
            {
                GTCore::BasicSerializer componentSubSerializer;
                component->Serialize(componentSubSerializer);

                // We write the name, the size, then the actual data.
                componentSerializer.WriteString(component->GetName());
                componentSerializer.Write(static_cast<uint32_t>(componentSubSerializer.GetBufferSizeInBytes()));
                componentSerializer.Write(componentSubSerializer.GetBuffer(), componentSubSerializer.GetBufferSizeInBytes());
            }
        }



        header.id          = Serialization::ChunkID_SceneNode_Components;
        header.version     = 1;
        header.sizeInBytes = componentSerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(componentSerializer.GetBuffer(), header.sizeInBytes);
    }

    void SceneNode::Deserialize(GTCore::Deserializer &deserializer)
    {
        // For now, we want to remove the scene node before deserializing. We then re-add it afterwards.
        auto scene = this->GetScene();
        if (scene != nullptr)
        {
            scene->RemoveSceneNode(*this);
        }


        // Deserialize the SceneObject first.
        SceneObject::Deserialize(deserializer);

        Serialization::ChunkHeader header;
        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneNode_General);
            {
                switch (header.version)
                {
                case 1:
                    {
                        deserializer.Read(this->uniqueID);
                        deserializer.Read(this->name);
                        deserializer.Read(this->isVisible);
                        deserializer.Read(this->inheritPosition);
                        deserializer.Read(this->inheritOrientation);
                        deserializer.Read(this->inheritScale);
                        deserializer.Read(reinterpret_cast<uint32_t &>(this->flags));
                        deserializer.Read(reinterpret_cast<uint32_t &>(this->typeID));

                        break;
                    }

                default:
                    {
                        GTEngine::Log("Error deserializing SceneNode. The main chunk is an unsupported version (%d).", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        break;
                    }
                }
            }
        }

        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneNode_Components);
            {
                // We need to remove components that were not part of the serialized data. Otherwise, the scene node won't be in the correct state.
                GTCore::Vector<GTCore::String> componentsToRemove;
                this->GetAttachedComponentNames(componentsToRemove);


                uint32_t componentCount;
                deserializer.Read(componentCount);

                for (uint32_t iComponent = 0; iComponent < componentCount; ++iComponent)
                {
                    GTCore::String name;
                    deserializer.Read(name);

                    uint32_t componentDataSizeInBytes;
                    deserializer.Read(componentDataSizeInBytes);

                    auto component = this->AddComponentByName(name.c_str());
                    if (component != nullptr)
                    {
                        component->Deserialize(deserializer);
                    }
                    else
                    {
                        // If we get here it means the component is unknown to both the engine and the client application. For now we're going to 
                        // just skip over the data, but in the future what we'll do is keep hold of it (placing it into a separate list) so that
                        // future serializations can keep hold of the data rather than losing it.
                        deserializer.Seek(componentDataSizeInBytes);
                    }


                    // We don't want to remove this component later on.
                    componentsToRemove.RemoveFirstOccuranceOf(name);
                }


                // Now we need to remove some no-longer-used components.
                for (size_t i = 0; i < componentsToRemove.count; ++i)
                {
                    this->RemoveComponentByName(componentsToRemove[i].c_str());
                }
            }
        }


        // Now we need to re-add the node.
        if (scene != nullptr)
        {
            scene->AddSceneNode(*this);
        }
    }

    void SceneNode::DisableSerialization()
    {
        this->flags = this->flags | NoSerialization;
    }

    void SceneNode::EnableSerialization()
    {
        this->flags = this->flags & ~NoSerialization;
    }

    bool SceneNode::IsSerializationEnabled() const
    {
        bool locallyEnabled = (this->flags & NoSerialization) == 0;

        if (locallyEnabled && this->parent != nullptr)
        {
            return this->parent->IsSerializationEnabled();
        }
        else
        {
            return locallyEnabled;
        }
    }


    void SceneNode::DisableStateStackStaging()
    {
        this->flags = this->flags | NoStateStackStaging;
    }

    void SceneNode::EnableStateStackStaging()
    {
        this->flags = this->flags & ~NoStateStackStaging;
    }

    bool SceneNode::IsStateStackStagingEnabled() const
    {
        bool locallyEnabled = (this->flags & NoStateStackStaging) == 0;

        if (locallyEnabled && this->parent != nullptr)
        {
            return this->parent->IsStateStackStagingEnabled();
        }
        else
        {
            return locallyEnabled;
        }
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

    void SceneNode::OnTransform(bool updateDynamicsObject)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnTransform(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeTransform(*this, updateDynamicsObject);
        }


        // When the transformation changes on a parent, the children will also have changed if they have inheritance.
        for (auto i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            if (i->IsPositionInheritanceEnabled() || i->IsOrientationInheritanceEnabled())
            {
                i->OnTransform(updateDynamicsObject);
            }
        }
    }

    void SceneNode::OnScale()
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnScale(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeScale(*this);
        }


        // When the scale changes on a parent, the children will also have changed scales if they have inheritted scaling enabled.
        for (auto i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            if (i->IsScaleInheritanceEnabled())
            {
                i->OnScale();
            }
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

        // When the visibility of a node changes, there is a chance the visibility of the child has also changed. We are going to call the OnVisibleChange is also changing.
        for (auto i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            i->OnVisibleChanged();
        }
    }

    void SceneNode::OnUpdate(double deltaTimeInSeconds)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnUpdate(*this, deltaTimeInSeconds);
        }
    }

    void SceneNode::OnContact(SceneNode &other, const btManifoldPoint &pt)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnContact(*this, other, pt);
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
