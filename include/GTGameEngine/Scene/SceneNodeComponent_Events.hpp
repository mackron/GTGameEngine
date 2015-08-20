// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeComponent_Events_hpp_
#define __GT_SceneNodeComponent_Events_hpp_

#include "SceneNodeComponent.hpp"
#include "SceneNodeComponentDescriptor_Events.hpp"
#include <functional>

namespace GT
{
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
