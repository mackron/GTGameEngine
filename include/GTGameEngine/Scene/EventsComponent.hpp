// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneNodeComponent_Events
#define GT_SceneNodeComponent_Events

#include "SceneNodeComponent.hpp"
#include <functional>

namespace GT
{
    class SceneNodeComponentDescriptor_Events : public SceneNodeComponentDescriptor
    {
    public:

        /// Retrieves the type ID of the component.
        static SceneNodeComponentTypeID TypeID();

        /// Retrieves the name of the component.
        static const char* Name();



    public:

        /// Constructor.
        SceneNodeComponentDescriptor_Events();

        /// Destructor.
        virtual ~SceneNodeComponentDescriptor_Events();


        /// Retrieves the unique ID of the component.
        virtual SceneNodeComponentTypeID GetTypeID() const;

        /// Retrieves the name of the component.
        virtual const char* GetName() const;


        /// @copydoc SceneNodeComponentDescriptor::CreateComponent()
        virtual SceneNodeComponent* CreateComponent() const;

        /// @copydoc SceneNodeComponentDescriptor::DeleteComponent()
        virtual void DeleteComponent(SceneNodeComponent* pComponent) const;
    };





    class SceneNodeComponent_Events : public SceneNodeComponent
    {
    public:

        typedef std::function<void (double deltaTimeInSeconds)> OnUpdateProc;
        typedef std::function<void (double deltaTimeInSeconds)> OnPostUpdateProc;


    public:

        /// Constructor.
        SceneNodeComponent_Events(const SceneNodeComponentDescriptor_Events &descriptor);

        /// Destructor.
        ~SceneNodeComponent_Events();


        /// Determines whether or not the OnUpdate event is enabled.
        bool IsOnUpdateEnabled() const;

        /// Determines whether or not the OnPostUpdate event is enabled.
        bool IsOnPostUpdateEnabled() const;


        /// Sets the OnUpdate event handler.
        void OnUpdate(OnUpdateProc proc);

        /// Sets the OnPostUpdate event handler.
        void OnPostUpdate(OnPostUpdateProc proc);


        /// Posts the OnUpdate event.
        void PostOnUpdate(double deltaTimeInSeconds);

        /// Posts the OnPostUpdate event.
        void PostOnPostUpdate(double deltaTimeInSeconds);




        ///////////////////////////////////////////
        // Statics

        static SceneNodeComponentTypeID GetTypeID() {
            return SceneNodeComponentDescriptor_Events::TypeID();
        }

        static const char* GetName() {
            return SceneNodeComponentDescriptor_Events::Name();
        }



    private:

        /// The function to call for OnUpdate events.
        OnUpdateProc m_onUpdate;

        /// The function to call for OnPostUpdate events.
        OnPostUpdateProc m_onPostUpdate;
    };
}

#endif
